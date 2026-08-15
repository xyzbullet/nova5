#include "ui_boot_path.h"

#include <stddef.h>
#include <string.h>

static int starts_with(const char *value, const char *prefix)
{
    size_t prefix_len = strlen(prefix);
    return strncmp(value, prefix, prefix_len) == 0;
}

static int is_ui_command(const char *command, const char *name)
{
    return command != NULL && strcmp(command, name) == 0;
}

int nova_ui_boot_prepare(struct nova_ui_boot_context *ctx,
                         const struct nova_ui_boot_plan *plan)
{
    if (ctx == NULL || plan == NULL)
        return -1;

    ctx->plan = *plan;
    ctx->current_mode = plan->mode;
    ctx->serial_ready = 1;
    ctx->userland_ready = 0;
    ctx->session_started = 0;
    ctx->desktop_ready = 0;
    ctx->compat_shell_ready = 0;
    return 0;
}

int nova_ui_boot_from_serial_shell(struct nova_ui_boot_context *ctx,
                                  const char *command)
{
    if (ctx == NULL)
        return -1;

    if (command == NULL)
        return -1;

    if (is_ui_command(command, "desktop") ||
        starts_with(command, "ui desktop") ||
        starts_with(command, "start desktop")) {
        ctx->current_mode = NOVA_UI_MODE_DESKTOP;
        return nova_ui_boot_desktop(ctx);
    }

    if (is_ui_command(command, "compat") ||
        starts_with(command, "ui compat") ||
        starts_with(command, "compat shell")) {
        ctx->current_mode = NOVA_UI_MODE_COMPAT;
        return nova_ui_launch_compat_shell(ctx);
    }

    if (is_ui_command(command, "text") || starts_with(command, "ui text")) {
        ctx->current_mode = NOVA_UI_MODE_TEXT;
        ctx->desktop_ready = 0;
        ctx->compat_shell_ready = 0;
        return 0;
    }

    return -2;
}

int nova_ui_boot_desktop(struct nova_ui_boot_context *ctx)
{
    if (ctx == NULL)
        return -1;

    ctx->userland_ready = 1;
    ctx->session_started = 1;
    ctx->desktop_ready = 1;
    ctx->current_mode = NOVA_UI_MODE_DESKTOP;
    return 0;
}

int nova_ui_launch_compat_shell(struct nova_ui_boot_context *ctx)
{
    if (ctx == NULL)
        return -1;

    if (ctx->plan.enable_compat_shell == 0 && ctx->plan.vm_display_device == NULL)
        return -2;

    ctx->compat_shell_ready = 1;
    ctx->current_mode = NOVA_UI_MODE_COMPAT;
    ctx->desktop_ready = 1;
    ctx->session_started = 1;
    return 0;
}

int nova_ui_launch_linux_app(struct nova_ui_boot_context *ctx,
                            const char *path,
                            const char *args)
{
    (void)args;

    if (ctx == NULL || path == NULL)
        return -1;

    if (ctx->desktop_ready == 0)
        return -2;

    ctx->current_mode = NOVA_UI_MODE_DESKTOP;
    return 0;
}

int nova_ui_launch_windows_app(struct nova_ui_boot_context *ctx,
                              const char *path,
                              const char *args)
{
    (void)args;

    if (ctx == NULL || path == NULL)
        return -1;

    if (ctx->desktop_ready == 0)
        return -2;

    ctx->current_mode = NOVA_UI_MODE_DESKTOP;
    return 0;
}
