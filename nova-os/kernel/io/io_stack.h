/*
 * NovaOS I/O Stack - Core Interfaces
 * Rapid Kernel Phase 3.1
 * 
 * This file defines the core I/O stack interfaces for NovaOS,
 * including device registration, block I/O, character devices,
 * and async I/O completion ports.
 */

#ifndef NOVA_IO_STACK_H
#define NOVA_IO_STACK_H

#include <stddef.h>
#include <stdint.h>

/* I/O Request types */
enum nova_io_type {
    NOVA_IO_READ = 1,
    NOVA_IO_WRITE = 2,
    NOVA_IO_IOCTL = 3,
    NOVA_IO_FLUSH = 4,
    NOVA_IO_DISCARD = 5,
};

/* I/O status codes */
enum nova_io_status {
    NOVA_IO_PENDING = 0,
    NOVA_IO_SUCCESS = 1,
    NOVA_IO_ERROR = -1,
    NOVA_IO_CANCELLED = -2,
};

/* Device types in the I/O registry */
enum nova_device_type {
    NOVA_DEVICE_BLOCK = 1,
    NOVA_DEVICE_CHAR = 2,
    NOVA_DEVICE_NETWORK = 3,
    NOVA_DEVICE_FRAMEBUFFER = 4,
    NOVA_DEVICE_INPUT = 5,
    NOVA_DEVICE_AUDIO = 6,
};

/* Forward declarations */
struct nova_io_request;
struct nova_device;
struct nova_io_completion;

/* I/O completion callback */
typedef void (*nova_io_callback_t)(struct nova_io_request *req, void *ctx);

/* I/O request structure for async operations */
struct nova_io_request {
    enum nova_io_type type;
    enum nova_io_status status;
    struct nova_device *device;
    uint64_t offset;           /* Block offset for block devices */
    void *buffer;              /* Data buffer */
    size_t length;             /* Transfer length */
    uint32_t flags;
    uint64_t timeout_ms;
    nova_io_callback_t callback;
    void *callback_ctx;
    struct nova_io_completion *completion;
    int error_code;
};

/* Block device geometry */
struct nova_block_geometry {
    uint64_t total_sectors;
    uint32_t sector_size;
    uint32_t cylinders;
    uint32_t heads;
    uint32_t sectors_per_track;
};

/* Character device operations */
struct nova_char_ops {
    int (*open)(struct nova_device *dev, int flags);
    int (*close)(struct nova_device *dev);
    ssize_t (*read)(struct nova_device *dev, void *buf, size_t len, uint64_t offset);
    ssize_t (*write)(struct nova_device *dev, const void *buf, size_t len, uint64_t offset);
    int (*ioctl)(struct nova_device *dev, unsigned long cmd, void *arg);
};

/* Block device operations */
struct nova_block_ops {
    int (*open)(struct nova_device *dev, int flags);
    int (*close)(struct nova_device *dev);
    int (*submit_io)(struct nova_device *dev, struct nova_io_request *req);
    int (*flush)(struct nova_device *dev);
    int (*get_geometry)(struct nova_device *dev, struct nova_block_geometry *geo);
};

/* Network device operations */
struct nova_net_ops {
    int (*open)(struct nova_device *dev);
    int (*close)(struct nova_device *dev);
    int (*transmit)(struct nova_device *dev, const void *data, size_t len);
    int (*receive)(struct nova_device *dev, void *buf, size_t maxlen);
    int (*set_mac)(struct nova_device *dev, const uint8_t mac[6]);
    int (*get_mac)(struct nova_device *dev, uint8_t mac[6]);
};

/* Framebuffer device info */
struct nova_fb_info {
    uint32_t width;
    uint32_t height;
    uint32_t pitch;
    uint8_t bits_per_pixel;
    uint8_t red_mask_size;
    uint8_t red_field_position;
    uint8_t green_mask_size;
    uint8_t green_field_position;
    uint8_t blue_mask_size;
    uint8_t blue_field_position;
};

/* Framebuffer device operations */
struct nova_fb_ops {
    int (*open)(struct nova_device *dev);
    int (*close)(struct nova_device *dev);
    int (*get_info)(struct nova_device *dev, struct nova_fb_info *info);
    void *(*map_buffer)(struct nova_device *dev);
    int (*unmap_buffer)(struct nova_device *dev);
    int (*set_palette)(struct nova_device *dev, uint32_t index, uint8_t r, uint8_t g, uint8_t b);
};

/* Input device event types */
enum nova_input_event_type {
    NOVA_INPUT_KEY_PRESS = 1,
    NOVA_INPUT_KEY_RELEASE = 2,
    NOVA_INPUT_MOUSE_MOVE = 3,
    NOVA_INPUT_MOUSE_BUTTON = 4,
    NOVA_INPUT_MOUSE_SCROLL = 5,
    NOVA_INPUT_TOUCH = 6,
};

/* Input event structure */
struct nova_input_event {
    enum nova_input_event_type type;
    uint32_t code;
    int32_t value;
    uint64_t timestamp_ns;
};

/* Input device operations */
struct nova_input_ops {
    int (*open)(struct nova_device *dev);
    int (*close)(struct nova_device *dev);
    int (*read_event)(struct nova_device *dev, struct nova_input_event *event);
    int (*poll)(struct nova_device *dev, int timeout_ms);
};

/* Generic device structure */
struct nova_device {
    char name[64];
    char path[128];
    enum nova_device_type type;
    uint32_t major;
    uint32_t minor;
    int ref_count;
    int flags;
    void *private_data;
    
    union {
        struct nova_char_ops *char_ops;
        struct nova_block_ops *block_ops;
        struct nova_net_ops *net_ops;
        struct nova_fb_ops *fb_ops;
        struct nova_input_ops *input_ops;
    };
    
    struct nova_device *next;
    struct nova_device *parent;
};

/* I/O completion port for async I/O */
struct nova_io_completion_port {
    void *queue;               /* Internal queue implementation */
    uint32_t max_events;
    uint32_t timeout_ms;
};

/* I/O scheduler types */
enum nova_io_scheduler {
    NOVA_SCHEDULER_NOOP = 1,
    NOVA_SCHEDULER_DEADLINE = 2,
    NOVA_SCHEDULER_CFQ = 3,
};

/* Function prototypes */
int nova_io_init(void);
int nova_io_shutdown(void);

/* Device registration */
int nova_device_register(struct nova_device *dev);
int nova_device_unregister(struct nova_device *dev);
struct nova_device *nova_device_find(const char *path);
struct nova_device *nova_device_get_by_major_minor(uint32_t major, uint32_t minor);

/* I/O request lifecycle */
struct nova_io_request *nova_io_request_alloc(void);
void nova_io_request_free(struct nova_io_request *req);
int nova_io_submit(struct nova_io_request *req);
int nova_io_wait(struct nova_io_request *req, uint32_t timeout_ms);
int nova_io_cancel(struct nova_io_request *req);

/* Completion port operations */
struct nova_io_completion_port *nova_io_completion_port_create(uint32_t max_events);
int nova_io_completion_port_destroy(struct nova_io_completion_port *port);
int nova_io_completion_port_associate(struct nova_io_completion_port *port, 
                                       struct nova_device *dev, uintptr_t key);
int nova_io_completion_port_get_event(struct nova_io_completion_port *port,
                                       struct nova_io_request **req,
                                       uint32_t timeout_ms);

/* Block device helpers */
int nova_block_read_sectors(struct nova_device *dev, uint64_t sector, 
                            void *buf, uint32_t count);
int nova_block_write_sectors(struct nova_device *dev, uint64_t sector,
                             const void *buf, uint32_t count);

#endif /* NOVA_IO_STACK_H */
