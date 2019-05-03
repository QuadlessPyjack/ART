/*
 * ble.h
 *
 * Created: 01/06/2018 20:25:31
 *  Author: Bogdan
 */ 


#ifndef BLE_H_
#define BLE_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/pgmspace.h>

#include <ctype.h>

#include "debug.h"
#include "payload.h"
#include "circularbuffer.h"

#define TX_PIN PA0
#define RX_PIN PA1
#ifndef _NOP
#define _NOP()  __asm__ volatile ("nop")
#endif

volatile int BIT_IDX = 0;
volatile unsigned char DBG_BYTE = 0;
volatile unsigned char RX_BYTE = 0;
volatile short RX_DONE = TRUE;
volatile short ISR_FIRED = FALSE;

volatile CBufferPtr BUFFER;

//! Initializes connection with BLE transmitter
//! Sets up Tx and Rx pins, Rx interrupt and initializes Rx buffer
void initBle()
{
	PORTA = (1 << PORTA0) | (0 << PORTA1);
	DDRA = (0 << DDA0) | (1 << DDA1);
	GIMSK = 0b00010000;
	PCMSK0 |= (1 << PCINT0);
	sei();
	BUFFER = cbuffer_init();
	//trace2Buffer(":");
}

inline void clearBleInterrupt()
{
	PCMSK0 |= (0 << PCINT0);
}

inline void setBleInterrupt()
{
	PCMSK0 |= (1 << PCINT0);
}

//! Reads from the Rx line 4 bytes at a time
//! This routine is triggered by PCINT0 which monitors the Rx line
//! \warning This routine is not meant to be called on its own! 
void serialRead()
{
	//_delay_us(37); WORKING
	for (uint8_t byteIdx = 0; byteIdx < 4; byteIdx++)
	{
		_delay_us(54); //53 // 54 seems stable for now. if desync'd, revert to value ABOVE!
		if (byteIdx > 0) { _delay_us(163); }
		while(BIT_IDX < 8)
		{
			RX_BYTE |= (PINA & 0x01) << BIT_IDX;
					PORTA |= (1 << PA1);
					_delay_us(1);
					PORTA &= ~(1 << PA1);
			BIT_IDX++;
			_delay_us(59); //60
		}

		cbuffer_put(BUFFER, RX_BYTE);
		DBG_BYTE = RX_BYTE;
		BIT_IDX = 0;
		RX_BYTE = 0;
	}

	PORTA |= (1 << PA1);
	_delay_us(1);
	PORTA &= ~(1 << PA1);
}

//! Prints the current value in RX_BYTE
//! \param binaryTrace Set to 0 to print RX_BYTE in binary form, 1 to print in decimal
void debugLogRXByteToScreen(uint8_t binaryTrace)
{
	if (RX_DONE == TRUE)
	{
		if (RX_BYTE != 0)
		{
			traceInt(1);
			if (binaryTrace == 0)
			{
				for (uint8_t idx = 0; idx < 8; ++idx)
				{
					uint8_t isSetFlag = 0b00000001 << idx;
					trace2Buffer(((RX_BYTE & isSetFlag) == isSetFlag) ? "1" : "0");
				}
			}
			//traceInt(BUFFER->count);					
		}
		RX_BYTE = 0;
		// workaround: read done, allow interrupt to fire again
		ISR_FIRED = FALSE;
	}
}

//! Prints the current value of RX_BYTE in binary, with separator
void debugBinaryTrace()
{
	if (RX_BYTE == 0)
	{
		return;
	}
	
	for (uint8_t idx = 0; idx < 8; ++idx)
	{
		uint8_t isSetFlag = 0b00000001 << idx;
		trace2Buffer(((RX_BYTE & isSetFlag) == isSetFlag) ? "1" : "0");
	}
	
	trace2Buffer(":");
	RX_BYTE = 0;
}

//! This interrupt is triggered when a change in potential is found on the Rx line
//! signaling that data will be received from BLE. 
//! The interrupt then calls serialRead() to process the incoming 4 byte payload
ISR(PCINT0_vect)
{
	// make sure we're not triggering the ISR during a read
	if (ISR_FIRED == TRUE)
	{
		return;
	}			

	ISR_FIRED = TRUE;
	serialRead();
	RX_DONE = TRUE;
}

#endif /* BLE_H_ */