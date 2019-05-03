/*
* Photon.c
*
* Created: 12/11/2017 23:43:37
*  Author: Bogdan
*/

/**
*   @file Photon.c
*   @author Bogdan Vitel
*   @date 12/11/2017 23:43:37
*   @brief Main entry point into the firmware
*/

#define F_CPU 1000000     // 1MHz
//#define USE115200BAUD

#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <string.h>

#define SSD1306_SCL		PB1	// SCL,	Pin 3 on SSD1306 Board
#define SSD1306_SDA		PB0	// SDA,	Pin 4 on SSD1306 Board
#define SSD1306_SA      0x78 // Slave address

#include "utils/gapi.h"
#include "utils/ble.h"
#include "utils/circularqueue.h"
#include "utils/payloadTypes.h"

//ISR(__vector_default)
//{
	//dbgPlotPixel(1,1);
	//dbgPlotPixel(2,2);
	//dbgPlotPixel(3,3);
	//dbgPlotPixel(1,3);
	//dbgPlotPixel(1,3);
//}

Payload readPayloadFromBle()
{
	if (cbuffer_isEmpty(BUFFER) < 4)
	{
		Payload empty;
		memset(&empty, 0, 4);
		return empty;
	}

	Payload p;

	uint8_t byte = cbuffer_get(BUFFER);
	p.data[METADATA] = byte;
	byte = cbuffer_get(BUFFER);
	p.data[ROTATION] = byte;
	byte = cbuffer_get(BUFFER);
	p.data[Y_COORDINATE] = byte;
	byte = cbuffer_get(BUFFER);
	p.data[X_COORDINATE] = byte;
	
	//isEvenParityValid(p);
	
	return p;
}

DrawCmd payloadToDrawCmd(Payload p)
{
	DrawCmd c;
	c.id = 1;
	c.x = p.data[X_COORDINATE];
	c.y = p.data[Y_COORDINATE];
	uint8_t meta = p.data[METADATA];
	uint8_t type = (meta & 0b00111000) >> 3;
	
	switch (type)
	{
		case 0:
		c.type = 't';
		break;
		case 1:
		c.type = 'm';
		break;
		default:
		c.type = 0;
		break;
	}

	return c;
}

void updateUi()
{
	Payload p = readPayloadFromBle();
	if (p.data[METADATA] == 0) { return; }
	int bIdx = 0;
	if (isEvenParityValid(&p, &bIdx) == FALSE) { return; }
	DrawCmd dc = payloadToDrawCmd(p);

	gapiLoadCommand(dc);
}

void init()
{
	_delay_ms(40);
	ssd1306_init();
	ssd1306_clear();
	initBle();
	gapiInit();
	//traceInt(0);
}

void render()
{
	cls();
	//memset(TEXT_BUFFER,0,64);
	ISR_FIRED = FALSE;
	updateUi();
	//drawTextBuffer();
	gapiUpdate();
	//blit();
}

int main(void)
{
	init();

	while(1)
	{
		render();
	}

	return 0;
}