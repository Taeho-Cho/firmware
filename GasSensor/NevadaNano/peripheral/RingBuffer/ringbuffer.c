/*
 * ringbuffer.c
 *
 *  Created on: Oct 18, 2021
 *      Author: chotaeho
 */

#include "ringbuffer.h"


void init(sRING_BUFFER_t *ringBuffer)
{
	ringBuffer->head = 0;
	ringBuffer->tail = 0;
	memset(ringBuffer->buffer, 0, BUFFER_SIZE);
}


bool isEmpty(sRING_BUFFER_t *ringBuffer)
{
	bool ret = false;

	if(ringBuffer->head == ringBuffer->tail) ret = true;

	return ret;
}


bool isFull(sRING_BUFFER_t *ringBuffer)
{
	bool ret = false;

	if(((ringBuffer->head + 1) % BUFFER_SIZE) == ringBuffer->tail) ret = true;

	return ret;
}


bool push(sRING_BUFFER_t *ringBuffer, uint8_t data)
{
	bool ret = true;

	if( isFull(ringBuffer) ) { ret = false; }
	else { ringBuffer->buffer[ringBuffer->head++] = data; }

	if(ringBuffer->head == BUFFER_SIZE) ringBuffer->head = 0;

	return ret;
}


bool pop(sRING_BUFFER_t *ringBuffer, uint8_t *data)
{
	bool ret = true;

	if( isEmpty(ringBuffer) ) { ret = false; }
	else { *data = ringBuffer->buffer[ringBuffer->tail++]; }

	if(ringBuffer->tail == BUFFER_SIZE) ringBuffer->tail = 0;

	return ret;
}


uint32_t numberOfData(sRING_BUFFER_t *ringBuffer)
{
	return (ringBuffer->head + BUFFER_SIZE - ringBuffer->tail) % BUFFER_SIZE;
}
