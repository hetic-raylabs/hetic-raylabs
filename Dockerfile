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
ENV USERNAME=${USERNAME} UID=${UID} GID=${GID}

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
  CCACHE_COMPRESS=true

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
# Copy entrypoint (from repo root)
# ------------------------------------------------------------------------------
COPY entrypoint.sh /usr/local/bin/entrypoint.sh
RUN chmod +x /usr/local/bin/entrypoint.sh

# ------------------------------------------------------------------------------
# Use tini as PID 1 + our entrypoint
# ------------------------------------------------------------------------------
ENTRYPOINT ["/usr/local/bin/entrypoint.sh"]
CMD ["bash"]
