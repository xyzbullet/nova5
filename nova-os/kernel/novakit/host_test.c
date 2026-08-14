#include "include/novakit.h"

#include <assert.h>
#include <string.h>

int
main(void)
{
	const novakit_device_t *device;
	novakit_registry_t registry;
	uint32_t device_id;

	novakit_registry_init(&registry);
	assert(novakit_register_device(&registry, 0x1AF4, 0x1000,
	    NOVAKIT_NETWORK, "virtio-net", &device_id) == 0);
	assert(novakit_set_state(&registry, device_id, NOVAKIT_ATTACHED) == 0);
	device = novakit_find(&registry, device_id);
	assert(device != NULL);
	assert(device->state == NOVAKIT_ATTACHED);
	assert(device->class == NOVAKIT_NETWORK);
	assert(strcmp(device->name, "virtio-net") == 0);
	return (0);
}