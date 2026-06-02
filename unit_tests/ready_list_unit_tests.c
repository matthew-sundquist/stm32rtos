#ifdef UNIT_TESTS
#include <stdlib.h>
#include <stm32l4xx.h>
#include "unit_tests.h"
#include "../src/tcb.h"
#include "../src/task_queue.h"


// need to create an override on the ASSERT macro to make it better for testing
void ready_lists_tests()
{
	task_list_t rl;

	init_task_queue(&rl);

	if (rl.head != NULL || rl.tail != NULL || rl.size != 0)
	{
		__BKPT(0);
	}
	tcb_t t1;

	tcb_t t2;

	tcb_t t3;

	if (task_push(&rl, &t1) != 0)
	{
		__BKPT(0);
	}

	if (rl.head != &t1 || rl.tail != &t1 || rl.size != 1)
	{
		__BKPT(0);
	}


	if (task_pop(&rl) != 0)
	{
		__BKPT(0);
	}

	if (task_pop(&rl) != 2)
	{
		__BKPT(0);
	}

	if (task_push(&rl, &t1) != 0 || task_push(&rl, &t2) != 0 || task_push(&rl, &t3) != 0)
	{
		__BKPT(0);
	}

	if (rl.size != 3 || rl.head != &t1 || rl.tail != &t3 || rl.head->next != &t2)
	{
		__BKPT(0);
	}
}
#endif
