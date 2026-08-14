#ifndef NOVAKIT_H
#define NOVAKIT_H

#include <stddef.h>
#include <stdint.h>

#define NOVAKIT_MAX_DEVICES 64u
#define NOVAKIT_NAME_LENGTH 48u

typedef enum {
	NOVAKIT_STORAGE = 0,
	NOVAKIT_NETWORK,
	NOVAKIT_GPU,
	NOVAKIT_INPUT,
	NOVAKIT_AUDIO,
	NOVAKIT_BUS
} novakit_device_class_t;

typedef enum {
	NOVAKIT_DETACHED = 0,
	NOVAKIT_ATTACHED
} novakit_device_state_t;

typedef struct {
	uint32_t id;
	uint16_t vendor_id;
	uint16_t device_id;
	novakit_device_class_t class;
	novakit_device_state_t state;
	char name[NOVAKIT_NAME_LENGTH];
} novakit_device_t;

typedef struct {
	novakit_device_t devices[NOVAKIT_MAX_DEVICES];
	size_t count;
} novakit_registry_t;

void novakit_registry_init(novakit_registry_t *registry);
int novakit_register_device(novakit_registry_t *registry, uint16_t vendor_id,
    uint16_t device_id, novakit_device_class_t class, const char *name,
    uint32_t *device_id_out);
int novakit_set_state(novakit_registry_t *registry, uint32_t device_id,
    novakit_device_state_t state);
const novakit_device_t *novakit_find(const novakit_registry_t *registry,
    uint32_t device_id);

#endif