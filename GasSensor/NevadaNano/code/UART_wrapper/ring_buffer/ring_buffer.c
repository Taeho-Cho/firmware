/*
 * ringbuffer.c
 *
 *  Created on: Oct 18, 2021
 *      Author: chotaeho
 */

#include "ring_buffer.h"
#include "string.h"

void RB_init(sRING_BUFFER_t *ringBuffer)
{
	ringBuffer->head = 0;
	ringBuffer->tail = 0;
	memset(ringBuffer->buffer, 0, RING_BUFFER_SIZE);
}


bool RB_isEmpty(sRING_BUFFER_t *ringBuffer)
{
	bool ret = false;

	if(ringBuffer->head == ringBuffer->tail) ret = true;

	return ret;
}


bool RB_isFull(sRING_BUFFER_t *ringBuffer)
{
	bool ret = false;

	if(((ringBuffer->head + 1) % RING_BUFFER_SIZE) == ringBuffer->tail) ret = true;

	return ret;
}


bool RB_push(sRING_BUFFER_t *ringBuffer, uint8_t data)
{
	bool ret = true;

	if( RB_isFull(ringBuffer) ) { ret = false; }
	else { ringBuffer->buffer[ringBuffer->head++] = data; }

	if(ringBuffer->head == RING_BUFFER_SIZE) ringBuffer->head = 0;

	return ret;
}


bool RB_pop(sRING_BUFFER_t *ringBuffer, uint8_t *data)
{
	bool ret = true;

	if( RB_isEmpty(ringBuffer) ) { ret = false; }
	else { *data = ringBuffer->buffer[ringBuffer->tail++]; }

	if(ringBuffer->tail == RING_BUFFER_SIZE) ringBuffer->tail = 0;

	return ret;
}


uint32_t RB_number_of_data(sRING_BUFFER_t *ringBuffer)
{
	return (ringBuffer->head + RING_BUFFER_SIZE - ringBuffer->tail) % RING_BUFFER_SIZE;
}
