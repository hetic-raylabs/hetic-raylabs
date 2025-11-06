# ------------------------------------------------------------------------------
# Base image
# ------------------------------------------------------------------------------
FROM ubuntu:24.04

# ------------------------------------------------------------------------------
# Global environment
# ------------------------------------------------------------------------------
ENV DEBIAN_FRONTEND=noninteractive \
    TZ=Etc/UTC \
    CC=clang \
    CXX=clang++ \
    CMAKE_GENERATOR=Ninja \
    LANG=C.UTF-8 \
    LC_ALL=C.UTF-8

# ------------------------------------------------------------------------------
# System toolchain & utilities
# ------------------------------------------------------------------------------
RUN apt-get update && apt-get upgrade -y && \
    apt-get install -y --no-install-recommends \
    build-essential cmake ninja-build pkg-config \
    clang llvm lldb gdb gdbserver \
    clang-tidy clang-format \
    ccache git curl ca-certificates \
    python3 python3-venv \
    unzip zip sudo gosu tini && \
    rm -rf /var/lib/apt/lists/*

# ------------------------------------------------------------------------------
# Conan 2.x in an isolated virtual environment
# ------------------------------------------------------------------------------
RUN python3 -m venv /opt/conan && \
    /opt/conan/bin/pip install --no-cache-dir "conan>=2.4,<3" && \
    ln -s /opt/conan/bin/conan /usr/local/bin/conan

# ------------------------------------------------------------------------------
# Build args for user identity
# ------------------------------------------------------------------------------
ARG USERNAME=dev
ARG UID=1000
ARG GID=1000

# ------------------------------------------------------------------------------
# Create real user/group (idempotent), sudo (nopasswd), workspace
# ------------------------------------------------------------------------------
RUN set -eux; \
    if ! getent group "${GID}" >/dev/null; then groupadd -g "${GID}" "${USERNAME}"; fi; \
    if ! getent passwd "${UID}" >/dev/null; then \
    useradd -m -u "${UID}" -g "${GID}" -s /bin/bash "${USERNAME}"; \
    else \
    # If some user already has UID, rename and set HOME
    name="$(getent passwd "${UID}" | cut -d: -f1)"; \
    mkdir -p "/home/${USERNAME}"; \
    usermod -d "/home/${USERNAME}" -l "${USERNAME}" "${name}"; \
    fi; \
    usermod -aG sudo "${USERNAME}" && echo "${USERNAME} ALL=(ALL) NOPASSWD:ALL" >/etc/sudoers.d/${USERNAME}; \
    mkdir -p /work /home/${USERNAME}/.local/bin

# ------------------------------------------------------------------------------
# ccache setup (symlink compilers), do not assume ownership yet (handled at entry)
# ------------------------------------------------------------------------------
ENV HOME=/home/${USERNAME} \
    CCACHE_DIR=/home/${USERNAME}/.cache/ccache \
    CCACHE_MAXSIZE=10G \
    CCACHE_COMPRESS=1
RUN ln -sf /usr/bin/ccache "$HOME/.local/bin/clang" \
    && ln -sf /usr/bin/ccache "$HOME/.local/bin/clang++" \
    && ln -sf /usr/bin/ccache "$HOME/.local/bin/gcc" \
    && ln -sf /usr/bin/ccache "$HOME/.local/bin/g++" \
    && echo 'export PATH="$HOME/.local/bin:$PATH"' >> "$HOME/.bashrc"

# ------------------------------------------------------------------------------
# Optional: pre-detect Conan profile (harmless if repeated later)
# ------------------------------------------------------------------------------
RUN conan profile detect --force || true

WORKDIR /work
EXPOSE 13000

# ------------------------------------------------------------------------------
# Entry point: fix volume ownership if needed, then drop to $USERNAME
# - USE_ROOT=1 to stay root for debugging if desired
# ------------------------------------------------------------------------------
ENV USE_ROOT=0
RUN cat >/usr/local/bin/entrypoint.sh <<'SH'
#!/usr/bin/env bash
set -euo pipefail

# If requested, stay as root (debug mode)
if [[ "${USE_ROOT:-0}" == "1" ]]; then
  exec /usr/bin/tini -g -- "$@"
fi

USER_NAME="${USERNAME:-dev}"
USER_HOME="/home/${USER_NAME}"

# Best-effort ownership fix for common mounts/caches
fix_own() {
  local p="$1"
  if [[ -e "$p" ]]; then
    # Only chown if not already owned by $USER_NAME
    if [[ "$(stat -c '%U:%G' "$p" 2>/dev/null || echo '')" != "${USER_NAME}:${USER_NAME}" ]]; then
      chown -R "${USER_NAME}:${USER_NAME}" "$p" || true
    fi
  fi
}

# Ensure home subdirs exist
mkdir -p "${USER_HOME}/.cache/ccache" "${USER_HOME}/.conan2" "${USER_HOME}/.local/bin" "/work"

# Fix ownership on typical paths (bind or named volumes)
fix_own "${USER_HOME}"
fix_own "${USER_HOME}/.cache"
fix_own "${USER_HOME}/.cache/ccache"
fix_own "${USER_HOME}/.conan2"
fix_own "/work"

# Minimal ccache init (idempotent)
sudo -u "${USER_NAME}" ccache --set-config=max_size="${CCACHE_MAXSIZE}" || true
sudo -u "${USER_NAME}" ccache --set-config=compression="${CCACHE_COMPRESS}" || true

# Drop privileges and run
exec /usr/bin/tini -g -- gosu "${USER_NAME}" "$@"
SH
RUN chmod +x /usr/local/bin/entrypoint.sh

ENTRYPOINT ["/usr/local/bin/entrypoint.sh"]
CMD ["bash"]
