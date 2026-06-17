
#include <stm32l4xx.h>
#include "scheduler.h"
#include "semaphore.h"
#include "assert.h"

void sem_init(semaphore_t *sem, uint32_t max_count, uint32_t init_count)
{

	ASSERT(init_count <= max_count);
	init_task_queue(&(sem->blocked_tasks));

	sem->max_count = max_count;
	sem->count = init_count;
}

void sem_aquire(semaphore_t *sem)
{
	do {
			uint32_t result = __LDREXW(&(sem->count));

			if (result > 0) // is free
			{
				if (__STREXW(--result, &(sem->count)) == 1)
				{
					continue; // failed
				}

				return; // success case
			}

			else // sem count was 0, cannot aquire, must wait
			{
				__CLREX();

				__disable_irq();

				tcb_t *cur_task = get_cur_task(); // we are the current task


				task_block(cur_task, BLOCKED);

				task_push(&(sem->blocked_tasks), cur_task);

				__enable_irq();

				scheduler_tick();

			}
	} while (1);
}

void sem_release(semaphore_t *sem)
{
	__disable_irq();

	if (sem->blocked_tasks.size == 0)
	{
		if (sem->count < sem->max_count)
		{
			sem->count++;
		}
	}
	else
	{
		tcb_t *task = task_pop(&(sem->blocked_tasks));
		task_add_ready(task);
	}

	__enable_irq();
}
