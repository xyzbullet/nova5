#!/bin/sh
set -eu

root=$(CDPATH= cd -- "$(dirname -- "$0")/../.." && pwd)
iso=${1:-"$root/build/novaos-prototype.iso"}
transcript=${NOVA_QEMU_TRANSCRIPT:-"$root/build/qemu-shell-transcript.txt"}
commands=${NOVA_QEMU_COMMANDS:-'help
info
uptime
'}

if [ ! -f "$iso" ]; then
	printf 'blocked: ISO does not exist: %s\n' "$iso" >&2
	exit 2
fi
if ! command -v qemu-system-x86_64 >/dev/null 2>&1; then
	printf '%s\n' 'blocked: qemu-system-x86_64 is required for the NovaOS shell proof' >&2
	exit 2
fi
if ! command -v timeout >/dev/null 2>&1; then
	printf '%s\n' 'blocked: timeout is required for scripted QEMU validation' >&2
	exit 2
fi

mkdir -p "$(dirname -- "$transcript")"
(
	sleep 4
	printf '%s' "$commands"
	sleep 2
) | timeout 20 qemu-system-x86_64 \
	-cdrom "$iso" \
	-boot d \
	-m 128M \
	-serial stdio \
	-display none \
	-monitor none \
	-no-reboot \
	-no-shutdown >"$transcript" 2>&1 || status=$?
status=${status:-0}

cat "$transcript"
case "$status" in
	0|124) ;;
	*) printf 'blocked: QEMU exited with status %s\n' "$status" >&2; exit "$status" ;;
esac

for expected in \
	'NovaOS Phase 2 prototype booted.' \
	'nova> help' \
	'commands: help, info, clear, uptime' \
	'NovaOS Phase 2 prototype' \
	'uptime: running under QEMU'
do
	if ! grep -F "$expected" "$transcript" >/dev/null 2>&1; then
		printf 'failed: transcript missing expected text: %s\n' "$expected" >&2
		exit 1
	fi
done

printf 'NovaOS QEMU serial shell proof passed: %s\n' "$transcript"
