
#include "mutex.h"
#include "scheduler.h"

void mutex_aquire(mutex_t *mut)
{
	/*
	 * Algorithm psuedocode:
	 * check if mutex is currently being used
	 * if true: add into blocked list
	 * else: set pid
	 */

	// cur task's pid is always valid
	do {
		uint32_t result = __LDREXW(&(mut->owner_pid));

		if (result == 0) // is free
		{
			if (__STREXW(get_running_pid(), &(mut->owner_pid)) == 1)
			{
				continue; // failed
			}

			return; // success case
		}

		else // even if this is our pid, we do not care, correct usage is not to try to aquire the mutex after you already have it
		{
			// this must add to this mutex's delayed list

			tcb_t *cur_task = get_cur_task();

			__disable_irq();

			cur_task->state = DELAYED;

			task_push(&(mut->delayed_tasks), cur_task);

			__enable_irq();

			scheduler_tick();

			return;
		}
	} while (1);
}

void mutex_release(mutex_t *mut)
{

}
