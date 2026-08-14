#ifndef NOVA_CORE_H
#define NOVA_CORE_H

#include <stddef.h>
#include <stdint.h>

#define NOVA_MAX_THREADS 32u
#define NOVA_IPC_QUEUE_LENGTH 16u
#define NOVA_IPC_PAYLOAD_SIZE 64u
#define NOVA_VM_MAX_PAGES 128u
#define NOVA_PAGE_SIZE 4096u
#define NOVA_MAX_MAPPINGS 128u

enum nova_result {
	NOVA_OK = 0,
	NOVA_ERR = -1,
	NOVA_FULL = -2,
	NOVA_EMPTY = -3,
	NOVA_NOT_FOUND = -4
};

enum nova_thread_state {
	NOVA_THREAD_DEAD = 0,
	NOVA_THREAD_RUNNABLE,
	NOVA_THREAD_BLOCKED
};

typedef struct {
	uint32_t id;
	uint8_t priority;
	uint64_t quanta;
	enum nova_thread_state state;
} nova_thread_t;

typedef struct {
	nova_thread_t threads[NOVA_MAX_THREADS];
	size_t count;
	uint64_t tick;
} nova_scheduler_t;

void nova_sched_init(nova_scheduler_t *scheduler);
int nova_sched_add(nova_scheduler_t *scheduler, uint8_t priority, uint32_t *id);
int nova_sched_set_state(nova_scheduler_t *scheduler, uint32_t id,
    enum nova_thread_state state);
int32_t nova_sched_pick_next(const nova_scheduler_t *scheduler);
void nova_sched_account(nova_scheduler_t *scheduler, uint32_t id);

typedef struct {
	uint32_t sender;
	uint16_t length;
	uint8_t payload[NOVA_IPC_PAYLOAD_SIZE];
} nova_ipc_message_t;

typedef struct {
	nova_ipc_message_t messages[NOVA_IPC_QUEUE_LENGTH];
	size_t head;
	size_t tail;
	size_t count;
} nova_ipc_endpoint_t;

void nova_ipc_endpoint_init(nova_ipc_endpoint_t *endpoint);
int nova_ipc_send(nova_ipc_endpoint_t *endpoint, uint32_t sender,
    const void *payload, size_t length);
int nova_ipc_receive(nova_ipc_endpoint_t *endpoint, nova_ipc_message_t *message);

typedef struct {
	uint8_t allocated[NOVA_VM_MAX_PAGES];
} nova_page_allocator_t;

typedef struct {
	uint64_t virtual_page;
	uint32_t physical_page;
	uint8_t writable;
} nova_vm_mapping_t;

typedef struct {
	nova_vm_mapping_t mappings[NOVA_MAX_MAPPINGS];
	size_t count;
} nova_address_space_t;

void nova_vm_init(nova_page_allocator_t *allocator);
int nova_vm_alloc_page(nova_page_allocator_t *allocator, uint32_t *physical_page);
int nova_vm_free_page(nova_page_allocator_t *allocator, uint32_t physical_page);
void nova_vm_address_space_init(nova_address_space_t *space);
int nova_vm_map(nova_address_space_t *space, uint64_t virtual_page,
    uint32_t physical_page, uint8_t writable);
int nova_vm_translate(const nova_address_space_t *space, uint64_t virtual_page,
    uint32_t *physical_page, uint8_t *writable);

#endif