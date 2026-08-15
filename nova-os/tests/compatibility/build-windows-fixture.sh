#!/bin/sh
set -eu

root=$(CDPATH= cd -- "$(dirname -- "$0")/../.." && pwd)
out_dir=${1:-"$root/build/compat"}
mkdir -p "$out_dir"

src=$root/tests/compatibility/fixtures/windows-hello.c
if command -v x86_64-w64-mingw32-gcc >/dev/null 2>&1; then
	windows_cc=${WINDOWS_CC:-x86_64-w64-mingw32-gcc}
elif command -v winegcc >/dev/null 2>&1; then
	windows_cc=${WINDOWS_CC:-winegcc}
else
	printf '%s\n' 'blocked: x86_64-w64-mingw32-gcc or winegcc is required for the Windows PE fixture' >&2
	exit 2
fi

printf 'building Windows PE fixture with %s\n' "$windows_cc"
"$windows_cc" "$src" -o "$out_dir/windows-hello.exe"
if command -v file >/dev/null 2>&1; then
	file "$out_dir/windows-hello.exe"
else
	printf 'built: %s\n' "$out_dir/windows-hello.exe"
fi
