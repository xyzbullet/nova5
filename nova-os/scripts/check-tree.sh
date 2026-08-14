#!/bin/sh
set -eu

root=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)

required_dirs="
  compat/linux
  compat/windows
  config
  docs
  kernel
  scripts
  tools/qemu
  userland/bin
  userland/services
"

for relative_dir in $required_dirs; do
  if [ ! -d "$root/$relative_dir" ]; then
    printf 'missing required directory: %s\n' "$relative_dir" >&2
    exit 1
  fi
done

required_files="
  README.md
  LICENSE
  Makefile
  config/nova.toml.example
  docs/architecture.md
  docs/compatibility.md
  docs/roadmap.md
  kernel/nova.conf
"

for relative_file in $required_files; do
  if [ ! -f "$root/$relative_file" ]; then
    printf 'missing required file: %s\n' "$relative_file" >&2
    exit 1
  fi
done

if ! command -v make >/dev/null 2>&1; then
  printf '%s\n' 'make is required for NovaOS development checks' >&2
  exit 1
fi

printf '%s\n' 'NovaOS skeleton check passed.'