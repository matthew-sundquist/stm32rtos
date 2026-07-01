/*
 * ring_buffer.h
 *
 *  Created on: Jun 29, 2026
 *      Author: Matthew Sundquist
 */

#ifndef RING_BUFFER_H_
#define RING_BUFFER_H_

#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>


typedef struct ring_buffer {
	uint8_t *buf;
	size_t size;

	uint64_t head;
	uint64_t tail;
} ring_buffer_t;

bool ring_buffer_init(ring_buffer_t* rb, uint8_t *buf, size_t size);

bool ring_buffer_push_byte(ring_buffer_t* rb, const uint8_t data);

bool ring_buffer_push(ring_buffer_t *rb, const uint8_t *data, size_t size);

bool ring_buffer_pop_byte(ring_buffer_t* rb, uint8_t* data);

bool ring_buffer_pop(ring_buffer_t* rb, uint8_t *data, size_t size);

uint32_t ring_buffer_get_count(ring_buffer_t* rb);

bool ring_buffer_empty(ring_buffer_t* rb);


#endif /* RING_BUFFER_H_ */
