#!/bin/sh
set -eu

root=$(CDPATH= cd -- "$(dirname -- "$0")/../.." && pwd)
fixture_dir=${1:-"$root/build/compat"}
linux_hello=$fixture_dir/linux-hello
windows_hello=$fixture_dir/windows-hello.exe

if [ ! -x "$linux_hello" ] || [ ! -f "$windows_hello" ]; then
	printf 'blocked: build fixtures first with tests/compatibility/build-hello-fixtures.sh\n' >&2
	exit 2
fi

printf '%s\n' 'host smoke: Linux ELF fixture (not a NovaOS compatibility pass)'
"$linux_hello"

if command -v wine64 >/dev/null 2>&1; then
	wine_cmd=wine64
elif command -v wine >/dev/null 2>&1; then
	wine_cmd=wine
else
	printf '%s\n' 'blocked: wine64 or wine is required to smoke-test the Windows PE fixture' >&2
	exit 2
fi

printf 'host smoke: Windows PE fixture via %s (not a NovaOS compatibility pass)\n' "$wine_cmd"
"$wine_cmd" "$windows_hello"
