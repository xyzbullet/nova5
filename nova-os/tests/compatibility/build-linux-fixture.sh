#!/bin/sh
set -eu

root=$(CDPATH= cd -- "$(dirname -- "$0")/../.." && pwd)
out_dir=${1:-"$root/build/compat"}
mkdir -p "$out_dir"

src=$root/tests/compatibility/fixtures/linux-hello.c
cc=${CC:-cc}
if command -v musl-gcc >/dev/null 2>&1; then
	linux_cc=${LINUX_CC:-musl-gcc}
	linux_flags=${LINUX_FLAGS:--static}
else
	linux_cc=${LINUX_CC:-$cc}
	linux_flags=${LINUX_FLAGS:--static}
fi

printf 'building Linux ELF fixture with %s\n' "$linux_cc"
# shellcheck disable=SC2086
"$linux_cc" $linux_flags "$src" -o "$out_dir/linux-hello"
if command -v file >/dev/null 2>&1; then
	file "$out_dir/linux-hello"
else
	printf 'built: %s\n' "$out_dir/linux-hello"
fi
