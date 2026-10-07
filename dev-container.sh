#!/usr/bin/env bash
set -euo pipefail

repo_root="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
cd "$repo_root"

usage() {
    printf 'Usage: %s [device]\n' "$(basename -- "$0")" >&2
    printf '  (no argument) Start/recreate python-dev without a serial device.\n' >&2
    printf '  device       Start/recreate python-dev with the serial device passed through.\n' >&2
}

if [[ $# -gt 1 ]]; then
    usage
    exit 2
fi

compose_files=(-f docker-compose.yml)
if [[ $# -eq 1 ]]; then
    case "$1" in
        device)
            serial_device="${DEV_SERIAL_DEVICE:-}"
            if [[ -z "$serial_device" && -f .env ]]; then
                serial_device="$(awk -F= '$1 == "DEV_SERIAL_DEVICE" { value = substr($0, index($0, "=") + 1) } END { print value }' .env)"
            fi
            serial_device="${serial_device:-/dev/ttyUSB0}"

            if [[ ! -c "$serial_device" ]]; then
                printf 'Serial device not found: %s\n' "$serial_device" >&2
                printf 'In Windows PowerShell, find the device BUSID and attach it to WSL, then retry:\n' >&2
                printf '  usbipd list\n' >&2
                printf '  usbipd attach --wsl --busid <BUSID>\n' >&2
                printf 'Replace <BUSID> with the value shown for your USB serial device.\n' >&2
                exit 1
            fi

            compose_files+=(-f docker-compose.serial.yml)
            printf 'Found serial device: %s\n' "$serial_device"
            ;;
        *)
            usage
            exit 2
            ;;
    esac
else
    printf 'Starting python-dev without a serial device.\n'
fi

docker compose "${compose_files[@]}" up -d --force-recreate python-dev
