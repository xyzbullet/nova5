# NovaOS Phase 2 architecture

This document records the Phase 2 implementation boundary. It extends the
FreeBSD-derived system described in `docs/architecture.md`; it does not claim
that the complete microkernel or compatibility runtimes are production-ready.

## Three kernel layers

```text
┌──────────────────────────────────────────────────────────────┐
│ BSD personality: POSIX, PID/VFS/networking, MAC, Capsicum    │
├──────────────────────────────────────────────────────────────┤
│ NovaKit: device registry, driver lifecycle, driver classes   │
├──────────────────────────────────────────────────────────────┤
│ Microkernel core: scheduler, threads, IPC, virtual memory    │
└──────────────────────────────────────────────────────────────┘
```

### Microkernel core

`kernel/core/` owns only primitives that must be trusted and small:

- SMP-aware scheduling interfaces with priority and fair-share metadata.
- Thread lifecycle and context-switching contracts.
- Message-based IPC endpoints with bounded queues.
- Physical page allocation and address-space mapping primitives.

The current implementation is a portable host-testable model of these
interfaces. It is not yet a ring-0 boot image. Context switching and hardware
page-table operations remain FreeBSD-architecture work after the upstream
source tree is attached.

### BSD personality

`kernel/bsd/` is the integration boundary for the FreeBSD-derived personality:

- Process IDs, signals, POSIX APIs, VFS, UFS/ZFS, sockets, MAC, Capsicum, and
  jails remain FreeBSD responsibilities.
- The BSD layer consumes core primitives through a narrow adapter instead of
  reaching into NovaKit internals.
- FreeBSD code remains upstream-tracked and retains its original license
  notices.

### NovaKit driver framework

`kernel/novakit/` owns the device registry and lifecycle contracts. Drivers are
classified as storage, network, GPU, input, audio, or bus controllers.

**Language choice:** new NovaKit-native drivers use Rust. The framework exposes
a C-compatible boundary for inherited FreeBSD drivers and isolated LinuxKPI
modules. Rust drivers must not directly depend on GPL code or Windows-derived
artifacts.

The current registry is host-testable. Dynamic kernel loading and hot-plug
event wiring are gated on the FreeBSD integration phase.

## Driver tiers

1. `drivers/native/` — inherited FreeBSD drivers, retained under their upstream
   licenses.
2. `drivers/linuxkpi/` — isolated LinuxKPI and drm-kmod integration. GPL
   components remain separate loadable modules and are not linked into the BSD
   core.
3. `drivers/novakit-native/` — new Rust drivers written against NovaKit.

No device is marked supported until it attaches and passes the compatibility
matrix.

## Application compatibility

- `compat/linuxulator/` tracks Linux ELF loading, syscall coverage, and runtime
  manifests.
- `compat/wine-proton/` tracks Wine/Proton, DXVK, VKD3D-Proton, and FAudio
  integration.
- No Windows source, headers, binaries, or ReactOS source may enter the tree.

## Phase 2 validation boundary

The repository currently validates the core and registry models on the host.
Linux and Windows runtime tests are represented as explicit not-run gates until
the required FreeBSD/Linuxulator and Wine/Proton runtimes are attached. A
future phase must demonstrate:

1. QEMU boots to a BSD-layer shell.
2. A virtio driver attaches through NovaKit and appears in the registry.
3. A statically linked Linux ELF binary executes.
4. A trivial Win32 console executable executes through Wine.