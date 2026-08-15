# Phase 2 compatibility gates

These gates are intentionally explicit. A missing runtime produces `not-run`,
not a passing result.

## Current status

| Gate | Status | Required runtime |
| --- | --- | --- |
| Static Linux ELF hello | Fixture builds and runs on this dev host (musl-gcc, static); NovaOS run still not-run | FreeBSD Linuxulator |
| Dynamic Linux CLI | Not run | Linuxulator userland |
| GUI toolkit smoke test | Not run | Linuxulator + display stack |
| Win32 console hello | Fixture builds (x86_64-w64-mingw32-gcc) and runs on this dev host via host Wine 9.0; NovaOS run still not-run | Wine |
| Proton launch | Not run | Wine/Proton + Vulkan |
| QEMU prototype serial shell | Verified in this dev environment: `make prototype-shell-test` boots the Multiboot ISO under QEMU and confirms the `nova>` shell responds to `help`/`info`/`uptime` over serial | qemu-system-x86_64 + grub-mkrescue |

Run compatibility gates only from a prepared NovaOS/FreeBSD image. Do not
substitute a host Linux execution for a NovaOS compatibility result.

## Fixture build and host smoke tests

The repository includes tiny source fixtures for the first Linux and Windows
hello-world gates:

```sh
cd nova-os
make compat-fixtures
make compat-host-smoke
```

`make compat-fixtures` writes generated binaries under `build/compat/`. It uses
`musl-gcc` when available for the static Linux ELF fixture, otherwise `cc
-static`. For the Windows PE fixture it requires `x86_64-w64-mingw32-gcc` or
`winegcc`; if neither is installed the target exits `2` and records a blocked
result instead of fabricating a pass.

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

**2026-08-14, Ubuntu 24.04 sandbox:** `make check`, `make phase2-test`,
`make prototype-image`, and `make prototype-shell-test` all pass, with a saved
transcript at `build/qemu-shell-transcript.txt` showing the booted kernel
answering `help`, `info`, and `uptime` at the `nova>` prompt. `make
compat-fixtures` and `make compat-host-smoke` also pass (musl-gcc for the
Linux fixture, x86_64-w64-mingw32-gcc + host Wine 9.0 for the Windows
fixture). None of this substitutes for the FreeBSD/Linuxulator or Wine-inside-
NovaOS gates above — it confirms the toolchain, the Multiboot boot path, and
the fixture build steps are sound on a plain Linux host.
