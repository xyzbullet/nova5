#include "include/novakit.h"

#include <string.h>

void
novakit_registry_init(novakit_registry_t *registry)
{
	memset(registry, 0, sizeof(*registry));
}

int
novakit_register_device(novakit_registry_t *registry, uint16_t vendor_id,
    uint16_t device_id, novakit_device_class_t class, const char *name,
    uint32_t *device_id_out)
{
	novakit_device_t *device;

	if (registry->count == NOVAKIT_MAX_DEVICES)
		return (-1);
	device = &registry->devices[registry->count];
	device->id = (uint32_t)registry->count + 1;
	device->vendor_id = vendor_id;
	device->device_id = device_id;
	device->class = class;
	device->state = NOVAKIT_DETACHED;
	strncpy(device->name, name, NOVAKIT_NAME_LENGTH - 1);
	device->name[NOVAKIT_NAME_LENGTH - 1] = '\0';
	registry->count++;
	*device_id_out = device->id;
	return (0);
}

int
novakit_set_state(novakit_registry_t *registry, uint32_t device_id,
    novakit_device_state_t state)
{
	novakit_device_t *device;

	if (device_id == 0 || device_id > registry->count)
		return (-1);
	device = &registry->devices[device_id - 1];
	device->state = state;
	return (0);
}

const novakit_device_t *
novakit_find(const novakit_registry_t *registry, uint32_t device_id)
{
	if (device_id == 0 || device_id > registry->count)
		return (NULL);
	return (&registry->devices[device_id - 1]);
}