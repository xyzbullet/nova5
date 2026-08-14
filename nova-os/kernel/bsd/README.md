# BSD personality layer

The BSD personality is the FreeBSD-derived layer above `kernel/core/`. It owns
POSIX process semantics, signals, VFS, networking, MAC, Capsicum, and jails.

Phase 2 starts with this integration contract rather than copying FreeBSD
source into the repository. Once `upstream/freebsd-src` is attached, adapters
will map the FreeBSD scheduler, VM, and IPC entry points to the core interfaces.

No BSD subsystem is considered migrated until it boots in QEMU and reaches a
shell through the Phase 2 validation gate.