
#include <stdlib.h>
#include "assert.h"
#include "mqueue.h"
#include <string.h>

#include <stm32l4xx.h>

void mqueue_init(mqueue_t *mq, void *buffer, size_t ele_size, uint32_t capacity)
{
	ASSERT(mq != NULL);
	ASSERT(buffer != NULL);
	ASSERT(capacity > 0);
	ASSERT(ele_size > 0);

	mq->buffer = buffer;
	mq->capacity = capacity;
	mq->count = 0;
	mq->head = 0;
	mq->tail = 0;
}

bool mqueue_push(mqueue_t *mq, void *data) // pushes single element
{
	__disable_irq();
	ASSERT(mq != NULL);
	ASSERT(data != NULL);
	if (mq->count >= mq->capacity)
	{
		__enable_irq();
		return false;
	}

	memcpy((uint8_t*)mq->buffer + (mq->tail * mq->ele_size), data, mq->ele_size);

	mq->tail = (mq->tail + 1) % mq->ele_size; // wrap tail

	mq->count++;

	__enable_irq();
	return true;

}
bool mqueue_pop(mqueue_t *mq, void *data) // pops single element
{
	__disable_irq();
	ASSERT(mq != NULL);
	ASSERT(data != NULL);

	if (mq->count < 1)
	{
		__enable_irq();
		return false;
	}

	memcpy((uint8_t*)mq->buffer + (mq->head * mq->ele_size), data, mq->ele_size);

	mq->head = (mq->head + 1) % mq->ele_size;

	mq->count--;

	__enable_irq();

	return true;
}
