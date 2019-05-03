/*
 * circularbuffer.h
 *
 * Created: 12/03/2019 15:59:28
 *  Author: bogdan
 */ 


#ifndef CIRCULARBUFFER_H_
#define CIRCULARBUFFER_H_
//#include <avr/io.h>
#include <stdint.h>
#define CBUFFER_SIZE 32

typedef struct CircularBuffer
{
	uint8_t buffer[CBUFFER_SIZE];
	uint8_t index;
	uint8_t count;
} CircularBuffer;

typedef CircularBuffer* CBufferPtr;

CBufferPtr cbuffer_init();
int cbuffer_isEmpty(CBufferPtr buffer);
int cbuffer_isFull(CBufferPtr buffer);
int cbuffer_get(CBufferPtr buffer);
void cbuffer_put(CBufferPtr buffer, const uint8_t val);
void cbuffer_flush(CBufferPtr buffer);



#endif /* CIRCULARBUFFER_H_ */
