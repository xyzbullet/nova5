# Windows compatibility layer

This directory owns the userland Wine and Proton integration. It does not
provide Windows kernel drivers.

Planned components:

- Version-pinned Wine and Proton runtime manifests.
- Per-application prefixes.
- DXVK, VKD3D-Proton, Gamescope, and controller integration.
- A compatibility status database with verified, community-tested, and
  experimental states.
- Diagnostics for prefixes, graphics translation, audio, and launch failures.

Applications requiring Windows kernel components remain unsupported until a safe
and documented alternative is available.