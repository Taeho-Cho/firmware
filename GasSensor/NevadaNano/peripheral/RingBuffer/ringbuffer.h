/*
 * ringbuffer.h
 *
 *  Created on: Oct 18, 2021
 *      Author: chotaeho
 */

#ifndef RINGBUFFER_RINGBUFFER_H_
#define RINGBUFFER_RINGBUFFER_H_

#include "main.h"


#define BUFFER_SIZE	256


typedef enum {
	eERROR_CODE_BUFFER_EMPTY,
	eERROR_CODE_BUFFER_FULL,
	eERROR_CODE_NOT_ENOUGH_TO_READ
} eERROR_CODE_t;


typedef struct
{
	uint32_t head;
	uint32_t tail;
	uint8_t  buffer[BUFFER_SIZE];
	eERROR_CODE_t error_code;
} sRING_BUFFER_t;


void init(sRING_BUFFER_t*);
bool isFull(sRING_BUFFER_t*);
bool isEmpty(sRING_BUFFER_t*);
bool push(sRING_BUFFER_t*, uint8_t);
bool pop(sRING_BUFFER_t*, uint8_t*);
uint32_t numberOfData(sRING_BUFFER_t*);


#endif /* RINGBUFFER_RINGBUFFER_H_ */
