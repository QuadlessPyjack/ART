/*
 * consts.h
 *
 * Created: 20/02/2018 18:17:09
 *  Author: Bogdan
 */ 


#ifndef CONSTS_H_
#define CONSTS_H_

#include <stdint.h>

static const uint16_t DISPLAY_WIDTH = 128;
static const uint16_t DISPLAY_HEIGHT = 64;
static const uint16_t LINE_OFFSET = 0; // 64 for 64x32 0 for 128x64 (default size)

#ifndef TRUE
#define TRUE 0
#endif

#ifndef FALSE
#define FALSE 1
#endif

// to plot a pixel, apply following calculation for flat memory transformation:
// x * y * 64



#endif /* CONSTS_H_ */
