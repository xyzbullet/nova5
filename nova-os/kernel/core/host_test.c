#include "include/nova_core.h"

#include <assert.h>
#include <string.h>

static void
test_scheduler(void)
{
	nova_scheduler_t scheduler;
	uint32_t low_priority;
	uint32_t high_priority;

	nova_sched_init(&scheduler);
	assert(nova_sched_add(&scheduler, 10, &low_priority) == NOVA_OK);
	assert(nova_sched_add(&scheduler, 50, &high_priority) == NOVA_OK);
	assert(nova_sched_pick_next(&scheduler) == (int32_t)high_priority);
	nova_sched_account(&scheduler, high_priority);
	assert(nova_sched_set_state(&scheduler, high_priority,
	    NOVA_THREAD_BLOCKED) == NOVA_OK);
	assert(nova_sched_pick_next(&scheduler) == (int32_t)low_priority);
}

static void
test_ipc(void)
{
	const char payload[] = "hello from ipc";
	nova_ipc_endpoint_t endpoint;
	nova_ipc_message_t message;

	nova_ipc_endpoint_init(&endpoint);
	assert(nova_ipc_receive(&endpoint, &message) == NOVA_EMPTY);
	assert(nova_ipc_send(&endpoint, 7, payload, sizeof(payload)) == NOVA_OK);
	assert(nova_ipc_receive(&endpoint, &message) == NOVA_OK);
	assert(message.sender == 7);
	assert(message.length == sizeof(payload));
	assert(memcmp(message.payload, payload, sizeof(payload)) == 0);
}

static void
test_vm(void)
{
	nova_page_allocator_t allocator;
	nova_address_space_t space;
	uint32_t physical_page;
	uint32_t translated;
	uint8_t writable;

	nova_vm_init(&allocator);
	assert(nova_vm_alloc_page(&allocator, &physical_page) == NOVA_OK);
	nova_vm_address_space_init(&space);
	assert(nova_vm_map(&space, 0x1000, physical_page, 1) == NOVA_OK);
	assert(nova_vm_translate(&space, 0x1000, &translated, &writable) == NOVA_OK);
	assert(translated == physical_page);
	assert(writable == 1);
	assert(nova_vm_free_page(&allocator, physical_page) == NOVA_OK);
}

int
main(void)
{
	test_scheduler();
	test_ipc();
	test_vm();
	return (0);
}