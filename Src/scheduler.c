/**
 *
 * In the future consider turning this into a state machine
 *
 */
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
	init_task_queue(&(sch_inst.delayed_list));

	return true;
}

// only function to set task->state = READY
void task_add_ready(tcb_t *task)
{
	ASSERT(task != NULL);
	ASSERT(task->priority < sch_inst.num_priorities);

	task_push(&(sch_inst.ready_lists[task->priority]), task);

	task->state = READY;

	sch_inst.ready_bitmap |= (1U << task->priority); // task is ready when added
}

tcb_t *task_pop_ready()
{
	if (sch_inst.ready_bitmap == 0)
	{
		return NULL;
	}

	uint32_t priority = 31 - __builtin_clz(sch_inst.ready_bitmap);

	ASSERT(priority < sch_inst.num_priorities);

	tcb_t *task = task_pop(&(sch_inst.ready_lists[priority]));

	if (sch_inst.ready_lists[priority].size == 0)
	{
		sch_inst.ready_bitmap &= ~(1U << priority); // remove from ready list
	}

	return task;
}

// only function to set task->state = BLOCKED
// assuming you can only call task_block on a ready task
void task_block(tcb_t *task)
{
	ASSERT(task != NULL);
	ASSERT(task->state == READY);

	task_remove(&(sch_inst.ready_lists[task->priority]), task);

	if (sch_inst.ready_lists[task->priority].size == 0)
	{
		sch_inst.ready_bitmap &= ~(1U << task->priority);
	}

	task->state = BLOCKED;

	task_push(&(sch_inst.blocked_list), task);
}

void task_unblock(tcb_t *task)
{
	ASSERT(task != NULL);
	ASSERT(task->state == BLOCKED);

	task_remove(&(sch_inst.blocked_list), task);

	task_add_ready(task); // sets state = ready
}

// assumes task is ready when called
// only function to set state = DELAYED
void task_delay(tcb_t *task)
{
	ASSERT(task != NULL);
	ASSERT(task->state == READY);

	task_remove(&(sch_inst.ready_lists[task->priority]), task);

	if (sch_inst.ready_lists[task->priority].size == 0)
	{
		sch_inst.ready_bitmap &= ~(1U << task->priority);
	}

	task->state = DELAYED;

	task_push(&(sch_inst.delayed_list), task);
}

void task_wake(tcb_t *task)
{
	ASSERT(task != NULL);
	ASSERT(task->state == DELAYED);

	task_remove(&(sch_inst.delayed_list), task);

	task_add_ready(task);
}

#ifdef DEBUG
tcb_t* get_cur_task()
{
	return sch_inst.cur_task;
}
#endif

void scheduler_tick()
{
	task_add_ready(sch_inst.cur_task); // put the current running task back to ready

	tcb_t *task = task_pop_ready(); // put the highest priority ready task into cur_task

	sch_inst.cur_task = task;

	task->state = RUNNING;

	next_process = task; // the thing the scheduler pulls in to switch to

	sch_ticks++;
}

