# NovaOS network stack prototype

This directory holds the host-testable network prototype for NovaOS. The design follows a BSD-style socket layer on top of a netdev registry and a packet abstraction (`struct nova_sk_buff`). The API is intentionally small and conservative so it can later host Linux-compat shims and Windows-style protocol adapters without forcing those shims into the kernel.

## Core API

The canonical API is `kernel/network/network_stack.h`.

- `struct nova_socket` models a BSD socket: family, type, protocol, state, and the device that owns transmission.
- `struct nova_net_device` models a network interface. Each device exposes a `start_xmit` callback and a stats block.
- `struct nova_sk_buff` is the internal packet container. It carries `data`, `len`, device ownership, and the metadata needed for later protocol dispatch.

## Socket semantics

The prototype intentionally keeps socket semantics aligned with BSD naming and behavior:

- `socket(domain, type, protocol)` allocates an object that captures the address family and socket type. The default prototype route is the loopback device for `AF_INET` / `AF_INET6` traffic.
- `bind()` reserves local endpoint metadata without forcing a full protocol stack. It is a no-op in the prototype but preserves the BSD call pattern.
- `connect()` marks the socket as connected; the prototype keeps it side-effect free but valid for host tests and future protocol handlers.
- `listen()` and `accept()` follow the expected BSD lifecycle for stream sockets, but the prototype still treats them as a placeholder until a TCP accept queue exists.
- `send()` and `recv()` are the main user-visible data path. The prototype turns payloads into `nova_sk_buff`s and dispatches them through the selected device's `start_xmit` hook.
- `shutdown()`, `getsockopt()`, and `setsockopt()` are present as compatibility hooks; they validate the socket and return success for prototype-only options.

The important design rule is that the socket API is a compatibility boundary, not the transport implementation. Linux/Windows shims can translate from their socket APIs into the same `nova_socket`/`nova_sk_buff` objects without requiring the kernel to understand each ABI directly.

## Netdev bootstrap path

The current host-valid bootstrap flow is:

1. `nova_network_init()` initializes the global netdev registry.
2. Device constructors register themselves with `nova_netdev_register()`.
3. The prototype registers `lo` and `nic0` into the same registry.
4. Later kernel startup should call the same registration path from the kernel module init/boot sequence.

The closest repository references for that bootstrap seam are:

- `boot/kernel.c` is the serial boot entry for the prototype shell.
- `kernel/modules/nova_core/nova_core.c` shows the module-load pattern used by a FreeBSD-like kernel: `MOD_LOAD` executes during the module/bootstrap path.
- `kernel/io/io_stack.c` shows the same device-registry pattern used by in-tree device objects.

The network stack should therefore plug into the same lifecycle: module load / kernel init -> `nova_network_init()` -> `nova_loopback_init()` + `nova_nic_stub_init()` -> netdev registry populated -> sockets dispatch through selected devices.

## Future compatibility plan

- Linux compatibility shims can map `socket()`, `bind()`, `connect()`, `sendmsg()`, and the `sockaddr` family metadata onto `nova_socket` without rewriting the core stack.
- Windows compatibility shims can plug into the same boundary as a WinSock adaptor for `AF_INET` and `SOCK_STREAM` / `SOCK_DGRAM` sockets.
- The hybrid kernel IPC path can sit above the same device registry and packet queue by using `nova_sk_buff` as the kernel-to-user message representation instead of exposing device internals directly.

The prototype intentionally leaves transport logic incomplete so it remains easy to add a real IPv4/IPv6 stack and IPC bridges later without destabilizing the public socket abstraction.
