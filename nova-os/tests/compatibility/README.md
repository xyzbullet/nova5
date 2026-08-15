# Phase 2 compatibility gates

These gates are intentionally explicit. A missing runtime produces `not-run`,
not a passing result.

## Current status

| Gate | Status | Required runtime |
| --- | --- | --- |
| Static Linux ELF hello | Fixture builds on this host; NovaOS run not-run | FreeBSD Linuxulator |
| Dynamic Linux CLI | Not run | Linuxulator userland |
| GUI toolkit smoke test | Not run | Linuxulator + display stack |
| Win32 console hello | Fixture build blocked in this host; NovaOS run not-run | Wine |
| Proton launch | Not run | Wine/Proton + Vulkan |
| QEMU prototype serial shell | Scripted proof available when QEMU/GRUB tools are installed | qemu-system-x86_64 + grub-mkrescue |

Run compatibility gates only from a prepared NovaOS/FreeBSD image. Do not
substitute a host Linux execution for a NovaOS compatibility result.

## Fixture build and host smoke tests

The repository includes tiny source fixtures for the first Linux and Windows
hello-world gates:

```sh
cd nova-os
make compat-linux-fixture
make compat-windows-fixture
make compat-fixtures
make compat-host-smoke
```

The split targets let developers prove the Linux fixture even when the Windows
toolchain is unavailable. All fixture targets write generated binaries under
`build/compat/`. The Linux target uses `musl-gcc` when available, otherwise `cc
-static`. The Windows target requires `x86_64-w64-mingw32-gcc` or `winegcc`; if
neither is installed it exits `2` and records a blocked result instead of
fabricating a pass.

`make compat-host-smoke` executes the Linux fixture on the host and the Windows
fixture through host Wine when available. Those host smoke checks prove fixture
sanity only; they are not NovaOS compatibility passes.

## QEMU prototype shell proof

The small Multiboot prototype can be scripted with:

```sh
cd nova-os
make prototype-shell-test
```

That target builds the prototype ISO, boots it under QEMU with serial stdio,
sends `help`, `info`, and `uptime`, then verifies the transcript contains the
interactive `nova>` shell responses. Missing QEMU/GRUB tooling exits `2` as a
blocked result.

## Prepared NovaOS/FreeBSD preflight

From a FreeBSD build/runtime host, set the image and fixture paths, then run:

```sh
export NOVA_IMAGE=/path/to/novaos.img
export NOVA_LINUX_HELLO=/path/to/static-linux-hello
export NOVA_WINDOWS_HELLO=/path/to/windows-hello.exe
sh nova-os/tests/compatibility/check-prerequisites.sh
```

The preflight exits with status `2` when a required runtime is missing. It
never marks a gate as passed by running a binary directly on the host.

## Validation attempt in this development environment

The FreeBSD/Linuxulator gates remain dependent on a FreeBSD-derived NovaOS
image. The QEMU prototype shell is a separate boot milestone and does not count
as the BSD-personality shell gate.
