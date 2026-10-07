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
DEV_SERIAL_DEVICE=/dev/ttyUSB0
EOF_ENV

printf 'Wrote .env for %s (UID:GID %s:%s).\n' \
    "$(id -un)" "$(id -u)" "$(id -g)"
printf 'Serial devices use the dialout group ID %s.\n' "$serial_gid"
printf 'Serial device path: /dev/ttyUSB0 (override DEV_SERIAL_DEVICE in .env if needed).\n'
printf 'Start/recreate without a serial device: bash ./dev-container.sh\n'
printf 'After attaching the serial device to WSL, use: bash ./dev-container.sh device\n'
