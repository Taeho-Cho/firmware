#ifndef _CIRCULAR_QUEUE_H_
#define _CIRCULAR_QUEUE_H_

#include "main.h"

#ifdef _CIRCULAR_QUEUE_C_
        #define CIRCULAR_QUEUE_EXT
#else
        #define CIRCULAR_QUEUE_EXT extern
#endif

#define MAX_NUMBER_OF_DATA  500
#define DATA_TYPE           uint8_t

typedef enum {
	eCQ_False = 0U, 
    eCQ_True  = 1U
} eCQ_Boolean_t;

typedef struct 
{
	uint32_t head;
	uint32_t tail;
	DATA_TYPE data[MAX_NUMBER_OF_DATA];
}CircularQueue;

CIRCULAR_QUEUE_EXT void CQ_init(CircularQueue *cq);

CIRCULAR_QUEUE_EXT eCQ_Boolean_t CQ_isEmpty(CircularQueue *cq);
CIRCULAR_QUEUE_EXT eCQ_Boolean_t CQ_isFull(CircularQueue *cq);

CIRCULAR_QUEUE_EXT eCQ_Boolean_t CQ_enqueue(CircularQueue *cq, DATA_TYPE data);
CIRCULAR_QUEUE_EXT eCQ_Boolean_t CQ_dequeue(CircularQueue *cq, DATA_TYPE *data);

CIRCULAR_QUEUE_EXT uint32_t CQ_NumberOfData(CircularQueue *cq);

#endif
