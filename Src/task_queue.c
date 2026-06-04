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
void task_push(task_queue_t *tq, tcb_t *task)
{
	ASSERT(tq != NULL);
	ASSERT(task != NULL);

	task->next = NULL;
	task->prev = NULL;

	if (tq->size == 0)
	{
		tq->head = task;
		tq->tail = task;
	}
	else
	{
		tq->tail->next = task;
		task->prev = tq->tail;
		tq->tail = task;

	}
	tq->size++;
}

// pop from head
tcb_t *task_pop(task_queue_t *tq)
{
	ASSERT(tq != NULL);

	if (tq->size <= 0 && tq->head == NULL)
	{
		return NULL;
	}

	tcb_t *task = tq->head;

	tq->head = tq->head->next;

	if (tq->head != NULL)
	{
		tq->head->prev = NULL;
	}
	else
	{
		tq->tail = NULL;
	}
	tq->size--;

	task->next = NULL;
	task->prev = NULL;

	return task;
}


void task_remove(task_queue_t *tq, tcb_t *task)
{
	ASSERT(tq != NULL);
	ASSERT(task != NULL);
	ASSERT(tq->size > 0);

	if (tq->head == task)
	{
		tq->head = task->next;
	}

	if (tq->tail == task)
	{
		tq->tail = task->prev;
	}

	if (task->prev != NULL)
	{
		task->prev->next = task->next;
	}

	if (task->next != NULL)
	{
		task->next->prev = task->prev;
	}

	tq->size--;

	task->next = NULL;
	task->prev = NULL;
}

