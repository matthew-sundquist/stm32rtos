/*
 * mqueue.h
 *
 *  Created on: Jun 13, 2026
 *      Author: Matthew Sundquist
 */

#ifndef MQUEUE_H_
#define MQUEUE_H_

#include <stdint.h>
#include <stdbool.h>

typedef struct mqueue {
	void *buffer;
	size_t capacity;
	size_t ele_size;

	uint32_t head;
	uint32_t tail;
	uint32_t count;
} mqueue_t;

void mqueue_init(mqueue_t *mq, void *buffer, size_t ele_size, uint32_t capacity);
bool mqueue_push(mqueue_t *mq, void *data); // pushes single element
bool mqueue_pop(mqueue_t *mq, void *data); // pops single element

#endif /* MQUEUE_H_ */
