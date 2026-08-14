#include "include/nova_core.h"

void
nova_vm_init(nova_page_allocator_t *allocator)
{
	size_t index;

	for (index = 0; index < NOVA_VM_MAX_PAGES; index++)
		allocator->allocated[index] = 0;
}

int
nova_vm_alloc_page(nova_page_allocator_t *allocator, uint32_t *physical_page)
{
	size_t index;

	for (index = 0; index < NOVA_VM_MAX_PAGES; index++) {
		if (allocator->allocated[index] != 0)
			continue;
		allocator->allocated[index] = 1;
		*physical_page = (uint32_t)index;
		return (NOVA_OK);
	}
	return (NOVA_FULL);
}

int
nova_vm_free_page(nova_page_allocator_t *allocator, uint32_t physical_page)
{
	if (physical_page >= NOVA_VM_MAX_PAGES ||
	    allocator->allocated[physical_page] == 0)
		return (NOVA_NOT_FOUND);
	allocator->allocated[physical_page] = 0;
	return (NOVA_OK);
}

void
nova_vm_address_space_init(nova_address_space_t *space)
{
	space->count = 0;
}

int
nova_vm_map(nova_address_space_t *space, uint64_t virtual_page,
    uint32_t physical_page, uint8_t writable)
{
	size_t index;

	for (index = 0; index < space->count; index++) {
		if (space->mappings[index].virtual_page != virtual_page)
			continue;
		space->mappings[index].physical_page = physical_page;
		space->mappings[index].writable = writable;
		return (NOVA_OK);
	}
	if (space->count == NOVA_MAX_MAPPINGS)
		return (NOVA_FULL);
	space->mappings[space->count].virtual_page = virtual_page;
	space->mappings[space->count].physical_page = physical_page;
	space->mappings[space->count].writable = writable;
	space->count++;
	return (NOVA_OK);
}

int
nova_vm_translate(const nova_address_space_t *space, uint64_t virtual_page,
    uint32_t *physical_page, uint8_t *writable)
{
	size_t index;

	for (index = 0; index < space->count; index++) {
		if (space->mappings[index].virtual_page != virtual_page)
			continue;
		*physical_page = space->mappings[index].physical_page;
		*writable = space->mappings[index].writable;
		return (NOVA_OK);
	}
	return (NOVA_NOT_FOUND);
}