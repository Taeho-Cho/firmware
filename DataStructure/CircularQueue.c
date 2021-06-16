
#define _CIRCULAR_QUEUE_C_
    #include "CircularQueue.h"
#undef _CIRCULAR_QUEUE_C_

void CQ_init(CIRCULAR_QUEUE *cq)
{
    cq->head = 0;
    cq->tail = 0;
    memset(cq->data, 0, sizeof(DATA_TYPE) * MAX_NUMBER_OF_DATA);
}


uint8_t CQ_is_empty(CIRCULAR_QUEUE *cq)
{
  if(cq->tail == cq->head) return 1;
  
  return 0;
}


uint8_t CQ_is_full(CIRCULAR_QUEUE *cq)
{
  if( ((cq->tail + 1)%MAX_NUMBER_OF_DATA) == cq->head) return 1;
  
  return 0;
}


uint8_t CQ_push(CIRCULAR_QUEUE *cq, DATA_TYPE data)
{
  if( CQ_is_full(cq) )   return 0;
    
  cq->tail = (cq->tail+1)%MAX_NUMBER_OF_DATA;
  cq->data[cq->tail] = data;
  return 1;
}


DATA_TYPE CQ_pop(CIRCULAR_QUEUE *cq)
{
    if( CQ_is_empty(cq) ) return 0;
    
    cq->head = (cq->head+1)%MAX_NUMBER_OF_DATA;
    return cq->data[cq->head];
}


DATA_TYPE CQ_peek(CIRCULAR_QUEUE *cq, uint8_t index)
{
    if( CQ_is_empty(cq) ) return 0;
    
    if( ((cq->head + index)%MAX_NUMBER_OF_DATA) > cq->tail ) return 0;

    return cq->data[cq->head + index];
}


uint8_t CQ_delete(CIRCULAR_QUEUE *cq, uint8_t number_of_data)
{   
    for(uint8_t i=0; i < number_of_data; i++ )
    {
        if( CQ_pop(cq) == 0) return 0;
    }

    return 1;
}
