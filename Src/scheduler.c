
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include "scheduler.h"
#include "assert.h"

static scheduler_t sch_inst;

tcb_t* cur_process = NULL;
tcb_t* next_process = NULL;
uint64_t sch_ticks = 0;


bool init_scheduler(uint8_t num_priorities)
{

	if (num_priorities > MAX_PRIORITIES || num_priorities < 1)
	{
		return false;
	}

	sch_inst.num_priorities = num_priorities;
	sch_inst.ready_bitmap = 0;
	sch_inst.cur_task = NULL;

	for (int i = 0; i < num_priorities; i++)
	{
		init_task_queue(&(sch_inst.ready_lists[i]));
	}

	init_task_queue(&(sch_inst.blocked_list));

	return true;
}

void task_add_ready(tcb_t *task)
{
	ASSERT(task != NULL);
	ASSERT(task->priority < sch_inst.num_priorties);

	push_task(&(sch_inst.ready_lists[task->priority]), task);

	task->state = READY;

	sch_inst.ready_bitmap |= (1U << task->priority); // task is assumed to be ready when added

}

tcb_t *task_pop_ready()
{
	if (sch_inst.ready_bitmap == 0)
	{
		return NULL;
	}

	uint32_t priority = 31 - __builtin_clz(sch_inst.ready_bitmap);

	ASSERT(priority < sch_inst.num_priorities);

	tcb_t *task = pop_task(&(sch_inst.ready_lists[priority]));

	if (sch_inst.ready_lists[priority].size == 0)
	{
		sch_inst.ready_bitmap &= ~(1U << priority); // remove from ready list
	}

	return task;
}

void task_block(tcb_t *task)
{
	ASSERT(task != NULL);

	push_task(&(sch_inst.blocked_list), task);

	task->state = BLOCKED;
}

tcb_t *task_remove_blocked()
{
	return pop_task(&(sch_inst.blocked_list));
}

#ifdef DEBUG
tcb_t* get_cur_task()
{
	return sch_inst.cur_task;
}
#endif

void scheduler_tick()
{
	add_task_to_ready(sch_inst.cur_task); // put the current running task back to ready

	uint8_t state = select_task(); // put the highest priority ready task into cur_task
	if (state == 1)
	{
		// no tasks ready : do not switch task
	}
	else if (state == 2)
	{
		// weird state : somehow recover?
	}
	else
	{
		next_process = sch_inst.cur_task; // set next process
	}

	sch_ticks++;
}

