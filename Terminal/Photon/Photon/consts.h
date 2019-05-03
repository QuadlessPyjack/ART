/*
 * consts.h
 *
 * Created: 20/02/2018 18:17:09
 *  Author: Bogdan
 */ 


#ifndef CONSTS_H_
#define CONSTS_H_

#ifdef _DESKTOP
#include <cstdint>
#endif

#include <avr/pgmspace.h>

static const uint16_t DISPLAY_WIDTH PROGMEM = 128;
static const uint16_t DISPLAY_HEIGHT PROGMEM = 64;
static const uint16_t LINE_OFFSET PROGMEM = 0; // 64 for 64x32 0 for 128x64 (default size)

#ifndef TRUE
#define TRUE 0
#endif

#ifndef FALSE
#define FALSE 1
#endif


#endif /* CONSTS_H_ */