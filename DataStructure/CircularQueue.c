#define _CIRCULAR_QUEUE_C_
    #include "CircularQueue.h"
#undef _CIRCULAR_QUEUE_C_

void CQ_init(CIRCULAR_QUEUE *cq)
{
    cq->head = 0;
    cq->tail = 0;
    memset(cq->data, 0, sizeof(DATA_TYPE) * MAX_NUMBER_OF_DATA);
}


Boolean CQ_isEmpty(CIRCULAR_QUEUE *cq)
{
  if(cq->tail == cq->head) return True;

  return False;
}


Boolean CQ_isFull(CIRCULAR_QUEUE *cq)
{
  if( ( (cq->tail + 1) % MAX_NUMBER_OF_DATA ) == cq->head) return True;

  return False;
}


Boolean CQ_enqueue(CIRCULAR_QUEUE *cq, DATA_TYPE data)
{
  if( CQ_is_full(cq) )   return False;

  cq->tail = (cq->tail + 1) % MAX_NUMBER_OF_DATA;
  cq->data[cq->tail] = data;
  return True;
}


Boolean CQ_dequeue(CIRCULAR_QUEUE *cq, DATA_TYPE *data)
{
    if( CQ_is_empty(cq) ) return False;

    *data = cq->data[cq->head];
    cq->head = (cq->head + 1) % MAX_NUMBER_OF_DATA;
    return True;
}


uint32_t CQ_NumberOfData(CIRCULAR_QUEUE *cq)
{
	return (cq->tail + MAX_NUMBER_OF_DATA - cq->head) % MAX_NUMBER_OF_DATA;
}
