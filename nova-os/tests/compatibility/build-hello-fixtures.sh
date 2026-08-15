#!/bin/sh
set -eu

root=$(CDPATH= cd -- "$(dirname -- "$0")/../.." && pwd)
out_dir=${1:-"$root/build/compat"}
mkdir -p "$out_dir"

linux_src=$root/tests/compatibility/fixtures/linux-hello.c
windows_src=$root/tests/compatibility/fixtures/windows-hello.c

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
"$linux_cc" $linux_flags "$linux_src" -o "$out_dir/linux-hello"
if command -v file >/dev/null 2>&1; then
	file "$out_dir/linux-hello"
else
	printf 'built: %s\n' "$out_dir/linux-hello"
fi

if command -v x86_64-w64-mingw32-gcc >/dev/null 2>&1; then
	windows_cc=${WINDOWS_CC:-x86_64-w64-mingw32-gcc}
elif command -v winegcc >/dev/null 2>&1; then
	windows_cc=${WINDOWS_CC:-winegcc}
else
	printf '%s\n' 'blocked: x86_64-w64-mingw32-gcc or winegcc is required for the Windows PE fixture' >&2
	exit 2
fi

printf 'building Windows PE fixture with %s\n' "$windows_cc"
"$windows_cc" "$windows_src" -o "$out_dir/windows-hello.exe"
if command -v file >/dev/null 2>&1; then
	file "$out_dir/windows-hello.exe"
else
	printf 'built: %s\n' "$out_dir/windows-hello.exe"
fi
