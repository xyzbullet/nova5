# NovaKit

NovaKit is NovaOS's driver registry and lifecycle boundary. New native drivers
will target Rust, while the C ABI keeps inherited FreeBSD drivers and isolated
LinuxKPI modules interoperable.

The current implementation provides a host-testable registry for device class,
PCI identity, attachment state, and userland discovery. Kernel dynamic loading,
hot-plug notifications, and actual bus probing are intentionally still gated on
the FreeBSD source integration.