# Tier 2: LinuxKPI

LinuxKPI is the realistic path to modern GPU and Wi-Fi hardware. This
directory contains only NovaOS's registration boundary and build notes; it
does not vendor Linux or drm-kmod source yet.

GPL-licensed LinuxKPI and drm-kmod modules must remain isolated loadable
modules and must never be statically linked into the BSD-licensed kernel core.
Every imported module needs an upstream revision and an entry in
`THIRDPARTY-LICENSES.md`.

`example_module.c` is a host-testable registration check only. It is not a
ported GPU or Wi-Fi driver and does not create a hardware support claim.