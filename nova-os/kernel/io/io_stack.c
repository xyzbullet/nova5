/*
 * NovaOS I/O Stack - Core Implementation
 * Rapid Kernel Phase 3.1
 */

#include "io_stack.h"
#include <string.h>
#include <stdlib.h>

/* Global device registry */
static struct nova_device *g_device_registry = NULL;
static uint32_t g_next_major = 1;
static int g_io_initialized = 0;

/* Simple spinlock placeholder - to be replaced with proper kernel sync */
static volatile int g_registry_lock = 0;

static void lock_registry(void) {
    while (__sync_lock_test_and_set(&g_registry_lock, 1)) {
        /* spin */
    }
}

static void unlock_registry(void) {
    __sync_lock_release(&g_registry_lock);
}

int nova_io_init(void) {
    if (g_io_initialized) {
        return 0;
    }
    
    g_device_registry = NULL;
    g_next_major = 1;
    g_io_initialized = 1;
    
    return 0;
}

int nova_io_shutdown(void) {
    struct nova_device *dev, *next;
    
    if (!g_io_initialized) {
        return 0;
    }
    
    lock_registry();
    dev = g_device_registry;
    while (dev != NULL) {
        next = dev->next;
        free(dev);
        dev = next;
    }
    g_device_registry = NULL;
    unlock_registry();
    
    g_io_initialized = 0;
    return 0;
}

int nova_device_register(struct nova_device *dev) {
    if (dev == NULL || dev->name[0] == '\0') {
        return -1;
    }
    
    lock_registry();
    
    /* Assign major number if not already assigned */
    if (dev->major == 0) {
        dev->major = g_next_major++;
    }
    
    /* Add to registry */
    dev->next = g_device_registry;
    if (g_device_registry != NULL) {
        g_device_registry->prev = dev;
    }
    g_device_registry = dev;
    
    unlock_registry();
    
    return 0;
}

int nova_device_unregister(struct nova_device *dev) {
    struct nova_device *curr, *prev;
    
    if (dev == NULL) {
        return -1;
    }
    
    lock_registry();
    
    prev = NULL;
    curr = g_device_registry;
    while (curr != NULL) {
        if (curr == dev) {
            if (prev != NULL) {
                prev->next = curr->next;
            } else {
                g_device_registry = curr->next;
            }
            if (curr->next != NULL) {
                curr->next->prev = prev;
            }
            unlock_registry();
            return 0;
        }
        prev = curr;
        curr = curr->next;
    }
    
    unlock_registry();
    return -1;  /* Device not found */
}

struct nova_device *nova_device_find(const char *path) {
    struct nova_device *dev;
    
    if (path == NULL) {
        return NULL;
    }
    
    lock_registry();
    dev = g_device_registry;
    while (dev != NULL) {
        if (strcmp(dev->path, path) == 0) {
            unlock_registry();
            return dev;
        }
        dev = dev->next;
    }
    unlock_registry();
    
    return NULL;
}

struct nova_device *nova_device_get_by_major_minor(uint32_t major, uint32_t minor) {
    struct nova_device *dev;
    
    lock_registry();
    dev = g_device_registry;
    while (dev != NULL) {
        if (dev->major == major && dev->minor == minor) {
            unlock_registry();
            return dev;
        }
        dev = dev->next;
    }
    unlock_registry();
    
    return NULL;
}

struct nova_io_request *nova_io_request_alloc(void) {
    struct nova_io_request *req;
    
    req = (struct nova_io_request *)calloc(1, sizeof(struct nova_io_request));
    if (req != NULL) {
        req->status = NOVA_IO_PENDING;
        req->flags = 0;
        req->timeout_ms = 5000;  /* Default 5 second timeout */
    }
    
    return req;
}

void nova_io_request_free(struct nova_io_request *req) {
    if (req != NULL) {
        free(req);
    }
}

int nova_io_submit(struct nova_io_request *req) {
    if (req == NULL || req->device == NULL || req->buffer == NULL) {
        return -1;
    }
    
    switch (req->device->type) {
        case NOVA_DEVICE_BLOCK:
            if (req->device->block_ops && req->device->block_ops->submit_io) {
                return req->device->block_ops->submit_io(req->device, req);
            }
            break;
            
        case NOVA_DEVICE_CHAR:
            /* Handle character device I/O */
            break;
            
        default:
            break;
    }
    
    req->status = NOVA_IO_ERROR;
    req->error_code = -1;
    return -1;
}

int nova_io_wait(struct nova_io_request *req, uint32_t timeout_ms) {
    /* Placeholder - in real implementation this would wait on completion */
    (void)req;
    (void)timeout_ms;
    return 0;
}

int nova_io_cancel(struct nova_io_request *req) {
    if (req == NULL) {
        return -1;
    }
    
    if (req->status != NOVA_IO_PENDING) {
        return -1;  /* Can only cancel pending requests */
    }
    
    req->status = NOVA_IO_CANCELLED;
    return 0;
}

int nova_block_read_sectors(struct nova_device *dev, uint64_t sector, 
                            void *buf, uint32_t count) {
    struct nova_io_request *req;
    int result;
    
    if (dev == NULL || buf == NULL || count == 0) {
        return -1;
    }
    
    req = nova_io_request_alloc();
    if (req == NULL) {
        return -1;
    }
    
    req->type = NOVA_IO_READ;
    req->device = dev;
    req->offset = sector;
    req->buffer = buf;
    req->length = count * dev->block_ops->get_geometry ? 
                  count * 512 : count * 512;  /* Assume 512 byte sectors */
    
    result = nova_io_submit(req);
    if (result == 0) {
        result = nova_io_wait(req, req->timeout_ms);
        if (req->status == NOVA_IO_SUCCESS) {
            result = 0;
        } else {
            result = req->error_code;
        }
    }
    
    nova_io_request_free(req);
    return result;
}

int nova_block_write_sectors(struct nova_device *dev, uint64_t sector,
                             const void *buf, uint32_t count) {
    struct nova_io_request *req;
    int result;
    
    if (dev == NULL || buf == NULL || count == 0) {
        return -1;
    }
    
    req = nova_io_request_alloc();
    if (req == NULL) {
        return -1;
    }
    
    req->type = NOVA_IO_WRITE;
    req->device = dev;
    req->offset = sector;
    req->buffer = (void *)buf;
    req->length = count * 512;  /* Assume 512 byte sectors */
    
    result = nova_io_submit(req);
    if (result == 0) {
        result = nova_io_wait(req, req->timeout_ms);
        if (req->status == NOVA_IO_SUCCESS) {
            result = 0;
        } else {
            result = req->error_code;
        }
    }
    
    nova_io_request_free(req);
    return result;
}

/* Completion port implementation placeholders */
struct nova_io_completion_port *nova_io_completion_port_create(uint32_t max_events) {
    struct nova_io_completion_port *port;
    
    port = (struct nova_io_completion_port *)calloc(1, sizeof(struct nova_io_completion_port));
    if (port != NULL) {
        port->max_events = max_events;
        port->timeout_ms = INFINITE;
    }
    
    return port;
}

int nova_io_completion_port_destroy(struct nova_io_completion_port *port) {
    if (port != NULL) {
        free(port);
        return 0;
    }
    return -1;
}

int nova_io_completion_port_associate(struct nova_io_completion_port *port,
                                       struct nova_device *dev, uintptr_t key) {
    (void)port;
    (void)dev;
    (void)key;
    /* TODO: Implement device association */
    return 0;
}

int nova_io_completion_port_get_event(struct nova_io_completion_port *port,
                                       struct nova_io_request **req,
                                       uint32_t timeout_ms) {
    (void)port;
    (void)req;
    (void)timeout_ms;
    /* TODO: Implement event retrieval */
    return -1;
}
