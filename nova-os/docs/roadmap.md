# NovaOS roadmap

## Phase 0: foundation

- Keep the repository structure and license boundaries clear.
- Pin a FreeBSD upstream branch and record the import process.
- Define amd64 UEFI as the first boot target.
- Add CI checks for shell scripts, kernel module compilation, and configuration.

## Phase 1: bootable developer image

- Build a reproducible FreeBSD-derived image.
- Boot to a minimal Wayland session in a virtual machine.
- Add serial and desktop diagnostics.
- Implement an atomic system-generation manifest and rollback command.

## Phase 2: hybrid kernel and compatibility foundation

- [x] Record the three-layer architecture and choose Rust for new NovaKit
  drivers.
- [x] Add host-testable scheduler, IPC, and virtual-memory core interfaces.
- [x] Add a host-testable NovaKit device registry and virtio-net driver
  scaffold.
- [x] Add LinuxKPI isolation boundaries and license tracking.
- [x] Add Linuxulator and Wine/Proton runtime manifests with explicit
  not-run compatibility gates.
- [ ] Attach the FreeBSD upstream tree and wire the core adapters.
- [ ] Boot a BSD-layer shell in QEMU.
- [ ] Attach a virtio driver through NovaKit during a QEMU boot.
- [ ] Execute static Linux ELF and Win32 console hello-world binaries.

The checked items are foundation work. They do not claim that the remaining
runtime or boot validation gates have passed.

## Phase 3: gaming path

- Package Steam, Proton, DXVK, VKD3D-Proton, and Gamescope.
- Add controller, shader-cache, fullscreen, and game-save integration.
- Publish a transparent compatibility report.
- Certify a small set of AMD and Intel GPU profiles.

## Phase 4: creative workstation

- Tune PipeWire for low-latency audio.
- Add capture, encoding, color-management, and GPU compute profiles.
- Validate an initial set of editing, 3D, audio, and image workflows.

## Phase 5: Windows application path

- Add per-application Wine prefixes and dependency management.
- Add desktop integration for fonts, clipboard, file dialogs, and MIME types.
- Track applications that require kernel-level Windows components as
  unsupported unless a safe alternative exists.

## Definition of an initial release

An initial release is ready only when it can install, boot, update, roll back,
play a verified game, run a verified Linux application, and complete a verified
creative workflow on documented hardware.