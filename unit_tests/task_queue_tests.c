#include "../src/config.h"

#ifdef UNIT_TESTS
#include <stdlib.h>
#include <stm32l4xx.h>
#include "unit_tests.h"
#include "../src/tcb.h"
#include "../src/task_queue.h"
#include "../src/assert.h"


void test_queue_init()
{
	task_queue_t tq;
	tcb_t task_1;
	tcb_t task_2;
	tq.size = 100; tq.head = &task_1; tq.tail = &task_2;

	init_task_queue(&tq);

	ASSERT(tq.size == 0);

	ASSERT(tq.head == NULL);
	ASSERT(tq.tail == NULL);

}

void test_queue_push()
{
	task_queue_t tq;
	tcb_t task_1;
	tcb_t task_2;
	tcb_t task_3;

	task_1.next = &task_2;

	init_task_queue(&tq);

	task_push(&tq, &task_1);

	ASSERT(tq.head == &task_1);
	ASSERT(tq.tail == &task_1);
	ASSERT(tq.size == 1);
	ASSERT(task_1.next == NULL);
	ASSERT(task_1.prev == NULL);

	task_push(&tq, &task_2);

	ASSERT(tq.head == &task_1);
	ASSERT(tq.tail == &task_2);
	ASSERT(tq.size == 2);
	ASSERT(task_1.next == &task_2);
	ASSERT(task_1.prev == NULL);
	ASSERT(task_2.prev == &task_1);
	ASSERT(task_2.next == NULL);

	task_push(&tq, &task_3);

	ASSERT(tq.head == &task_1);
	ASSERT(tq.tail == &task_3);
	ASSERT(tq.size == 3);
	ASSERT(task_1.prev == NULL);
	ASSERT(task_1.next == &task_2);
	ASSERT(task_2.prev == &task_1);
	ASSERT(task_2.next == &task_3);
	ASSERT(task_3.prev == &task_2);
	ASSERT(task_3.next == NULL);
}

void test_queue_pop()
{
	task_queue_t tq;
	tcb_t task_1;
	tcb_t task_2;

	init_task_queue(&tq);

	task_push(&tq, &task_1);
	ASSERT(task_pop(&tq) == &task_1);

	ASSERT(tq.head == NULL);
	ASSERT(tq.tail == NULL);
	ASSERT(tq.size == 0);

	task_push(&tq, &task_1);
	task_push(&tq, &task_2);

	ASSERT(task_pop(&tq) == &task_1);
	ASSERT(tq.size == 1);
	ASSERT(task_pop(&tq) == &task_2);
	ASSERT(tq.size == 0);
	ASSERT(tq.head == NULL);
	ASSERT(tq.tail == NULL);
	ASSERT(task_pop(&tq) == NULL);
	ASSERT(tq.size == 0);
}

void test_queue_remove()
{
	task_queue_t tq;
	tcb_t task_1;
	tcb_t task_2;
	tcb_t task_3;

	init_task_queue(&tq);

	task_push(&tq, &task_1);
	task_push(&tq, &task_2);
	task_push(&tq, &task_3);

	task_remove(&tq, &task_2);

	ASSERT(tq.size == 2);
	ASSERT(tq.head == &task_1);
	ASSERT(tq.tail == &task_3);
	ASSERT(task_1.next == &task_3);
	ASSERT(task_1.prev == NULL);
	ASSERT(task_3.next == NULL);
	ASSERT(task_3.prev == &task_1);

	task_remove(&tq, &task_1);

	ASSERT(tq.size == 1);
	ASSERT(tq.head == &task_3);
	ASSERT(tq.tail == &task_3);
	ASSERT(task_3.next == NULL);
	ASSERT(task_3.prev == NULL);

	task_remove(&tq, &task_3);

	ASSERT(tq.size == 0);
	ASSERT(tq.head == NULL);
	ASSERT(tq.tail == NULL);
}

test_case_t tests[] =
{
		{"queue init", test_queue_init},
		{"queue push", test_queue_push},
		{"queue pop", test_queue_pop},
		{"queue remove", test_queue_remove}
};

// need to create an override on the ASSERT macro to make it better for testing
void task_queue_tests()
{
	int num_tests = sizeof(tests) / sizeof(tests[0]);

	for (int i = 0; i < num_tests; i++)
	{
		uint32_t prev_errors = num_errors;

		tests[i].fn();

		if (prev_errors != num_errors)
		{
			__BKPT(0);
		}
	}
}
#endif
