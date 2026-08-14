# Third-party license register

This is a register, not a license grant. Imported code must retain its own
copyright and license notices, and every release requires an IP/licensing
review.

| Component | Intended use | License boundary | Vendored in this phase |
| --- | --- | --- | --- |
| FreeBSD source tree | Kernel, BSD personality, native drivers | BSD-family upstream notices retained | No |
| FreeBSD Linuxulator | Linux ELF/syscall compatibility | FreeBSD upstream notices retained | No |
| LinuxKPI / drm-kmod | Isolated GPU and Wi-Fi driver modules | GPL components stay separate from BSD core | No |
| Wine | Win32 and PE compatibility | Upstream LGPL; userspace process | No |
| Proton | Gaming-oriented Wine integration | Upstream project licenses | No |
| DXVK | Direct3D 9/10/11 to Vulkan | Upstream project license | No |
| VKD3D-Proton | Direct3D 12 to Vulkan | Upstream project license | No |
| FAudio | XAudio-compatible audio runtime | Upstream project license | No |

## Non-negotiable exclusions

- No Microsoft source, headers, symbols, binaries, or other Windows-derived
  artifacts.
- No ReactOS source imports.
- No GPL-derived code statically linked into the BSD-licensed kernel core.
- No copied source without a recorded upstream revision and license notice.