#![no_std]

//! NovaKit-native virtio-net driver boundary.
//!
//! This is an intentionally small scaffold. PCI probing, virtqueue ownership,
//! DMA mapping, interrupts, and the C-compatible NovaKit registration bridge
//! are not implemented until the FreeBSD bus integration is available.

pub const VIRTIO_VENDOR_ID: u16 = 0x1AF4;
pub const VIRTIO_NET_DEVICE_ID: u16 = 0x1000;

#[derive(Clone, Copy, Debug, Eq, PartialEq)]
pub enum DriverState {
    Detached,
    Probed,
}

pub struct VirtioNet {
    state: DriverState,
}

impl VirtioNet {
    pub const fn new() -> Self {
        Self {
            state: DriverState::Detached,
        }
    }

    pub const fn state(&self) -> DriverState {
        self.state
    }
}