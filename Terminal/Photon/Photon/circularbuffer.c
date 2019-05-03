/*
 * circularbuffer.c
 *
 * Created: 12/03/2019 15:59:54
 *  Author: bogdan
 */ 

#include <string.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include "utils/circularbuffer.h"


CBufferPtr cbuffer_init()
{
    CBufferPtr buffer = (CircularBuffer*)malloc(sizeof(CircularBuffer));
    memset(buffer, 0, sizeof(CircularBuffer));

    return buffer;
}

int cbuffer_isEmpty(CBufferPtr buffer)
{
	return buffer->count;
}

int cbuffer_isFull(CBufferPtr buffer)
{
	return (buffer->count >= CBUFFER_SIZE);
}

int cbuffer_get(CBufferPtr buffer)
{
	int c;
	if (buffer->count > 0)
	{
        c = buffer->buffer[buffer->index - 1];
		buffer->index = ((buffer->index - 1) % CBUFFER_SIZE);
		buffer->count--;
	} else {
		c = -1;
	}
	
	return c;
}

void cbuffer_put(CBufferPtr buffer, const uint8_t val)
{
	buffer->buffer[buffer->index] = val;
	buffer->index = ((buffer->index + 1) % CBUFFER_SIZE);
	++(buffer->count);
}

void cbuffer_flush(CBufferPtr buffer)
{
	buffer->count = 0;
	buffer->index = 0;
	memset(buffer->buffer, 0, sizeof(buffer->buffer));
}
