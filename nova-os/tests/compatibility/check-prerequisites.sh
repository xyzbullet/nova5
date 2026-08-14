#!/bin/sh
set -eu

root=$(CDPATH= cd -- "$(dirname -- "$0")/../.." && pwd)
blocked=0

require_command() {
	if command -v "$1" >/dev/null 2>&1; then
		printf 'available: %s\n' "$1"
	else
		printf 'blocked: missing command %s\n' "$1"
		blocked=1
	fi
}

if [ "$(uname -s)" != "FreeBSD" ]; then
	printf 'blocked: Phase 2 boot gates require a FreeBSD build/runtime host\n'
	blocked=1
else
	printf 'available: FreeBSD host\n'
fi

require_command qemu-system-x86_64
require_command makefs
require_command mkimg
require_command wine

if [ -z "${NOVA_IMAGE:-}" ]; then
	printf 'blocked: set NOVA_IMAGE to a prepared NovaOS image\n'
	blocked=1
elif [ ! -f "$NOVA_IMAGE" ]; then
	printf 'blocked: NOVA_IMAGE does not exist: %s\n' "$NOVA_IMAGE"
	blocked=1
else
	printf 'available: NovaOS image %s\n' "$NOVA_IMAGE"
fi

if [ -z "${NOVA_LINUX_HELLO:-}" ]; then
	printf 'blocked: set NOVA_LINUX_HELLO to a static Linux ELF fixture\n'
	blocked=1
elif [ ! -x "$NOVA_LINUX_HELLO" ]; then
	printf 'blocked: Linux hello fixture is not executable: %s\n' "$NOVA_LINUX_HELLO"
	blocked=1
else
	printf 'available: Linux hello fixture %s\n' "$NOVA_LINUX_HELLO"
fi

if [ -z "${NOVA_WINDOWS_HELLO:-}" ]; then
	printf 'blocked: set NOVA_WINDOWS_HELLO to a Win32 console fixture\n'
	blocked=1
elif [ ! -f "$NOVA_WINDOWS_HELLO" ]; then
	printf 'blocked: Windows hello fixture does not exist: %s\n' "$NOVA_WINDOWS_HELLO"
	blocked=1
else
	printf 'available: Windows hello fixture %s\n' "$NOVA_WINDOWS_HELLO"
fi

if [ "$blocked" -ne 0 ]; then
	printf '%s\n' 'Phase 2 gates are blocked; no compatibility result was recorded.'
	exit 2
fi

printf '%s\n' 'Phase 2 gate prerequisites are available.'
printf 'Run the image with: %s/tools/qemu/run.sh "$NOVA_IMAGE"\n' "$root"