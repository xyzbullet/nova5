Compatibility notes — Rapid + FreeBSD base + macOS hybrid ideas

Goal: combine Rapid kernel and FreeBSD base with macOS hybrid-kernel concepts into a new hybrid approach. This is intentionally complex and higher-risk; these notes outline initial compatibility considerations.

1) Kernel architecture compatibility
- FreeBSD kernel is monolithic-modular; Rapid kernel appears to be a project layering on top of FreeBSD-like interfaces.
- macOS hybrid kernel (XNU) blends Mach microkernel IPC with BSD services. Key ideas to consider:
  - Separate low-level Mach-like IPC/messaging and scheduler concepts from BSD networking and VFS.
  - Keep BSD socket and POSIX compatibility layers for userland compatibility.
- Approach: define a hybrid design where Rapid/FreeBSD provide BSD-compatible services (VFS, networking, POSIX), while essential low-level primitives (scheduler, IPC, low-level object abstractions) can adopt hybrid patterns inspired by XNU if beneficial.

2) ABI and syscall compatibility
- Preserve FreeBSD/BSD syscall set where feasible to ease porting userland components.
- Provide a compatibility shim layer for any macOS-specific kernel interfaces only if necessary — prefer userland adaptations to kernel ABI changes.

3) Drivers model
- Reuse drivers/linuxkpi and native drivers where possible. For macOS hybrid ideas, evaluate driver isolation boundaries but prefer to maintain existing driver ABI to reduce porting cost.

4) Networking stack
- Maintain BSD-style socket API for userland compatibility.
- Implement netdev abstraction that can accept devices from multiple driver models (native, linuxkpi). This repository already has a clear nova_net_device abstraction (kernel/network/network_stack.h).

5) Risks and mitigation
- Risk: merging hybrid kernel ideas will increase complexity and delay. Mitigation: phase hybrid concepts — keep BSD-compatible core first, then refactor low-level primitives behind well-defined interfaces.

6) Next steps
- Produce a mapping doc that lists Rapid kernel files vs FreeBSD equivalents and where macOS hybrid concepts could plug in. This will be part of repo-analysis deliverables.
