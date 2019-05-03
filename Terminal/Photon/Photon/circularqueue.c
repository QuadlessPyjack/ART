/*
 * circularqueue.c
 *
 * Created: 03/04/2019 11:04:04
 *  Author: bogdan
 */

#include <string.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include "utils/circularqueue.h"

CQueuePtr cqueue_init()
{
	CQueuePtr queue = (CircularQueue*)malloc(sizeof(CircularQueue));
	memset(queue, 0, sizeof(CircularQueue));

	return queue;
}
int cqueue_isEmpty(CQueuePtr buffer)
{
	return buffer->count;
}
int cqueue_isFull(CQueuePtr buffer)
{
	return (buffer->count >= CQUEUE_SIZE);
}
Data cqueue_dequeue(CQueuePtr buffer)
{
	Data d;
	d.type = NOT_SET;
	if (buffer->ridx == buffer->widx)
	{
		return d;
	}
	
	d = buffer->buffer[buffer->ridx];
	buffer->ridx = ((buffer->ridx + 1) % CQUEUE_SIZE);
	--(buffer->count);

	return d;
}
void cqueue_enqueue(CQueuePtr buffer, const Data val)
{
	buffer->buffer[buffer->widx] = val;
	buffer->widx = ((buffer->widx + 1) % CQUEUE_SIZE);
	++(buffer->count);
}
void cqueue_flush(CQueuePtr buffer)
{
	buffer->count = 0;
	buffer->widx = 0;
	buffer->ridx = 0;
	memset(buffer->buffer, 0, sizeof(buffer->buffer));
}