/*
 * debug.h
 *
 * Created: 15/10/2018 19:59:51
 *  Author: Bogdan
 */

#ifndef _DEBUG_H_
#define _DEBUG_H_

#include "ssd1306xled.h"

/**
*	@file debug.h
*   @author Bogdan Vitel
*   @date 15/10/2018 19:59:51
*   @brief Misc debug routines used to check various functionality
*
*	Usage: place #define NAMESPACE to enable only the routine you want to use
*   Example: to use the Parity Check Test, in your source file set #define _DEBUG_USING_CHECKCOUNTER
*/

//! Plots a pixel on the screen at the specified coordinates
//! NO BOUND CHECK IMPLEMENTED
//! \param x The X coordinate of the pixel
//! \param y The Y coordinate of the pixel
void dbgPlotPixel(uint16_t x, uint16_t y)
{
	ssd1306_setpos(0, 0);

	uint8_t shift_counter = y % 8;
	if (y % 8 != 0)
	{
		y = y / 8;
	}
	// picture this as a CRT spot moving left-right, up-down
	uint16_t pixelAddress = (0 + 128) * y + x;
	ssd1306_send_data_start();
	for (int idx = 0; idx < pixelAddress; ++idx)
	{
		if (idx == (pixelAddress - 1))
		{
			ssd1306_send_byte(0b00000001 << shift_counter);
			continue;
		}
		
		ssd1306_send_byte(0b00000000);
	}
	
	ssd1306_send_data_stop();
}

#ifdef _DEBUG_USING_BIN_TRACE
// forward declaration
void trace2Buffer(const char* message);

//! Prints a numeric value in binary form, separated by :
//! \param val the value to print
void trace2Binary(uint8_t val)
{
	for (uint8_t idx = 0; idx < 8; ++idx)
	{
		uint8_t isSetFlag = 0b00000001 << idx;
		trace2Buffer(((val & isSetFlag) == isSetFlag) ? "1" : "0");
	}
	trace2Buffer(":");
}
#endif

//------------------------BLE Debug Utils-----------------------------------------------
#ifdef _DEBUG_USING_BLE
//! Sends a 1us pulse over the PA1 (Tx) line
void writeTestLoop()
{
	PORTA |= (1 << PA1);
	_delay_us(1);
	PORTA &= ~(1 << PA1);
}
#endif

//-----------------------Container Tests------------------------------------------------
#ifdef _DEBUG_USING_CONTAINERS
//! Circular Buffer Test
//! Fills the buffer with uint8_t then reads from buffer
void cbufferTest()
{
	for (uint8_t idx = 0; idx < 4; ++idx)
	{
		cbuffer_put(BUFFER, idx);
		if(cbuffer_isEmpty(BUFFER) > 0)
		{
			int8_t byte = cbuffer_get(BUFFER);
			char msg[2] = {'0' + byte, 0};
			trace2Buffer(msg);
		}
	}
}

void cqueueTest()
{
Data d0;
Data d1;
Data d2;
Data d3;

Payload p0;
Payload p1;
Payload p2;
Payload p3;

p0.data[0] = '0';
p1.data[0] = '1';
p2.data[0] = '2';
p3.data[0] = '3';

d0.type = UINT8;
d1.type = UINT8;
d2.type = UINT8;
d3.type = UINT8;

d0.value.uint8 = 0;
d1.value.uint8 = 1;
d2.value.uint8 = 2;
d3.value.uint8 = 3;

CQueuePtr testQueue;
testQueue = cqueue_init();

cqueue_enqueue(testQueue, d0);
cqueue_enqueue(testQueue, d1);
cqueue_enqueue(testQueue, d2);
cqueue_enqueue(testQueue, d3);

Data d = cqueue_dequeue(testQueue);
//Payload p;
//p = d.value.payload;
//char msg[2] = {p.data[0], 0};

traceInt(d.value.uint8);

Data dA;
//dA.type = 3;
//dA.value.payload.data[0] = 0;

dA = cqueue_dequeue(testQueue);
//p = dA.value.payload;
//char msg0[2] = {p.data[0], 0};

traceInt(dA.value.uint8);

d = cqueue_dequeue(testQueue);
//p = d.value.payload;
//char msg1[2] = {p.data[0], 0};

//trace2Buffer(msg1);
traceInt(d.value.uint8);

d = cqueue_dequeue(testQueue);
//p = d.value.payload;
//char msg2[2] = {p.data[0], 0};

//trace2Buffer(msg2);
traceInt(d.value.uint8);
}

volatile static Payload p;
#endif

//------------------------------Parity Check Counter Test--------------------
#ifdef _DEBUG_USING_CHECKCOUNTER
#define _CHECKCOUNTER_INIT 	int i = -1;uint8_t good = 0;uint8_t bad = 0;

//! Place this in the render loop
void goodBadCheckCounterDraw()
{
	traceInt(good);
	trace2Buffer(":");
	traceInt(bad);
	if (i > -1)
	{
		traceInt(i);
	} else
	{
		traceInt(9);
	}
}

//! Place this where the payload is received
void goodBadCheckCounterUpdate()
{
	if (isEvenParityValid(&p, &i) == TRUE)
	{
		good++;
		} else {
		bad++;
	}
}
#endif

#endif // _DEBUG_H_