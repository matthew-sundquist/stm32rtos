/*
 * scheduler.h
 *
 *  Created on: May 27, 2026
 *      Author: Matthew Sundquist
 */

#ifndef SCHEDULER_H_
#define SCHEDULER_H_


#include <stdint.h>
#include <stdbool.h>
#include "task_queue.h"
#include "tcb.h"

#define MAX_PRIORITIES 15

extern tcb_t* cur_process;
extern tcb_t* next_process;

extern uint64_t num_scheduler_ticks;

typedef struct scheduler {
	tcb_t *cur_task;
	task_queue_t ready_lists[MAX_PRIORITIES];
	task_queue_t blocked_list;
	task_queue_t delayed_list;
	uint8_t num_priorities;
	uint32_t ready_bitmap;
} scheduler_t;

//void scheduler_init(uint8_t num_priorities); -> good with what I have
//
//void task_add_ready(tcb_t *task); -> move task into runnable state
//tcb_t* task_pop_ready(void); -> find highest priority task and return it
//
//void task_block(tcb_t *task); -> insert into blocked list and clear bitmaks if nessisary
//void task_unblock(tcb_t *task); -> move task back into runnable state, from blocked list to ready list
//
//void task_delay(tcb_t *task, uint32_t ticks); -> set task into delayed list, prob not blocked list
//
//void scheduler_run(void); -> maybe is just the pendsv handler, may already be done
//void scheduler_tick_handler(void); -> runs every systick interrupt


void task_add_ready(tcb_t *task);
tcb_t *task_pop_ready();

void task_block(tcb_t *task);
void task_unblock(tcb_t *task);

void task_delay(tcb_t *task);
void task_wake(tcb_t *task);


#ifdef DEBUG
tcb_t* get_cur_task();
#endif

bool init_scheduler(uint8_t num_priorities);
void scheduler_tick();
void handle_blocked();


#endif /* SCHEDULER_H_ */
