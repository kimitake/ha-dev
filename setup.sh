#!/usr/bin/env bash
set -euo pipefail

repo_root="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
cd "$repo_root"

cat > .env <<EOF_ENV
DEV_UID=$(id -u)
DEV_GID=$(id -g)
DEV_USERNAME=$(id -un)
EOF_ENV

printf 'Wrote .env for %s (UID:GID %s:%s).\n' \
    "$(id -un)" "$(id -u)" "$(id -g)"
printf 'Now run: docker compose up --build -d\n'
