#!/usr/bin/env bash
set -euo pipefail

repo_root="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
cd "$repo_root"

serial_gid="$(getent group dialout | cut -d: -f3)"
if [[ -z "$serial_gid" ]]; then
    printf 'The dialout group was not found.\n' >&2
    exit 1
fi

cat > .env <<EOF_ENV
DEV_UID=$(id -u)
DEV_GID=$(id -g)
DEV_USERNAME=$(id -un)
DEV_SERIAL_GID=$serial_gid
EOF_ENV

printf 'Wrote .env for %s (UID:GID %s:%s).\n' \
    "$(id -un)" "$(id -u)" "$(id -g)"
printf 'Serial devices use the dialout group ID %s.\n' "$serial_gid"
printf 'Now run: docker compose up --build -d\n'
