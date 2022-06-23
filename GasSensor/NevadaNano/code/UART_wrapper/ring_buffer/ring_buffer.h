/*
 * ringbuffer.h
 *
 *  Created on: Oct 18, 2021
 *      Author: chotaeho
 */

#ifndef RING_BUFFER_H_
#define RING_BUFFER_H_

#include "stdint.h"
#include "stdbool.h"



#define RING_BUFFER_SIZE    128


typedef enum {
	eERROR_CODE_BUFFER_EMPTY,
	eERROR_CODE_BUFFER_FULL,
	eERROR_CODE_NOT_ENOUGH_TO_READ
} eERROR_CODE_t;


typedef struct
{
	uint32_t head;
	uint32_t tail;
	uint8_t  buffer[RING_BUFFER_SIZE];
	eERROR_CODE_t error_code;
} sRING_BUFFER_t;


void RB_init(sRING_BUFFER_t*);
bool RB_isFull(sRING_BUFFER_t*);
bool RB_isEmpty(sRING_BUFFER_t*);
bool RB_push(sRING_BUFFER_t*, uint8_t);
bool RB_pop(sRING_BUFFER_t*, uint8_t*);
uint32_t RB_number_of_data(sRING_BUFFER_t*);


#endif /* RING_BUFFER_H_ */
