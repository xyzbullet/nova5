# Tier 3: NovaKit-native drivers

New NovaKit-native drivers use Rust for memory-safety benefits. The first
scaffold is `virtio-net`, selected because it is available in QEMU and gives
Phase 2 a deterministic driver-attachment target.

The Rust crate is not yet linked into the FreeBSD kernel. Its next integration
step is a narrow C-compatible NovaKit registration bridge and a real virtqueue
implementation.