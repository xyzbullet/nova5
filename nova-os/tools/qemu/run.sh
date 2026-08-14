#!/bin/sh
set -eu

if [ "$#" -lt 1 ]; then
  printf 'usage: %s DISK_OR_ISO [qemu options...]\n' "$0" >&2
  exit 2
fi

image=$1
shift

if ! command -v qemu-system-x86_64 >/dev/null 2>&1; then
  printf '%s\n' 'qemu-system-x86_64 is required to boot a NovaOS developer image.' >&2
  exit 1
fi

exec qemu-system-x86_64 \
  -machine q35 \
  -m 4096 \
  -smp 4 \
  -enable-kvm \
  -drive "file=$image,if=virtio,format=raw" \
  "$@"