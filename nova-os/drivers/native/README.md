# Tier 1: native drivers

This directory is reserved for drivers inherited from the FreeBSD upstream
tree. Each import must record its upstream revision, preserve license notices,
and expose its attachment state through NovaKit.

The first QEMU target is virtio-net or virtio-blk. It is not marked supported
until the device attaches and appears in the registry during a boot test.