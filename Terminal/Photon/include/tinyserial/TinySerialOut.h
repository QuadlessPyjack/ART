/*
 * TinySerialOut.h
 *
 *  Copyright (C) 2015  Armin Joachimsmeyer
 *  Email: armin.joachimsmeyer@gmail.com
 *  License: GPL v3 (http://www.gnu.org/licenses/gpl.html)
 *
 */

#ifndef TINY_SERIAL_OUT_H_
#define TINY_SERIAL_OUT_H_

#include <stdint.h>
#include <avr/interrupt.h>  // for cli() and sei()
#include <avr/io.h>

/*
 * Change this, if you need another pin as serial output
 * or set it as Symbol
 */
#ifndef TX_PIN
#define TX_PIN     PA0 // use one of PB0 to PB4 (+PB5) here
#endif

#define USE115200BAUD // outcomment this to use bigger (120 bytes for unrolled loop) but faster code

int8_t sUseCliSeiForStrings;
void useCliSeiForStrings(int8_t aUseCliSeiForStrings);

inline void initTXPin() {
    // TX_PIN is active LOW, so set it to HIGH initially
    PORTA |= (1 << TX_PIN);
    // set pin direction to output
    DDRA |= (1 << TX_PIN);
}

void write1Start8Data1StopNoParity(uint8_t aChar);
inline void write1Start8Data1StopNoParityWithCliSei(uint8_t aChar) {
    cli();
    write1Start8Data1StopNoParity(aChar);
    sei();
}

void writeString(const char * aStringPtr);
void writeString_P(const char * aStringPtr);
void writeString_E(const char * aStringPtr);
void writeStringWithCliSei(const char * aStringPtr);
void writeStringWithoutCliSei(const char * aStringPtr);
void writeStringSkipLeadingSpaces(const char * aStringPtr);

void writeByte(int8_t aByte);
void writeUnsignedByte(uint8_t aByte);
void writeUnsignedByteHex(uint8_t aByte);
void writeInt(int aInteger);
void writeUnsignedInt(unsigned int aInteger);
void writeLong(long aLong);
void writeFloat(double aFloat);

#endif /* TINY_SERIAL_OUT_H_ */
