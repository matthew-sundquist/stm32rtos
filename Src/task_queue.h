/*
 * task_list.h
 *
 *  Created on: May 24, 2026
 *      Author: Matthew Sundquist
 */
#ifndef TASK_LIST_H_
#define TASK_LIST_H_

#include "tcb.h"
#include "stdint.h"

typedef struct task_queue
{
	tcb_t *head;
	tcb_t *tail;
	uint8_t size;
} task_queue_t;

void init_task_queue(task_queue_t *tq); // init struct
void push_task(task_queue_t *tq, tcb_t *task);
tcb_t *pop_task(task_queue_t *tq); // pops from task


#endif /* TASK_LIST_H_ */
