/*
 * NovaOS UI boot path prototype.
 *
 * This is a minimal, host-testable contract for moving from the kernel serial
 * shell into a userland desktop session. It intentionally stays small so the
 * eventual BSD/POSIX implementation can evolve around a fixed state machine.
 */

#ifndef NOVA_UI_BOOT_PATH_H
#define NOVA_UI_BOOT_PATH_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

enum nova_ui_mode {
    NOVA_UI_MODE_TEXT = 0,
    NOVA_UI_MODE_DESKTOP = 1,
    NOVA_UI_MODE_COMPAT = 2,
};

enum nova_ui_event {
    NOVA_UI_EVENT_SERIAL_READY = 1,
    NOVA_UI_EVENT_SESSION_MANAGER_READY = 2,
    NOVA_UI_EVENT_DESKTOP_READY = 3,
    NOVA_UI_EVENT_COMPAT_SHELL_READY = 4,
    NOVA_UI_EVENT_LINUX_APP_LAUNCHED = 5,
    NOVA_UI_EVENT_WINDOWS_APP_LAUNCHED = 6,
};

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

struct nova_ui_boot_context {
    struct nova_ui_boot_plan plan;
    enum nova_ui_mode current_mode;
    int serial_ready;
    int userland_ready;
    int session_started;
    int desktop_ready;
    int compat_shell_ready;
};

int nova_ui_boot_prepare(struct nova_ui_boot_context *ctx,
                         const struct nova_ui_boot_plan *plan);
int nova_ui_boot_from_serial_shell(struct nova_ui_boot_context *ctx,
                                  const char *command);
int nova_ui_boot_desktop(struct nova_ui_boot_context *ctx);
int nova_ui_launch_compat_shell(struct nova_ui_boot_context *ctx);
int nova_ui_launch_linux_app(struct nova_ui_boot_context *ctx,
                            const char *path,
                            const char *args);
int nova_ui_launch_windows_app(struct nova_ui_boot_context *ctx,
                              const char *path,
                              const char *args);

#ifdef __cplusplus
}
#endif

#endif /* NOVA_UI_BOOT_PATH_H */
