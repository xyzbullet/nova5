# NovaOS

NovaOS is a FreeBSD-derived desktop operating system for gaming, creative work,
and everyday computing. It is designed around a stable FreeBSD kernel base with
first-class Linux application support and a Windows compatibility path through
Wine/Proton.

> **Status:** repository skeleton and architecture baseline. This project does
> not yet contain a complete kernel fork, installer, desktop environment, or
> production compatibility runtime.

## Product goals

- Reliable gaming on modern AMD, Intel, and NVIDIA hardware.
- Low-latency audio and graphics workflows for editing and production.
- Linux applications through the FreeBSD Linuxulator and packaged runtime
  environments.
- Windows games through Proton and Windows applications through Wine.
- A coherent desktop experience with atomic updates, recovery snapshots, and
  reproducible system images.
- A maintainable upstream relationship with FreeBSD instead of an unmergeable
  long-lived fork.

## Architecture at a glance

NovaOS uses a **hybrid integration model**:

1. The kernel starts from the FreeBSD source tree.
2. Hardware support, scheduling, networking, storage, and security remain
   kernel responsibilities.
3. Linux and Windows compatibility are isolated in userland/runtime layers.
4. Desktop services communicate over stable IPC boundaries rather than adding
   application-specific behavior to the kernel.

This makes "hybrid" a practical architectural boundary: the system can combine
FreeBSD kernel facilities with Linux and Windows application ecosystems without
embedding foreign application runtimes in kernel mode.

See [`docs/architecture.md`](docs/architecture.md) for the full design and
[`docs/roadmap.md`](docs/roadmap.md) for staged implementation work.

## Repository layout

```text
nova-os/
├── build/                 # Generated output; kept out of source control
├── compat/
│   ├── linuxulator/       # Linux ELF/syscall compatibility milestones
│   ├── wine-proton/       # Wine/Proton integration milestones
│   ├── linux/             # Existing Linux integration notes
│   └── windows/           # Existing Windows integration notes
├── config/
│   └── nova.toml.example  # Image and workstation configuration
├── docs/                  # Architecture, compatibility, and roadmap docs
├── kernel/
│   ├── core/              # Host-testable microkernel primitives
│   ├── bsd/               # FreeBSD personality integration boundary
│   ├── novakit/            # Device registry and driver lifecycle
│   ├── nova.conf          # FreeBSD kernel configuration overlay
│   └── modules/           # Small NovaOS-specific kernel modules
├── drivers/
│   ├── native/            # Inherited FreeBSD driver boundary
│   ├── linuxkpi/          # Isolated LinuxKPI boundary
│   └── novakit-native/    # Rust NovaKit driver scaffolds
├── scripts/               # Host checks and build entry points
├── tools/
│   └── qemu/              # Local VM boot helpers
└── userland/
    ├── services/          # Long-running NovaOS desktop services
    ├── uimode/            # UI mode boot planning and desktop startup contract
    └── bin/               # User-facing command wrappers
```

## Getting started

The initial repository is intentionally host-friendly: structural checks and
configuration generation work on Linux, macOS, and FreeBSD. Building a bootable
FreeBSD image requires a FreeBSD build host or a prepared cross-build
environment.

```sh
cd nova-os
make help
make check
make configure
```

To attach an upstream FreeBSD checkout later:

```sh
git clone --filter=blob:none https://git.FreeBSD.org/src.git upstream/freebsd-src
```

The upstream checkout is intentionally not copied into this repository. Keep
NovaOS changes as small overlays or topic branches so they can be rebased as
FreeBSD evolves.

## Development principles

- Do not add compatibility behavior to the kernel unless it is required for
  hardware, scheduling, security, or a kernel ABI.
- Prefer upstreamable FreeBSD changes over NovaOS-only patches.
- Keep Linux and Windows runtime versions pinned and reproducible.
- Every hardware or compatibility claim must have a repeatable test.
- Never call a placeholder runtime "supported" until it passes the relevant
  smoke-test suite.

## License

The FreeBSD-derived portions must preserve their original BSD license notices.
New NovaOS code should use the 2-clause BSD license unless a component's
upstream license requires another compatible license. See
[`LICENSE`](LICENSE) and retain notices when importing code.
## Phase 3.1 prototype shell

The Phase 3.1 Rapid Kernel target boots a freestanding Multiboot kernel in QEMU
and exposes a serial command line.  It is not a full POSIX-certified system yet,
but shell commands now call through a small kernel syscall dispatcher (`write`,
`uname`, `getpid`, and `uptime`) so the prototype has an explicit ABI boundary
for the upcoming BSD personality work.

```sh
cd nova-os
make prototype-shell-test
```

Upstream source checkouts are intentionally kept out of the repository.  To pull
the actual sources for the BSD/POSIX personality, driver inheritance, and clean
compatibility runtimes, run:

```sh
cd nova-os
make phase3-fetch
```

This clones the repositories listed in `sources/phase3.1.toml` into
`build/upstream/` with shallow checkouts.
