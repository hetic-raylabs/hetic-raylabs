#!/usr/bin/env bash
set -euo pipefail

# Option pour rester root
if [[ "${USE_ROOT:-0}" == "1" ]]; then
  exec /usr/bin/tini -g -- "$@"
fi

USER_NAME="${USERNAME:-dev}"
# Récupère UID/GID num. associés à l'utilisateur (existent à build-time)
TARGET_UID="$(id -u "${USER_NAME}")"
TARGET_GID="$(id -g "${USER_NAME}")"
USER_HOME="$(getent passwd "${USER_NAME}" | cut -d: -f6)"

# Crée tous les répertoires nécessaires
mkdir -p "${USER_HOME}/.cache/ccache" \
         "${USER_HOME}/.config/ccache" \
         "${USER_HOME}/.conan2" \
         "${USER_HOME}/.local/bin" \
         "/work"

# chown **numérique** (robuste si le groupe ne s'appelle pas 'dev')
fix_own() {
  local p="$1"; [[ -e "$p" ]] || return 0
  chown -R "${TARGET_UID}:${TARGET_GID}" "$p" || true
}
# chown /work but EXCLUDE .git (do not touch git metadata)
fix_work_excluding_git() {
  local root="/work"
  [[ -d "$root" ]] || return 0
  # chown each first-level entry except .git, and recurse
  find "$root" -mindepth 1 -maxdepth 1 \
    -not -path "$root/.git" \
    -exec chown -R "${TARGET_UID}:${TARGET_GID}" {} + || true
}
fix_own "${USER_HOME}"
fix_own "${USER_HOME}/.cache"
fix_own "${USER_HOME}/.cache/ccache"
fix_own "${USER_HOME}/.config"
fix_own "${USER_HOME}/.config/ccache"
fix_own "${USER_HOME}/.conan2"
# fix_own "/work"
fix_work_excluding_git

# ccache init (booléens attendus)
sudo -u "${USER_NAME}" ccache --set-config=max_size="${CCACHE_MAXSIZE}" || true
sudo -u "${USER_NAME}" ccache --set-config=compression="${CCACHE_COMPRESS}" || true

exec /usr/bin/tini -g -- gosu "${USER_NAME}" "$@"
