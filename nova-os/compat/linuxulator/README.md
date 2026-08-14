# Linuxulator compatibility

Phase 2 expands the existing Linux boundary into explicit ELF and syscall
milestones:

1. Static Linux `hello world`.
2. Dynamic glibc or musl CLI binary.
3. GUI toolkit smoke test.

This directory owns runtime manifests and test descriptions. It does not
pretend to execute Linux binaries on the current host until a FreeBSD
Linuxulator runtime is attached.