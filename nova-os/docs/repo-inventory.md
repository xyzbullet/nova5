Repository inventory for NovaOS (initial scan)

Location: /workspaces/nova5/nova-os

Top-level directories discovered:
- boot/            — bootloader/entry assembly, kernel.c, linker
- compat/          — compatibility layers (linux, windows, linuxulator, wine-proton)
- drivers/         — linuxkpi, native, novakit-native drivers
- kernel/          — kernel sources (bsd, core, io, modules, network, novakit)
- userland/        — bin, services, uimode (desktop)
- tools/           — build and helper tools
- sources/         — third-party sources
- docs/            — documentation (this file will live here)

Components the project requested to prioritize:
- Rapid kernel (present under kernel/ and novakit/ sources) — need deeper inventory of Rapid-specific subsystems
- net/             — kernel/network/ contains network_stack.h and basic interfaces
- userland/init/   — userland init likely under userland/services or separate; further scan needed
- uimode/          — userland/uimode contains desktop files
- lang/            — not present as a top-level 'lang' directory; likely planned (NovaLang) — create a new lang/ plan
- toolchain/       — not present as 'toolchain' top-level — toolchain work will need new layout or reuse tools/
- drivers/         — exists; contains native and linuxkpi drivers

Next steps / gaps to inventory in detail:
1. Rapid kernel specifics: locate Rapid kernel sources and document how they map to FreeBSD base layout. Check kernel/novakit, kernel/core, and kernel/bsd for Rapid-specific files.
2. Networking code: expand inventory under kernel/network — currently only network_stack.h present. Identify any implementations, protocol handlers, and drivers that integrate with netdev ops.
3. Userland init: locate or add an init/ service under userland/services; define expectations for init process and syscalls.
4. Language/runtime: NovaLang (Kotlin Native) - create lang/ plan for runtime, cross-compilation, and integration with toolchain.
5. Toolchain: create a toolchain/ directory or adapt tools/ for cross-compilation to x86_64 and Kotlin Native integration.
6. Drivers: create a mapping of required NIC drivers, disk, and other hardware; leverage drivers/linuxkpi for Linux driver compatibility where possible.

This inventory is a first pass; a more detailed file-level inventory and mapping to FreeBSD subsystems will follow in the repo-analysis workstream.
