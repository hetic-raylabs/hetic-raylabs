# ------------------------------------------------------------------------------
# Base image
# ------------------------------------------------------------------------------
FROM ubuntu:24.04

# ------------------------------------------------------------------------------
# Global environment (non-interactive apt, timezone, default toolchain/generator)
# ------------------------------------------------------------------------------
ENV DEBIAN_FRONTEND=noninteractive \
    TZ=Etc/UTC \
    CC=clang \
    CXX=clang++ \
    CMAKE_GENERATOR=Ninja

# ------------------------------------------------------------------------------
# System toolchain & utilities
# - Clang/LLVM, CMake, Ninja, debugging tools
# - ccache for faster incremental builds
# - Python + venv for Conan (PEP 668 compliant)
# ------------------------------------------------------------------------------
RUN apt-get update && apt-get upgrade -y && \
    apt-get install -y --no-install-recommends \
    build-essential cmake ninja-build pkg-config \
    clang llvm lldb gdb gdbserver \
    clang-tidy clang-format \
    ccache git curl ca-certificates \
    python3 python3-venv \
    unzip zip sudo && \
    rm -rf /var/lib/apt/lists/*

# ------------------------------------------------------------------------------
# Conan 2.x in an isolated virtual environment (no system-site-packages)
# ------------------------------------------------------------------------------
RUN python3 -m venv /opt/conan && \
    /opt/conan/bin/pip install --no-cache-dir "conan>=2.4,<3" && \
    ln -s /opt/conan/bin/conan /usr/local/bin/conan

# ------------------------------------------------------------------------------
# Runtime identity (numeric UID/GID to avoid collisions with existing users)
# - We do NOT create system users/groups; we just prepare writable locations
# ------------------------------------------------------------------------------
ARG USERNAME=dev
ARG UID=1000
ARG GID=1000

RUN set -eux; \
    mkdir -p "/home/${USERNAME}/.cache/ccache" "/work" && \
    chown -R "${UID}:${GID}" "/home/${USERNAME}" "/work"

# Home & ccache environment for the numeric identity
ENV HOME=/home/${USERNAME} \
    CCACHE_DIR=/home/${USERNAME}/.cache/ccache \
    CCACHE_MAXSIZE=10G \
    CCACHE_COMPRESS=1

# Run as the numeric uid:gid
USER ${UID}:${GID}
WORKDIR /work

# ------------------------------------------------------------------------------
# ccache setup (use ccache transparently for common compilers)
# ------------------------------------------------------------------------------
RUN mkdir -p "${CCACHE_DIR}" && \
    ccache --set-config=max_size=${CCACHE_MAXSIZE} && \
    ccache --set-config=compression=${CCACHE_COMPRESS} && \
    mkdir -p "${HOME}/.local/bin" && \
    ln -s /usr/bin/ccache "${HOME}/.local/bin/clang"  || true && \
    ln -s /usr/bin/ccache "${HOME}/.local/bin/clang++"|| true && \
    ln -s /usr/bin/ccache "${HOME}/.local/bin/gcc"    || true && \
    ln -s /usr/bin/ccache "${HOME}/.local/bin/g++"    || true && \
    echo 'export PATH="$HOME/.local/bin:$PATH"' >> "${HOME}/.bashrc"

# ------------------------------------------------------------------------------
# Conan profile detection (optional, harmless if it needs to be re-run later)
# ------------------------------------------------------------------------------
RUN conan profile detect --force || true

# ------------------------------------------------------------------------------
# Debug port (gdbserver) and default command
# ------------------------------------------------------------------------------
EXPOSE 13000
CMD ["bash"]
