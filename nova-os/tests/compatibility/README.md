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