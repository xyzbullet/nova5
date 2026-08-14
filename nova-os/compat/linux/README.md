# Linux compatibility layer

This directory owns NovaOS packaging and integration around the FreeBSD
Linuxulator. It does not contain a second Linux kernel.

Planned components:

- Versioned Linux userland runtime manifests.
- Vulkan/OpenGL and PipeWire bridge configuration.
- A sandboxed application launcher.
- Installation and smoke-test definitions.
- Runtime diagnostics for missing libraries, graphics, and audio.

The first implementation should use an existing FreeBSD Linuxulator rather than
forking an ABI implementation prematurely.