#include "ready_list.h"
#include <stdlib.h>
#include <string.h>

uint8_t init_ready_list(ready_list_t *rl)
{
	if (rl == NULL)
	{
		return 1;
	}

	rl->head = NULL;
	rl->tail = NULL;
	rl->size = 0;

	return 0;
}


uint8_t push_task(ready_list_t *rl, tcb_t *task)
{
	if (rl == NULL || task == NULL)
	{
		return 1;
	}

	if (rl->size == 0)
	{
		rl->head = task;
		rl->tail = task;
	}
	else if (rl->size == 1)
	{
		rl->head->next = task;
		rl->tail = task;
	}
	else
	{
		rl->tail->next = task;
		rl->tail = task;
	}
	rl->size++;

	return 0;
}

uint8_t pop_task(ready_list_t *rl)
{
	if (rl == NULL)
	{
		return 1;
	}

	if (rl->size <= 0)
	{
		return 2;
	}

	if (rl->size == 1)
	{
		rl->head == NULL;
		rl->tail == NULL;
	}
	else
	{
		tcb_t *old_head = rl->head;

		rl->head = rl->head->next;
		old_head->next = NULL;
	}

	rl->size--;
	return 0;
}

uint8_t shift_list(ready_list_t *rl)
{
	if (rl == NULL)
	{
		return 1;
	}

	if (rl->size <= 1)
	{
		return 0;
	}
	else
	{
		tcb_t *old_head = rl->head;
		rl->head = rl->head->next;

		rl->tail->next = old_head;
		old_head->next = NULL;
		rl->tail = old_head;
	}

	return 0;
}
