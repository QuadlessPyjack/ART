/*
 * cqueue.h
 *
 * Created: 03/04/2019 11:03:27
 *  Author: bogdan
 */ 

#ifndef CQUEUE_H_
#define CQUEUE_H_

#include <stdint.h>
#include "payload.h"

#define CQUEUE_SIZE 4

typedef struct Data
{
	enum E_TYPE
	{
		NOT_SET = 0,
		UINT8   = 1,
		UINT16  = 2,
		PAYLOAD = 3		
	} type;
	
	union
	{
		uint8_t  uint8;
		uint16_t uint16;
		Payload  payload;
	} value;
} Data;

typedef struct CircularQueue
{
	Data buffer[CQUEUE_SIZE];
	uint8_t widx;
	uint8_t ridx;
	uint8_t count;
} CircularQueue;

typedef CircularQueue* CQueuePtr;

CQueuePtr cqueue_init();
int cqueue_isEmpty(CQueuePtr buffer);
int cqueue_isFull(CQueuePtr buffer);
Data cqueue_dequeue(CQueuePtr buffer);
void cqueue_enqueue(CQueuePtr buffer, const Data val);
void cqueue_flush(CQueuePtr buffer);



#endif /* CQUEUE_H_ */