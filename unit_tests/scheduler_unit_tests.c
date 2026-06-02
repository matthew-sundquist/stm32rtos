#ifdef UNIT_TESTS
#include <stdlib.h>
#include <stm32l4xx.h>
#include <stdbool.h>
#include "unit_tests.h"
#include "../src/tcb.h"
#include "../src/scheduler.h"
#include "../src/task_queue.h"

void scheduler_tests()
{
	tcb_t t1;
	tcb_t t2;
	tcb_t t3;

	t1.priority = 1;
	t2.priority = 1;
	t3.priority = 1;

	if (init_scheduler(0) != true || init_scheduler(-1) != false) // -1 overflow to highest allowed priority
	{
		__BKPT(0);
	}

	if (init_scheduler(5) != true)
	{
		__BKPT(0);
	}

	if (get_cur_task() != NULL)
	{
		__BKPT(0);
	}

	task_add_ready(NULL); // not valid anymore

	task_add_ready(&t1);


	if (select_task() != 0)
	{
		__BKPT(0);
	}

	if (get_cur_task() != &t1)
	{
		__BKPT(0);
	}

	if (task_remove_from_ready(1) != 3)
	{
		__BKPT(0);
	}

	if (task_add_ready(&t2) != 0 || task_add_ready(&t3) != 0)
	{
		__BKPT(0);
	}

	if (task_remove_from_ready(1) != 0)
	{
		__BKPT(0);
	}

	if (task_remove_from_ready(1) != 0)
	{
		__BKPT(0);
	}

	if (task_remove_from_ready(1) != 3)
	{
		__BKPT(0);
	}

	task_add_ready(&t2);

	if (task_remove_from_ready(3) != 3)
	{
		__BKPT(0);
	}

	tcb_t t4;

	t4.priority = 10;

	if (task_add_ready(&t4) != 2)
	{
		__BKPT(0);
	}

	t4.priority = 4;

	if (task_add_ready(&t4) != 0)
	{
		__BKPT(0);
	}

	tcb_t t5;

	t5.priority = 3;

	task_add_ready(&t5);

	if (select_task() != 0)
	{
		__BKPT(0);
	}


	if (get_cur_task() != &t4)
	{
		__BKPT(0);
	}

}
#endif
