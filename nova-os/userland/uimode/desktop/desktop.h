/*
 * NovaOS Desktop Environment - UIMode Core
 * Rapid Kernel Phase 3.1
 * 
 * This file defines the core desktop environment interfaces for NovaOS,
 * including window management, compositor, and desktop shell.
 */

#ifndef NOVA_UIMODE_DESKTOP_H
#define NOVA_UIMODE_DESKTOP_H

#include <stdint.h>
#include <stddef.h>

/* Window types */
enum nova_window_type {
    NOVA_WINDOW_NORMAL = 0,
    NOVA_WINDOW_DIALOG = 1,
    NOVA_WINDOW_POPUP = 2,
    NOVA_WINDOW_TOOLBAR = 3,
    NOVA_WINDOW_MENU = 4,
    NOVA_WINDOW_FULLSCREEN = 5,
};

/* Window states */
enum nova_window_state {
    NOVA_WINDOW_STATE_NORMAL = 0,
    NOVA_WINDOW_STATE_MINIMIZED = 1,
    NOVA_WINDOW_STATE_MAXIMIZED = 2,
    NOVA_WINDOW_STATE_FULLSCREEN = 4,
    NOVA_WINDOW_STATE_ACTIVE = 8,
    NOVA_WINDOW_STATE_HIDDEN = 16,
};

/* Cursor types */
enum nova_cursor_type {
    NOVA_CURSOR_ARROW = 0,
    NOVA_CURSOR_IBEAM = 1,
    NOVA_CURSOR_WAIT = 2,
    NOVA_CURSOR_CROSS = 3,
    NOVA_CURSOR_UPARROW = 4,
    NOVA_CURSOR_SIZE = 5,
    NOVA_CURSOR_SIZEWE = 6,
    NOVA_CURSOR_SIZENS = 7,
    NOVA_CURSOR_SIZEALL = 8,
    NOVA_CURSOR_NO = 9,
    NOVA_CURSOR_HAND = 10,
};

/* Color structure */
struct nova_color {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
};

/* Rectangle structure */
struct nova_rect {
    int32_t x;
    int32_t y;
    uint32_t width;
    uint32_t height;
};

/* Point structure */
struct nova_point {
    int32_t x;
    int32_t y;
};

/* Window properties */
struct nova_window_props {
    char title[256];
    enum nova_window_type type;
    struct nova_rect bounds;
    uint32_t flags;
    void *icon_data;
    size_t icon_size;
};

/* Window events */
enum nova_window_event_type {
    NOVA_EVENT_WINDOW_CREATE = 1,
    NOVA_EVENT_WINDOW_DESTROY = 2,
    NOVA_EVENT_WINDOW_MOVE = 3,
    NOVA_EVENT_WINDOW_RESIZE = 4,
    NOVA_EVENT_WINDOW_FOCUS = 5,
    NOVA_EVENT_WINDOW_BLUR = 6,
    NOVA_EVENT_WINDOW_MINIMIZE = 7,
    NOVA_EVENT_WINDOW_MAXIMIZE = 8,
    NOVA_EVENT_WINDOW_CLOSE = 9,
};

/* Window event structure */
struct nova_window_event {
    enum nova_window_event_type type;
    uint32_t window_id;
    union {
        struct nova_rect bounds;
        int focused;
    } data;
    uint64_t timestamp;
};

/* Forward declarations */
struct nova_window;
struct nova_compositor;
struct nova_desktop;

/* Window operations */
struct nova_window_ops {
    int (*create)(struct nova_window *win, const struct nova_window_props *props);
    int (*destroy)(struct nova_window *win);
    int (*show)(struct nova_window *win);
    int (*hide)(struct nova_window *win);
    int (*move)(struct nova_window *win, int32_t x, int32_t y);
    int (*resize)(struct nova_window *win, uint32_t width, uint32_t height);
    int (*set_title)(struct nova_window *win, const char *title);
    int (*set_focus)(struct nova_window *win);
    int (*minimize)(struct nova_window *win);
    int (*maximize)(struct nova_window *win);
    int (*restore)(struct nova_window *win);
    int (*close)(struct nova_window *win);
};

/* Window structure */
struct nova_window {
    uint32_t id;
    struct nova_window_props props;
    enum nova_window_state state;
    struct nova_window_ops *ops;
    void *surface;           /* Rendering surface */
    void *private_data;
    struct nova_window *parent;
    struct nova_window *next;
    struct nova_window *prev;
};

/* Compositor rendering modes */
enum nova_compositor_mode {
    NOVA_COMP_SOFTWARE = 0,
    NOVA_COMP_HARDWARE = 1,
    NOVA_COMP_MIXED = 2,
};

/* Compositor configuration */
struct nova_compositor_config {
    enum nova_compositor_mode mode;
    uint32_t vsync_enabled;
    uint32_t triple_buffering;
    uint32_t cursor_composition;
    struct nova_color background_color;
};

/* Compositor operations */
struct nova_compositor_ops {
    int (*init)(struct nova_compositor *comp, const struct nova_compositor_config *config);
    int (*shutdown)(struct nova_compositor *comp);
    int (*add_window)(struct nova_compositor *comp, struct nova_window *win);
    int (*remove_window)(struct nova_compositor *comp, struct nova_window *win);
    int (*composite)(struct nova_compositor *comp);
    int (*set_cursor)(struct nova_compositor *comp, enum nova_cursor_type cursor);
    int (*move_cursor)(struct nova_compositor *comp, int32_t x, int32_t y);
    int (*update_region)(struct nova_compositor *comp, struct nova_rect *region);
};

/* Compositor structure */
struct nova_compositor {
    struct nova_compositor_config config;
    struct nova_compositor_ops *ops;
    struct nova_window *windows;
    uint32_t screen_width;
    uint32_t screen_height;
    uint32_t bits_per_pixel;
    void *framebuffer;
    void *cursor_surface;
    struct nova_point cursor_pos;
    enum nova_cursor_type cursor_type;
    int cursor_visible;
};

/* Desktop taskbar position */
enum nova_taskbar_pos {
    NOVA_TASKBAR_BOTTOM = 0,
    NOVA_TASKBAR_TOP = 1,
    NOVA_TASKBAR_LEFT = 2,
    NOVA_TASKBAR_RIGHT = 3,
};

/* Desktop configuration */
struct nova_desktop_config {
    char wallpaper_path[256];
    struct nova_color background_color;
    enum nova_taskbar_pos taskbar_position;
    uint32_t taskbar_height;
    uint32_t show_desktop_icons;
    char font_name[64];
    uint32_t font_size;
};

/* Desktop operations */
struct nova_desktop_ops {
    int (*init)(struct nova_desktop *desk, const struct nova_desktop_config *config);
    int (*shutdown)(struct nova_desktop *desk);
    int (*set_wallpaper)(struct nova_desktop *desk, const char *path);
    int (*refresh)(struct nova_desktop *desk);
    int (*show_context_menu)(struct nova_desktop *desk, int32_t x, int32_t y);
};

/* Desktop structure */
struct nova_desktop {
    struct nova_desktop_config config;
    struct nova_desktop_ops *ops;
    struct nova_compositor *compositor;
    void *wallpaper_surface;
    struct nova_window *desktop_icons;
    struct nova_window *taskbar;
};

/* Taskbar item types */
enum nova_taskbar_item_type {
    NOVA_TASKBAR_ITEM_APP = 0,
    NOVA_TASKBAR_ITEM_TRAY = 1,
    NOVA_TASKBAR_ITEM_SYSTEM = 2,
};

/* Taskbar item */
struct nova_taskbar_item {
    enum nova_taskbar_item_type type;
    char title[128];
    void *icon;
    uint32_t window_id;
    uint32_t flags;
    struct nova_taskbar_item *next;
};

/* UI Mode initialization */
int nova_uimode_init(void);
int nova_uimode_shutdown(void);

/* Desktop lifecycle */
int nova_desktop_create(struct nova_desktop **desk, const struct nova_desktop_config *config);
int nova_desktop_destroy(struct nova_desktop *desk);

/* Compositor lifecycle */
int nova_compositor_create(struct nova_compositor **comp, const struct nova_compositor_config *config);
int nova_compositor_destroy(struct nova_compositor *comp);

/* Window management */
int nova_window_create(struct nova_window **win, const struct nova_window_props *props);
int nova_window_destroy(struct nova_window *win);
struct nova_window *nova_window_find(uint32_t id);
int nova_window_bring_to_front(struct nova_window *win);
int nova_window_send_to_back(struct nova_window *win);

/* Event handling */
int nova_poll_events(struct nova_window_event *event, int timeout_ms);
int nova_dispatch_event(const struct nova_window_event *event);

/* Cursor management */
int nova_set_cursor(enum nova_cursor_type cursor);
int nova_show_cursor(void);
int nova_hide_cursor(void);

/* Graphics primitives */
int nova_draw_rect(struct nova_rect *rect, struct nova_color *color);
int nova_draw_line(int32_t x0, int32_t y0, int32_t x1, int32_t y1, struct nova_color *color);
int nova_draw_text(int32_t x, int32_t y, const char *text, struct nova_color *color);
int nova_blit_surface(void *src, struct nova_rect *src_rect, 
                      void *dst, struct nova_rect *dst_rect);

#endif /* NOVA_UIMODE_DESKTOP_H */
