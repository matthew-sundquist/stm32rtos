/*
 * ready_list.h
 *
 *  Created on: May 24, 2026
 *      Author: Matthew Sundquist
 */
#ifndef READY_LIST_H_
#define READY_LIST_H_

#include "tcb.h"
#include "stdint.h"

typedef struct ready_list
{
	tcb_t *head;
	tcb_t *tail;
	uint8_t size;
} ready_list_t;

uint8_t init_ready_list(ready_list_t *rl); // init struct
uint8_t push_task(ready_list_t *rl, tcb_t *task);
uint8_t pop_task(ready_list_t *rl); // pops from task
uint8_t shift_list(ready_list_t *rl);



#endif /* READY_LIST_H_ */
