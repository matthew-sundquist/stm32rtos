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
#include <stdio.h>

static scheduler_t sch_inst;

tcb_t* cur_process = NULL;
tcb_t* next_process = NULL;
uint32_t sch_ticks = 0;

void init_scheduler()
{

	sch_inst.ready_bitmap = 0;
	sch_inst.cur_task = NULL;

	for (int i = 0; i < MAX_PRIORITIES; i++)
	{
		init_task_queue(&(sch_inst.ready_lists[i]));
	}

	init_task_queue(&(sch_inst.blocked_list));
	init_task_queue(&(sch_inst.delayed_list));

}

// only function to set task->state = READY
void task_add_ready(tcb_t *task)
{
	ASSERT(task != NULL);
	ASSERT(task->priority < MAX_PRIORITIES);

	__disable_irq();
	task_push(&(sch_inst.ready_lists[task->priority]), task);

	task->state = READY;

	sch_inst.ready_bitmap |= (1U << task->priority); // task is ready when added

	__enable_irq();
}

tcb_t *task_pop_ready()
{
	if (sch_inst.ready_bitmap == 0)
	{
		return NULL;
	}

	uint32_t priority = 31 - __builtin_clz(sch_inst.ready_bitmap);

	ASSERT(priority < MAX_PRIORITIES);

	__disable_irq();
	tcb_t *task = task_pop(&(sch_inst.ready_lists[priority]));

	if (sch_inst.ready_lists[priority].size == 0)
	{
		sch_inst.ready_bitmap &= ~(1U << priority); // remove from ready list
	}
	__enable_irq();

	return task;
}


// changes task state to state, removes from ready list if it contains it
void task_block(tcb_t *task, task_state_t state)
{
	ASSERT(task != NULL);

	__disable_irq();
	if (task->state == READY)
	{
		task_remove(&(sch_inst.ready_lists[task->priority]), task);

		if (sch_inst.ready_lists[task->priority].size == 0)
		{
			sch_inst.ready_bitmap &= ~(1U << task->priority);
		}
	}

	task->state = state;
	__enable_irq();

}

// moves task from blocked state to the ready list
void task_unblock(tcb_t *task)
{
	ASSERT(task != NULL);

	task_add_ready(task); // sets state = ready
}

void task_sleep(uint32_t ticks_asleep)
{

	ASSERT(sch_inst.cur_task != NULL);

	__disable_irq();

	sch_inst.cur_task->wake_tick = sch_ticks + ticks_asleep;

	ASSERT(sch_inst.cur_task->state == RUNNING);

	task_block(sch_inst.cur_task, DELAYED);

	task_push(&(sch_inst.delayed_list), sch_inst.cur_task);

	__enable_irq();

	scheduler_tick();
}

void scheduler_handle_blocked()
{
	__disable_irq();

	tcb_t *walker = sch_inst.delayed_list.head;
	tcb_t *next = NULL;

	while (walker != NULL)
	{
		next = walker->next;

		if (walker->wake_tick <= sch_ticks)
		{
			task_remove(&(sch_inst.delayed_list), walker);

			task_unblock(walker);

		}

		walker = next;
	}

	__enable_irq();
}


#ifdef DEBUG

scheduler_t *get_scheduler()
{
	return &sch_inst;
}

uint32_t get_ticks()
{
	return sch_ticks;
}
#endif

uint32_t get_running_pid()
{
	if (sch_inst.cur_task == NULL)
	{
		return 0;
	}

	return sch_inst.cur_task->pid;
}

tcb_t* get_cur_task()
{
	return sch_inst.cur_task;
}

void scheduler_tick()
{

	scheduler_handle_blocked();

	/*
	 * If task is not in the running state, it has been either been put to sleep, or is waiting on a semaphore/mutex,
	 * in which case, it has already been placed in the respective list
	 * Therefor if it is running, we can assume that it is still in the READY state,
	 * meaning it can be put back into the ready list
	 */

	__disable_irq();

	if (sch_inst.cur_task->state == RUNNING)
	{
		task_add_ready(sch_inst.cur_task);
	}

	sch_inst.cur_task = task_pop_ready();

	if (sch_inst.cur_task == NULL)
	{
		ASSERT(0); // put into breakpoint
	}

	next_process = sch_inst.cur_task;

	sch_inst.cur_task->state = RUNNING;

	sch_ticks++;

	__enable_irq();

	SCB->ICSR = SCB_ICSR_PENDSVSET_Msk; // deferred PendSV interrupt
}

