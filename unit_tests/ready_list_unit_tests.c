
#include <stdlib.h>
#include <stm32l4xx.h>
#include "unit_tests.h"
#include "../src/tcb.h"
#include "../src/ready_list.h"


void ready_lists_tests()
{
	ready_list_t rl;

	init_ready_list(&rl);

	if (rl.head != NULL || rl.tail != NULL || rl.size != 0)
	{
		__BKPT(0);
	}
	tcb_t t1;

	tcb_t t2;

	tcb_t t3;

	if (push_task(&rl, &t1) != 0)
	{
		__BKPT(0);
	}

	if (rl.head != &t1 || rl.tail != &t1 || rl.size != 1)
	{
		__BKPT(0);
	}


	if (pop_task(&rl) != 0)
	{
		__BKPT(0);
	}

	if (pop_task(&rl) != 2)
	{
		__BKPT(0);
	}

	if (push_task(&rl, &t1) != 0 || push_task(&rl, &t2) != 0 || push_task(&rl, &t3) != 0)
	{
		__BKPT(0);
	}

	if (rl.size != 3 || rl.head != &t1 || rl.tail != &t3 || rl.head->next != &t2)
	{
		__BKPT(0);
	}
}
