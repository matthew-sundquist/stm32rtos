/*
 * mutex.h
 *
 *  Created on: Jun 6, 2026
 *      Author: Matthew Sundquist
 */

#ifndef MUTEX_H_
#define MUTEX_H_

#include <stdlib.h>
#include "task_queue.h"

typedef struct mutex
{
	uint32_t owner_pid; // if 0, no one has mutex, if 1, someone has mutex

	task_queue_t delayed_tasks;
} mutex_t;

void mutex_aquire(mutex_t *mut);
void mutex_release(mutex_t *mut);

#endif /* MUTEX_H_ */
