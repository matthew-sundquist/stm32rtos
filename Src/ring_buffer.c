
#include "ring_buffer.h"
#include <string.h>

static inline size_t get_total_elements(const uint32_t head, const uint32_t tail, const size_t size);

static inline size_t get_free_space(const uint32_t head, const uint32_t tail, const size_t size);

static inline bool is_empty(const uint32_t head, const uint32_t tail);

static inline bool is_full(const uint32_t head, const uint32_t tail);

bool ring_buffer_init(ring_buffer_t* rb, uint8_t *buf, size_t size)
{
	if (!rb || !buf || size <= 0)
	{
		return false;
	}

	rb->buf = buf;
	rb->size = size;
	rb->head = 0;
	rb->tail = 0;

	return true;
}

bool ring_buffer_push_byte(ring_buffer_t* rb, const uint8_t data)
{
	const static uint8_t size = 1;
	if (!rb)
	{
		return false;
	}


	if (is_full(rb->head, rb->tail))
	{
		// full case
		return false;
	}

	rb->buf[rb->head] = data;
	rb->head = (rb->head + size) % rb->size;
	return true;
}

bool ring_buffer_push(ring_buffer_t *rb, const uint8_t *data, size_t size)
{
	if (!rb || !data || size <= 0)
	{
		return false;
	}

	if (size > get_free_space(rb->head, rb->tail, rb->size))
	{
		return false;
	}

	// split into two memcpy's each copying one contigous memory space
	size_t len_first_seg = rb->size - rb->head;

	if (len_first_seg > size)
	{
		len_first_seg = size;
	}

	memcpy(&rb->buf[rb->head], data, len_first_seg);
	memcpy(&rb->buf[0], data + len_first_seg, size - len_first_seg);

	rb->head = (rb->head + size) % rb->size;
	return true;
}

bool ring_buffer_pop_byte(ring_buffer_t* rb, uint8_t* data)
{
	if (!rb || !data)
	{
		return false;
	}

	if (is_empty(rb->head, rb->tail))
	{
		return false;
	}

	*data = rb->tail;

	rb->tail++;

	return true;

}

bool ring_buffer_pop(ring_buffer_t* rb, uint8_t *data, size_t size)
{
	if (!rb || !data || size <= 0)
	{
		return false;
	}

	if (size > get_total_elements(rb->head, rb->tail, rb->size))
	{
		return false;
	}

	size_t len_first_seg = rb->size - rb->tail;

	if (len_first_seg > size)
	{
		len_first_seg = size;
	}

	memcpy(data, &rb->buf[rb->tail], len_first_seg);
	memcpy(data + len_first_seg, &rb->buf[0], size - len_first_seg);

	rb->tail = (rb->tail + size) % rb->size;

	return true;
}

uint32_t ring_buffer_get_count(ring_buffer_t* rb)
{
	if (!rb)
	{
		return 0;
	}
	return get_total_elements(rb->head, rb->tail, rb->size);
}

bool ring_buffer_empty(ring_buffer_t* rb)
{
	return is_empty(rb->head, rb->tail);
}

static inline size_t get_free_space(const uint32_t head, const uint32_t tail, const size_t size)
{
	return size - get_total_elements(head, tail, size);
}

static inline size_t get_total_elements(const uint32_t head, const uint32_t tail, const size_t size)
{
	return (head + size - tail) % size;
}

static inline bool is_empty(const uint32_t head, const uint32_t tail)
{
	return head == tail;
}

static inline bool is_full(const uint32_t head, const uint32_t tail)
{
	return (head+1) == tail;
}
