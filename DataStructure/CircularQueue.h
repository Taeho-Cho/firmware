#ifndef _CIRCULAR_QUEUE_H_
#define _CIRCULAR_QUEUE_H_

#ifdef _CIRCULAR_QUEUE_C_
    #define CIRCULAR_QUEUE_EXT
#else
	#define CIRCULAR_QUEUE_EXT extern
#endif

#define MAX_NUMBER_OF_DATA  500
#define DATA_TYPE           uint8_t

typedef enum {
	False = 0U, True = 1U
} Boolean;

typedef struct {
	uint32_t head;
	uint32_t tail;
	DATA_TYPE data[MAX_NUMBER_OF_DATA];
} CIRCULAR_QUEUE;

CIRCULAR_QUEUE_EXT void CQ_init(CIRCULAR_QUEUE *cq);

CIRCULAR_QUEUE_EXT Boolean CQ_isEmpty(CIRCULAR_QUEUE *cq);
CIRCULAR_QUEUE_EXT Boolean CQ_isFull(CIRCULAR_QUEUE *cq);

CIRCULAR_QUEUE_EXT Boolean CQ_enqueue(CIRCULAR_QUEUE *cq, DATA_TYPE data);
CIRCULAR_QUEUE_EXT Boolean CQ_dequeue(CIRCULAR_QUEUE *cq, DATA_TYPE *data);

CIRCULAR_QUEUE_EXT uint32_t CQ_NumberOfData(CIRCULAR_QUEUE *cq);

#endif
