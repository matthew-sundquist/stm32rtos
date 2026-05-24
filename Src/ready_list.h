/*
 * ready_list.h
 *
 *  Created on: May 24, 2026
 *      Author: Matthew Sundquist
 */

#ifndef READY_LIST_H_
#define READY_LIST_H_

typedef struct ready_list
{
	tcb_t *head;
	tcb_t *tail;
	uint8_t len;
} ready_list_t;

int init_ready_list(ready_list *rl);
int add_task(ready_list *rl, tcb_t *task);
int shift_list(ready_list *rl);



#endif /* READY_LIST_H_ */
