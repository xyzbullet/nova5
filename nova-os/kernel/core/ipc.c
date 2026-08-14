#include "include/nova_core.h"

#include <string.h>

void
nova_ipc_endpoint_init(nova_ipc_endpoint_t *endpoint)
{
	endpoint->head = 0;
	endpoint->tail = 0;
	endpoint->count = 0;
}

int
nova_ipc_send(nova_ipc_endpoint_t *endpoint, uint32_t sender,
    const void *payload, size_t length)
{
	nova_ipc_message_t *message;

	if (length > NOVA_IPC_PAYLOAD_SIZE)
		return (NOVA_ERR);
	if (endpoint->count == NOVA_IPC_QUEUE_LENGTH)
		return (NOVA_FULL);
	message = &endpoint->messages[endpoint->tail];
	message->sender = sender;
	message->length = (uint16_t)length;
	memcpy(message->payload, payload, length);
	endpoint->tail = (endpoint->tail + 1) % NOVA_IPC_QUEUE_LENGTH;
	endpoint->count++;
	return (NOVA_OK);
}

int
nova_ipc_receive(nova_ipc_endpoint_t *endpoint, nova_ipc_message_t *message)
{
	if (endpoint->count == 0)
		return (NOVA_EMPTY);
	*message = endpoint->messages[endpoint->head];
	endpoint->head = (endpoint->head + 1) % NOVA_IPC_QUEUE_LENGTH;
	endpoint->count--;
	return (NOVA_OK);
}