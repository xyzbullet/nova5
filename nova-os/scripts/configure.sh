#!/bin/sh
set -eu

if [ "$#" -ne 2 ]; then
  printf 'usage: %s BUILD_DIR CONFIG_FILE\n' "$0" >&2
  exit 2
fi

build_dir=$1
config_file=$2

mkdir -p "$build_dir"

if [ ! -f "$config_file" ]; then
  printf 'configuration file not found: %s\n' "$config_file" >&2
  exit 1
fi

cat >"$build_dir/manifest.txt" <<EOF
project=nova-os
config=$config_file
kernel_config=kernel/nova.conf
linux_runtime=compat/linux
windows_runtime=compat/windows
EOF

printf 'Wrote %s\n' "$build_dir/manifest.txt"