#define _CIRCULAR_QUEUE_C_
    #include "CircularQueue.h"
#undef _CIRCULAR_QUEUE_C_

void CQ_init(CircularQueue *cq)
{
    cq->head = 0;
    cq->tail = 0;
    memset(cq->data, 0, sizeof(DATA_TYPE) * MAX_NUMBER_OF_DATA);
}


eCQ_Boolean_t CQ_isEmpty(CircularQueue *cq)
{
    if(cq->tail == cq->head) return eCQ_True;

    return eCQ_False;
}


eCQ_Boolean_t CQ_isFull(CircularQueue *cq)
{
    if( ( (cq->tail + 1) % MAX_NUMBER_OF_DATA ) == cq->head) return eCQ_True;

    return eCQ_False;
}


eCQ_Boolean_t CQ_enqueue(CircularQueue *cq, DATA_TYPE data)
{
    if( CQ_isFull(cq) )   return eCQ_False;

    cq->tail = (cq->tail + 1) % MAX_NUMBER_OF_DATA;
    cq->data[cq->tail] = data;
    return eCQ_True;
}


eCQ_Boolean_t CQ_dequeue(CircularQueue *cq, DATA_TYPE *data)
{
    if( CQ_isEmpty(cq) ) return eCQ_False;

    cq->head = (cq->head + 1) % MAX_NUMBER_OF_DATA;
    *data = cq->data[cq->head];
    return eCQ_True;
}


uint32_t CQ_NumberOfData(CircularQueue *cq)
{
	return (cq->tail + MAX_NUMBER_OF_DATA - cq->head) % MAX_NUMBER_OF_DATA;
}