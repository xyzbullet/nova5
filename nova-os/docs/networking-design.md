Networking design (initial) — NovaOS

Scope: Provide a clear, BSD-compatible socket API for userland, a modular kernel TCP/IP stack, network device abstraction, and a minimal prototype (loopback + NIC stub) for early verification.

Design principles:
- POSIX/BSD socket API for userland compatibility (nova_socket, nova_bind, etc.).
- Net device abstraction (nova_net_device) with pluggable ops: open, stop, start_xmit, set_mac, get_stats.
- Packet buffer (nova_sk_buff) as the internal packet unit; explicit headroom/tailroom for driver prepends/appends.
- Protocol stack layers: link -> network (IPv4/IPv6/ICMP) -> transport (TCP/UDP) -> socket layer.
- Modular protocol handlers to allow replacement or testing with Rust components where appropriate.

Minimal prototype goals (MVP):
1) Loopback device (lo): registers as NOVA_IFF_LOOPBACK, accepts transmissions and immediately delivers packets back to the stack via nova_netif_receive_skb.
2) NIC stub (nic0): a stub net device that simulates a physical NIC — start_xmit will update tx counters and drop or echo packets; later replaced by real driver.
3) Verify basic socket operations using a simple userland test or kernel unit test that sends/receives on AF_INET/AF_UNIX.

Interfaces to implement in prototype:
- nova_netdev_register / nova_netif_receive_skb / nova_dev_queue_xmit (invoked by NIC/loopback ops)
- Simple start_xmit in nic_stub that increments stats and frees skb
- Loopback start_xmit that clones skb and calls nova_netif_receive_skb

File layout (proposed):
- kernel/network/network_stack.h  (existing)
- kernel/network/loopback.c       (new prototype)
- kernel/network/nic_stub.c       (new prototype)
- userland/test/netloop (simple test harness) — later

Verification steps:
- Build kernel or compile prototypes (as unit modules)
- Confirm that registering devices works (nova_netdev_find("lo") returns device)
- Confirm that sending a packet via nova_dev_queue_xmit/ start_xmit triggers loopback receive

Next actions:
- Add loopback.c and nic_stub.c prototypes (done in this commit)
- Expand kernel/network implementations and add a small userland test harness
- Wire nova_network_init into the kernel bootstrap path after module/device init

Bootstrap and registration path:
- The repository already shows the bootstrap pattern in `boot/kernel.c` and the module-load hook in `kernel/modules/nova_core/nova_core.c` (`MOD_LOAD` / `DECLARE_MODULE`).
- The netdev registry should be initialized in the same boot sequence: `nova_network_init()` -> `nova_loopback_init()` -> `nova_nic_stub_init()` -> `nova_netdev_register()` adds `lo` and `nic0` to the global registry.
- This matches the device-registry pattern in `kernel/io/io_stack.c`, where a global registry is populated at init time and later resolved by name via `nova_netdev_find()`.
