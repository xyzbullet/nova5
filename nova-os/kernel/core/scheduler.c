#include "include/nova_core.h"

void
nova_sched_init(nova_scheduler_t *scheduler)
{
	size_t index;

	scheduler->count = 0;
	scheduler->tick = 0;
	for (index = 0; index < NOVA_MAX_THREADS; index++)
		scheduler->threads[index].state = NOVA_THREAD_DEAD;
}

int
nova_sched_add(nova_scheduler_t *scheduler, uint8_t priority, uint32_t *id)
{
	size_t index;

	for (index = 0; index < NOVA_MAX_THREADS; index++) {
		if (scheduler->threads[index].state != NOVA_THREAD_DEAD)
			continue;
		scheduler->threads[index].id = (uint32_t)index + 1;
		scheduler->threads[index].priority = priority;
		scheduler->threads[index].quanta = 0;
		scheduler->threads[index].state = NOVA_THREAD_RUNNABLE;
		scheduler->count++;
		*id = scheduler->threads[index].id;
		return (NOVA_OK);
	}
	return (NOVA_FULL);
}

int
nova_sched_set_state(nova_scheduler_t *scheduler, uint32_t id,
    enum nova_thread_state state)
{
	size_t index;

	if (id == 0 || id > NOVA_MAX_THREADS)
		return (NOVA_NOT_FOUND);
	index = (size_t)id - 1;
	if (scheduler->threads[index].state == NOVA_THREAD_DEAD)
		return (NOVA_NOT_FOUND);
	scheduler->threads[index].state = state;
	return (NOVA_OK);
}

int32_t
nova_sched_pick_next(const nova_scheduler_t *scheduler)
{
	int32_t selected = -1;
	size_t index;

	for (index = 0; index < NOVA_MAX_THREADS; index++) {
		const nova_thread_t *candidate = &scheduler->threads[index];
		const nova_thread_t *current;

		if (candidate->state != NOVA_THREAD_RUNNABLE)
			continue;
		if (selected < 0) {
			selected = (int32_t)index;
			continue;
		}
		current = &scheduler->threads[(size_t)selected];
		if (candidate->priority > current->priority ||
		    (candidate->priority == current->priority &&
		    candidate->quanta < current->quanta))
			selected = (int32_t)index;
	}
	return (selected < 0 ? -1 : (int32_t)scheduler->threads[(size_t)selected].id);
}

void
nova_sched_account(nova_scheduler_t *scheduler, uint32_t id)
{
	if (id == 0 || id > NOVA_MAX_THREADS)
		return;
	if (scheduler->threads[(size_t)id - 1].state != NOVA_THREAD_DEAD) {
		scheduler->threads[(size_t)id - 1].quanta++;
		scheduler->tick++;
	}
}