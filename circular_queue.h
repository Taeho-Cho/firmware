#ifndef _CIRCULAR_QUEUE_H_
#define _CIRCULAR_QUEUE_H_

#include "main.h"

#ifdef _CIRCULAR_QUEUE_C_
    #define CIRCULAR_QUEUE_EXT
#else 
    #define CIRCULAR_QUEUE_EXT extern
#endif

#define MAX_NUMBER_OF_DATA  512
#define DATA_TYPE           uint8_t

typedef struct {
  uint16_t head; 
  uint16_t tail;
  DATA_TYPE data[MAX_NUMBER_OF_DATA];
} CIRCULAR_QUEUE;


CIRCULAR_QUEUE_EXT void CQ_init(CIRCULAR_QUEUE *cq);

CIRCULAR_QUEUE_EXT uint8_t CQ_is_empty(CIRCULAR_QUEUE *cq);
CIRCULAR_QUEUE_EXT uint8_t CQ_is_full (CIRCULAR_QUEUE *cq);

CIRCULAR_QUEUE_EXT uint8_t CQ_push(CIRCULAR_QUEUE *cq, DATA_TYPE data);

CIRCULAR_QUEUE_EXT DATA_TYPE CQ_pop(CIRCULAR_QUEUE *cq);

CIRCULAR_QUEUE_EXT DATA_TYPE CQ_peek(CIRCULAR_QUEUE *cq, uint8_t index);

CIRCULAR_QUEUE_EXT uint8_t CQ_delete(CIRCULAR_QUEUE *cq, uint8_t number_of_data);

#endif