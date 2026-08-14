# NovaOS

FreeBSD-derived desktop operating system foundation for gaming, creative work,
and general-purpose computing with Linux and Windows application compatibility.

## Run & Operate

- `pnpm --filter @workspace/api-server run dev` — run the API server (port 5000)
- `pnpm run typecheck` — full typecheck across all packages
- `pnpm run build` — typecheck + build all packages
- `pnpm --filter @workspace/api-spec run codegen` — regenerate API hooks and Zod schemas from the OpenAPI spec
- `pnpm --filter @workspace/db run push` — push DB schema changes (dev only)
- Required env: `DATABASE_URL` — Postgres connection string

## Stack

- pnpm workspaces, Node.js 24, TypeScript 5.9
- API: Express 5
- DB: PostgreSQL + Drizzle ORM
- Validation: Zod (`zod/v4`), `drizzle-zod`
- API codegen: Orval (from OpenAPI spec)
- Build: esbuild (CJS bundle)

## Where things live

- `nova-os/README.md` — project scope, principles, and development commands
- `nova-os/docs/architecture.md` — source of truth for the hybrid integration model
- `nova-os/docs/compatibility.md` — compatibility boundaries and test matrix
- `nova-os/docs/roadmap.md` — staged implementation plan
- `nova-os/kernel/nova.conf` — FreeBSD kernel configuration overlay
- `nova-os/compat/` — Linuxulator and Wine/Proton integration boundaries

## Architecture decisions

- NovaOS begins as a FreeBSD-derived distribution with an upstream-friendly
  overlay, rather than copying the full FreeBSD source tree into the project.
- The hybrid architecture combines a FreeBSD kernel base with userland
  compatibility layers; Linux and Windows application runtimes stay out of
  kernel mode.
- Linux support is planned around the FreeBSD Linuxulator, and Windows support
  around Wine/Proton, with explicit compatibility status labels.
- The first target is amd64 UEFI, with reproducible images and rollback before
  broad hardware certification.

## Product

NovaOS aims to provide a stable desktop for games, video/audio/image editing,
development, and everyday applications. Its compatibility strategy prioritizes
native stability, then Linux applications, Steam/Proton gaming, and Windows
desktop software.

## User preferences

_Populate as you build — explicit user instructions worth remembering across sessions._

## Gotchas

_Populate as you build — sharp edges, "always run X before Y" rules._

## Pointers

- See the `pnpm-workspace` skill for workspace structure, TypeScript setup, and package details
