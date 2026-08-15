# NovaOS architecture

## Scope

NovaOS is a FreeBSD-derived desktop distribution, not a promise to merge an
entire Linux kernel and Windows kernel into one kernel image. The first
implementation should be shippable and maintainable:

```text
Applications
├── Native BSD / POSIX applications
├── Linux applications ──► Linuxulator + Linux userland runtime
└── Windows applications
    ├── Games ──► Wine + Proton + DXVK/VKD3D
    └── Desktop software ──► Wine runtime and per-app prefixes

Desktop services ──► stable IPC and session APIs
FreeBSD-derived kernel ──► drivers, scheduler, VM, storage, networking, security
Firmware / hardware
```

The system is "hybrid" at the integration boundary. Kernel mode stays small
and hardware-focused while userland can host multiple application ecosystems.
This is safer than placing Wine, Proton, or Linux ABI emulation in the kernel.

## Kernel base

`kernel/nova.conf` is an overlay configuration that starts from FreeBSD's
`GENERIC` configuration. NovaOS-specific kernel changes should be organized as:

1. An upstream FreeBSD contribution when generally useful.
2. A small kernel module when the behavior is optional and isolated.
3. A distribution/userland service when kernel mode is unnecessary.

The first NovaOS kernel work should focus on hardware enablement, real-time
workload tuning, GPU memory behavior, suspend/resume, and observability.

## Compatibility layers

### Linux

The Linux path uses the FreeBSD Linuxulator plus a versioned Linux userland
runtime. Applications should be installed from signed runtime manifests rather
than modifying the host filesystem. The compatibility contract includes:

- ELF and syscall compatibility.
- Linux filesystem layout inside an isolated prefix.
- GPU and audio device access.
- Vulkan/OpenGL loader selection.
- Container or sandbox boundaries for untrusted applications.

### Windows

The Windows path is userland-only:

- Wine provides the Win32 API implementation.
- Proton provides gaming-oriented patches, launch behavior, and integration.
- DXVK and VKD3D-Proton translate DirectX workloads to Vulkan.
- Per-title prefixes prevent one application's dependencies from breaking
  another application's environment.

Windows kernel drivers, anti-cheat systems that require kernel access, and DRM
systems are explicit compatibility risks. They must not be advertised as
supported until verified on target hardware and software.

## Desktop stack

The default desktop stack is intended to be Wayland-first:

- A Nova session manager owns login, power, display, and runtime state.
- PipeWire handles audio and screen capture.
- A compositor provides the interactive shell and game-friendly presentation.
- Portals and sandbox policies mediate file, screen, and device access.

For the UI boot path specifically, the serial kernel shell stays available as a
recovery console and a one-way handoff to a userland session manager. The
handoff should not embed desktop logic in the kernel; instead the kernel exposes a
minimal command contract such as `ui desktop`, `ui compat`, or `ui text`, and
userland `nova-sessiond` decides which service tree to start. The prototype
interface is documented in `userland/uimode/README.md` and implemented in
`userland/uimode/desktop/ui_boot_path.[ch]`.

The initial implementation can consume existing mature components. NovaOS
should only fork a component when its behavior is a product-defining need.

## Update and recovery model

System files should be assembled into immutable image generations. A system
update creates a new generation, validates it, and switches the boot entry.
Keeping previous generations enables rollback after a bad driver or runtime
update. User data and application prefixes live outside the system generation.

## Compatibility priority

1. Native FreeBSD/BSD software and hardware stability.
2. Linux productivity and development applications.
3. Steam and Proton gaming.
4. Windows productivity applications.
5. Unsupported kernel-level Windows software.

This order keeps the project honest and gives each stage a measurable exit
criterion.