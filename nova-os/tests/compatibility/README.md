# Phase 2 compatibility gates

These gates are intentionally explicit. A missing runtime produces `not-run`,
not a passing result.

## Current status

| Gate | Status | Required runtime |
| --- | --- | --- |
| Static Linux ELF hello | Not run | FreeBSD Linuxulator |
| Dynamic Linux CLI | Not run | Linuxulator userland |
| GUI toolkit smoke test | Not run | Linuxulator + display stack |
| Win32 console hello | Not run | Wine |
| Proton launch | Not run | Wine/Proton + Vulkan |

Run these only from a prepared NovaOS/FreeBSD image. Do not substitute a host
Linux execution for a NovaOS compatibility result.

## Preflight

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