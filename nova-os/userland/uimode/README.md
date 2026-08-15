# NovaOS UI mode and desktop startup

This directory defines the userland-facing shape of NovaOS's UI mode boot path. The
system is intentionally split between:

- the serial kernel shell, which remains available as a diagnostics console and
  emergency recovery path
- userland session services, which own the desktop environment, compositor, login,
  and application launch flow

The goal is to keep the kernel minimal while making the desktop session a real
userland service with explicit startup and shutdown contracts.

## Boot path proposal

The recommended boot sequence is:

1. Bootloader loads the kernel and starts the serial console.
2. Kernel early boot prints a `nova>` shell and exposes a minimal syscall/console
   interface for diagnostics.
3. PID 1 enters the BSD-style init path and starts userland init.
4. Userland init launches the session manager (`nova-sessiond`).
5. `nova-sessiond` inspects the machine profile, display server, and boot target.
6. It chooses one of:
   - `text-shell` mode for recovery or console-only boots
   - `desktop` mode for the normal workstation session
   - `compat` mode for a UI compatibility shell inside a VM or dev environment
7. In desktop mode the session manager starts:
   - compositor (`nova-compositor` or a Wayland compositor backend)
   - login/greeter (`nova-login`)
   - desktop shell (`nova-shell`)
   - launcher service (`nova-launcher`)
8. Once a user session is active, application launchers route to native,
   Linux, or Windows runtime entrypoints.

This is intentionally a userland bootstrap chain. The kernel is not expected to
know what a desktop session looks like beyond a minimal console interface and the
ability to boot a service process.

## Interface between the serial kernel shell and the desktop session

The serial shell remains the kernel-owned console. It should not directly start a
full desktop compositor; instead it hands off through a narrow userland control
interface.

A minimal prototype contract looks like this:

```c
struct nova_ui_boot_plan {
    enum nova_ui_mode mode;
    const char *display_server;
    const char *session_manager;
    const char *desktop_shell;
    const char *compat_shell;
    const char *kernel_console_device;
    const char *vm_display_device;
    uint32_t auto_start_desktop;
    uint32_t enable_compat_shell;
};

int nova_ui_boot_from_serial_shell(struct nova_ui_boot_context *ctx,
                                  const char *command);
int nova_ui_boot_desktop(struct nova_ui_boot_context *ctx);
int nova_ui_launch_compat_shell(struct nova_ui_boot_context *ctx);
```

That lets the kernel shell run a simple command such as `ui desktop` or
`ui compat`, which is translated into a userland launch request rather than
hard-coded desktop behavior in the kernel.

## Required service set

For the UI path to really work, NovaOS needs the following from userland:

- `nova-init` or equivalent PID 1 bootstrap
- `nova-sessiond` to own users, sessions, and display selection
- `nova-displayd` or compositor backend with a Wayland/X11-compatible socket API
- `nova-greeter` or login infrastructure
- `nova-shell` or desktop shell with taskbar, launcher and tray responsibilities
- `nova-launcher` for application launching and runtime detection
- `nova-compat-shell` for QEMU/dev-VM compatibility output

The desktop should not depend on the serial shell after session startup. The
serial console remains a debugging and fallback path, not the primary UI I/O.

## Linux app launching requirements

Linux compatibility depends on the runtime layers already called out in the repo
architecture, but the UI path needs the desktop-side launch wire to work:

- Linux ELF loader and runtime manifest discovery
- Linux /usr filesystem layout inside a controlled prefix
- XDG desktop entry parsing and MIME resolution
- launch wrapper that sets `DISPLAY`, `WAYLAND_DISPLAY`, `XDG_RUNTIME_DIR`, and
  runtime environment variables
- sandbox/portal policy for file, network, and display access
- a launcher status surface so the shell can show `verified`, `running`, or
  `blocked` state for each app

Without these, the desktop can reach the login screen but still cannot safely run
Linux-native software.

## Windows app launching requirements

The Windows path should be userland-only and routed through a compatibility
runtime, not a kernel shim.

Minimum requirements:

- Wine or Proton installation and prefix management
- per-app prefix isolation and dependency resolution
- launch wrappers for `.exe`, `.msi`, and Steam/game shortcuts
- library mapping and Vulkan/DXVK translation into the NovaOS desktop environment
- file and clipboard handoff between VM-hosted and native desktop contexts
- explicit compatibility reporting for unsupported kernel-level Windows drivers

This is separate from the native desktop session; it is simply another runtime
entrypoint that the launcher must know how to invoke.

## UI compatibility shell under QEMU or a dev VM

For QEMU and developer VMs, a compatibility shell is useful even before a full
GPU stack exists. The compatibility shell should:

- provide a minimal desktop or framebuffer shell over a software-rendered output
- allow the serial shell to start a userland session when `-display none` is in use
- bind to a virtual framebuffer, VNC, or virtio-gpu output so the session is
  visible in a dev VM
- expose a fallback text mode for headless CI or remote consoles
- keep the kernel serial console available for debugging while the UI session runs

The practical target is a virtualized display path that does not require a full
production GPU setup. An emulator-native framebuffer or VNC backend is sufficient
for early desktop validation.

## Prototype

The prototype C interface in `userland/uimode/desktop/ui_boot_path.h` models the
boot handoff and keeps the lifecycle states explicit: serial shell ready -> session
manager active -> desktop ready -> application launch. It is intentionally small and
host-testable so the eventual kernel/userland boundary can be implemented without
locking the design to a single compositor or launcher.
