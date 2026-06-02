#include <stdlib.h>
#include <string.h>
#include "task_queue.h"
#include "assert.h"

void init_task_queue(task_queue_t *tq)
{

	ASSERT(tq != NULL);

	tq->head = NULL;
	tq->tail = NULL;
	tq->size = 0;

}

// push to tail
void push_task(task_queue_t *tq, tcb_t *task)
{
	ASSERT(tq != NULL);
	ASSERT(task != NULL);

	if (tq->size == 0)
	{
		tq->head = task;
		tq->tail = task;
	}
	else if (tq->size == 1)
	{
		tq->head->next = task;
		tq->tail = task;
	}
	else
	{
		tq->tail->next = task;
		tq->tail = task;
	}
	tq->size++;
}

// pop from head
tcb_t *pop_task(task_queue_t *tq)
{
	ASSERT(tq != NULL);

	ASSERT(tq->size > 0);

	tcb_t *popped_task;
	if (tq->size == 1)
	{
		popped_task = tq->head;
		tq->head = NULL;
		tq->tail = NULL;
	}
	else
	{
		popped_task = tq->head;

		tq->head = tq->head->next;
		popped_task->next = NULL;
	}

	tq->size--;

	return popped_task;
}
