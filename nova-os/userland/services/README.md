# NovaOS userland services

Services in this directory are long-running pieces of the desktop experience,
not application compatibility runtimes. Each service should have:

- A narrow responsibility and documented IPC contract.
- A foreground mode for supervision and testing.
- Structured logs suitable for support bundles.
- Explicit privilege boundaries.
- A shutdown and recovery behavior.

Initial service candidates are session management, hardware profile selection,
runtime discovery, update generation management, and compatibility diagnostics.