/*
 * @file graphics.h
 *
 *  @date: 26/02/2018 17:09:00
 *  @author: Bogdan Vitel
 *  @brief Specialized graphics library for Project Photon
 */

#ifndef GRAPHICS_H_
#define GRAPHICS_H_

#ifndef _DESKTOP

#include <util/delay.h>
#include <avr/pgmspace.h>
#include <string.h>
#include "ssd1306xled.h"

#endif // _DESKTOP

#include "../consts.h"

#ifdef _DESKTOP
#include <cstdint>
#endif

//! Display buffer.
//! Due to memory constraints, this buffer only covers a DISPLAY_WIDTH x 16 sector of the 
//! active display memory.
static uint8_t GRAPHICS_BUFFER[256] = {0};
	
static void initGfxBuffer()
{
	memset(GRAPHICS_BUFFER, 0, 256);
	ssd1306_setpos(0, 0);
}

//! Used to keep track of the current Y-band sector we're in
static uint8_t CURRENT_DRAW_SECTOR = 0;

	//			         |h64(M)
	//	w128(M)__________|
	//			|  ______|________w64(D)
	//			| |    | |
	//			| |____| |
	//			|______|_|
	//			       |
	//			       |h32(D)

	// text offsets: X38 Y14
	
	// Display memory addressing scheme
	//
	// 4 "address lines" made of DISPLAY_WIDTH bytes
	// Y \ X ->    X[0]     X[1]     X[2]     X[3] ...         X[4]
	// | Ln 0  byte0[0] byte1[0] byte2[0] byte3[0] ... byteWIDTH[0] 0
	// V            [1]      [1]      [1]      [1] ...          [1] 0
	// byte0[0]<< 2 [2]      [2]      [2]      [2] ...          [2] 0
	// byte0[0]<< 3 [3]      [3]      [3]      [3] ...          [3] 0
	// byte0[0]<< 4 [4]      [4]      [4]      [4] ...          [4] 0
	//              [5]      [5]      [5]      [5] ...          [5] 0
	//              [6]		 [6]	  [6]	   [6] ...			[6] 0
	//              [7]		 [7]	  [7]	   [7] ...			[7] 0
	//   byteWIDTH+1[0]  bW+2[0]  bW+3[0]  bW+4[0] ..byteWIDTH*2[0] b0 (reverse order)

//! Sets a pixel as 'lit' at the specified graphics memory buffer location
//! \param idx The bit index for the pixel
static void writePixelAt(const uint32_t idx)
{
	uint8_t b_offset = idx % 8;
	uint32_t b_idx = idx >> 3; //idx / 8;                         
	GRAPHICS_BUFFER[b_idx] |= 0b00000001 << b_offset;
}

//! Reads the pixel value [lit|unlit] at the specified graphics memory buffer location
//! \param idx The bit index for the pixel
static inline uint8_t readPixelAt(const uint32_t idx)
{
	uint8_t b_offset = idx % 8;
	uint32_t b_idx = idx >> 3; //idx / 8; // uint8_t
	uint8_t isSetFlag = 0b00000001 << b_offset;
	
	return ((GRAPHICS_BUFFER[b_idx] & isSetFlag) == isSetFlag) ? 1 : 0;
}

//! Convenience macro for (X,Y) to linear conversion
//! \param idx Linear offset from the implicit O(x,y) origin point of the item drawn
#define OFFSET(idx) (x + (idx))+(y << 7) // + DISPLAY_WIDTH * y DISPLAY_WIDTH = 128 = 2^

//! Convenience macro for jumping one line (Y+1) from O(x,y)
//! \param idx Y offset from the implicit O(x,y) origin point of the item drawn
#define LINEPOS(idx) (idx << 7)

//! Draws a crosshair at the specified coordinates.
//! Used to mark the placement spot for the physical object being tracked.
//! \param x The X origin of the symbol
//! \param y The Y origin of the symbol
void drawTarget(uint16_t x, uint16_t y)
{
	writePixelAt(OFFSET(2));                 //A
	writePixelAt(OFFSET(LINEPOS(1) + 2));    //B
	writePixelAt(OFFSET(LINEPOS(2)));        //C     A
	writePixelAt(OFFSET(LINEPOS(2) + 1));    //D     B
	writePixelAt(OFFSET(LINEPOS(2) + 2));    //E   CDEFG 
	writePixelAt(OFFSET(LINEPOS(2) + 3));    //F     H
	writePixelAt(OFFSET(LINEPOS(2) + 4));    //G     I
	writePixelAt(OFFSET(LINEPOS(3) + 2));    //H
	writePixelAt(OFFSET(LINEPOS(4) + 2));    //I
}

//! Draws a square at the specified coordinates.
//! Used to mark the physical object being tracked.
//! \param x The X origin of the symbol
//! \param y The Y origin of the symbol
void drawMarker(uint16_t x, uint8_t y)
{
	writePixelAt(OFFSET(0));                        //A
	writePixelAt(OFFSET(1));                        //B
	writePixelAt(OFFSET(2));                        //C
	writePixelAt(OFFSET(3));                        //D   ABCD
	writePixelAt(OFFSET(LINEPOS(1)));               //E   E  F
	writePixelAt(OFFSET(LINEPOS(1) + 3));           //F   G  H
	writePixelAt(OFFSET(LINEPOS(2)));               //G   IKLJ
	writePixelAt(OFFSET(LINEPOS(2) + 3));           //H
	writePixelAt(OFFSET(LINEPOS(3)));               //I
	writePixelAt(OFFSET(LINEPOS(3) + 3));           //J
	writePixelAt(OFFSET(LINEPOS(3) + 1));           //K
	writePixelAt(OFFSET(LINEPOS(3) + 2));           //L
}

//! Lights a pixel at the given coordinates
//! \param x X coordinate of the pixel
//! \param y Y coordinate of the pixel
static inline void drawPixel(const uint16_t x, const uint16_t y)
{
	writePixelAt(OFFSET(0));
}

//! Text buffer used by the tracing utilities
static char TEXT_BUFFER[64] = "";

//! Draws a number @p num at @p x and @p y coordinates on screen
//! \param x X coordinate at which to draw the number
//! \param y Y coordinate at which to draw the number
//! \param num the number to draw
static inline void drawNumber(const uint8_t x, const uint8_t y, const uint8_t num)
{
	switch (num)
	{
		case 48://0:
		{
			writePixelAt(OFFSET(3 + LINEPOS(1)));
			writePixelAt(OFFSET(3 + LINEPOS(2)));
			writePixelAt(OFFSET(3 + LINEPOS(3)));
			writePixelAt(OFFSET(3 + LINEPOS(4)));
			writePixelAt(OFFSET(3 + LINEPOS(5)));

			writePixelAt(OFFSET(LINEPOS(1)));
			writePixelAt(OFFSET(LINEPOS(2)));
			writePixelAt(OFFSET(LINEPOS(3)));
			writePixelAt(OFFSET(LINEPOS(4)));
			writePixelAt(OFFSET(LINEPOS(5)));

			writePixelAt(OFFSET(1 + LINEPOS(1)));
			writePixelAt(OFFSET(2 + LINEPOS(1)));
			
			writePixelAt(OFFSET(1 + LINEPOS(5)));
			writePixelAt(OFFSET(2 + LINEPOS(5)));
			
			break;
		}
		case 49://1:
		{
			writePixelAt(OFFSET(LINEPOS(2)));
			writePixelAt(OFFSET(1 + LINEPOS(2)));

			writePixelAt(OFFSET(2 + LINEPOS(1)));
			writePixelAt(OFFSET(2 + LINEPOS(2)));
			writePixelAt(OFFSET(2 + LINEPOS(3)));
			writePixelAt(OFFSET(2 + LINEPOS(4)));
			writePixelAt(OFFSET(2 + LINEPOS(5)));
			break;
		}
		case 50://2:
		{
			writePixelAt(OFFSET(LINEPOS(1)));
			writePixelAt(OFFSET(LINEPOS(1) + 1));
			writePixelAt(OFFSET(LINEPOS(1) + 2));

			writePixelAt(OFFSET(2 + LINEPOS(2)));
			writePixelAt(OFFSET(2 + LINEPOS(3)));
			writePixelAt(OFFSET(1 + LINEPOS(4)));
			writePixelAt(OFFSET(LINEPOS(4)));
			
			writePixelAt(OFFSET(2 + LINEPOS(5)));
			writePixelAt(OFFSET(LINEPOS(5)));
			writePixelAt(OFFSET(1 + LINEPOS(5)));
			break;
		}
		case 51://3:
		{
			writePixelAt(OFFSET(LINEPOS(1)));
			writePixelAt(OFFSET(1 + LINEPOS(1)));
			writePixelAt(OFFSET(2 + LINEPOS(1)));

			writePixelAt(OFFSET(2 + LINEPOS(2)));
			writePixelAt(OFFSET(2 + LINEPOS(3)));
			writePixelAt(OFFSET(2 + LINEPOS(4)));
			writePixelAt(OFFSET(2 + LINEPOS(5)));

			writePixelAt(OFFSET(LINEPOS(1)));
			writePixelAt(OFFSET(1 + LINEPOS(1)));
			
			writePixelAt(OFFSET(LINEPOS(3)));
			writePixelAt(OFFSET(1 + LINEPOS(3)));

			writePixelAt(OFFSET(LINEPOS(5)));
			writePixelAt(OFFSET(1 + LINEPOS(5)));
			
			break;
		}
		case 52://4:
		{
			writePixelAt(OFFSET(LINEPOS(1)));
			writePixelAt(OFFSET(LINEPOS(2)));

			writePixelAt(OFFSET(1 + LINEPOS(3)));
			writePixelAt(OFFSET(2 + LINEPOS(3)));

			writePixelAt(OFFSET(2 + LINEPOS(4)));
			writePixelAt(OFFSET(2 + LINEPOS(5)));
			break;
		}
		case 53://5:
		{
			writePixelAt(OFFSET(0 + LINEPOS(1)));
			writePixelAt(OFFSET(1 + LINEPOS(1)));
			writePixelAt(OFFSET(2 + LINEPOS(1)));
			
			writePixelAt(OFFSET(LINEPOS(2)));
			writePixelAt(OFFSET(LINEPOS(3)));

			writePixelAt(OFFSET(1 + LINEPOS(3)));
			writePixelAt(OFFSET(2 + LINEPOS(3)));

			writePixelAt(OFFSET(2 + LINEPOS(4)));
			writePixelAt(OFFSET(2 + LINEPOS(5)));

			writePixelAt(OFFSET(2 + LINEPOS(5)));
			writePixelAt(OFFSET(1 + LINEPOS(5)));
			writePixelAt(OFFSET(LINEPOS(5)));

			break;
		}
		case 54://6:
		{
			writePixelAt(OFFSET(LINEPOS(1)));
			writePixelAt(OFFSET(1 + LINEPOS(1)));
			writePixelAt(OFFSET(2 + LINEPOS(1)));

			writePixelAt(OFFSET(LINEPOS(2)));
			writePixelAt(OFFSET(LINEPOS(3)));
			writePixelAt(OFFSET(LINEPOS(4)));
			writePixelAt(OFFSET(LINEPOS(5)));

			writePixelAt(OFFSET(1 + LINEPOS(3)));
			writePixelAt(OFFSET(2 + LINEPOS(3)));
			
			writePixelAt(OFFSET(2 + LINEPOS(4)));

			writePixelAt(OFFSET(1 + LINEPOS(5)));
			writePixelAt(OFFSET(2 + LINEPOS(5)));
			break;
		}
		case 55://7:
		{
			writePixelAt(OFFSET(LINEPOS(1)));
			writePixelAt(OFFSET(1 + LINEPOS(1)));
			writePixelAt(OFFSET(2 + LINEPOS(1)));

			writePixelAt(OFFSET(2 + LINEPOS(2)));
			writePixelAt(OFFSET(2 + LINEPOS(3)));
			writePixelAt(OFFSET(2 + LINEPOS(4)));
			writePixelAt(OFFSET(2 + LINEPOS(5)));

			writePixelAt(OFFSET(1 + LINEPOS(3)));
			break;
		}
		case 56://8:
		{
			writePixelAt(OFFSET(LINEPOS(1)));
			writePixelAt(OFFSET(1 + LINEPOS(1)));
			writePixelAt(OFFSET(2 + LINEPOS(1)));

			writePixelAt(OFFSET(LINEPOS(2)));
			writePixelAt(OFFSET(LINEPOS(2) + 2));
			writePixelAt(OFFSET(LINEPOS(3) + 1));
			writePixelAt(OFFSET(LINEPOS(4)));
			writePixelAt(OFFSET(LINEPOS(4) + 2));
			
			writePixelAt(OFFSET(LINEPOS(5)));
			writePixelAt(OFFSET(LINEPOS(5) + 1));
			writePixelAt(OFFSET(LINEPOS(5) + 2));
			break;
		}
		case 57://9:
		{
			writePixelAt(OFFSET(LINEPOS(1)));
			writePixelAt(OFFSET(1 + LINEPOS(1)));
			writePixelAt(OFFSET(2 + LINEPOS(1)));

			writePixelAt(OFFSET(LINEPOS(2)));
			writePixelAt(OFFSET(LINEPOS(3)));
			writePixelAt(OFFSET(LINEPOS(3) + 1));

			writePixelAt(OFFSET(2 + LINEPOS(2)));
			writePixelAt(OFFSET(2 + LINEPOS(3)));
			writePixelAt(OFFSET(2 + LINEPOS(4)));
			writePixelAt(OFFSET(2 + LINEPOS(5)));
			break;
		}
		case 88://'x':
		{
			writePixelAt(OFFSET(LINEPOS(1)));
			writePixelAt(OFFSET(2 + LINEPOS(1)));

			writePixelAt(OFFSET(LINEPOS(2)));
			writePixelAt(OFFSET(LINEPOS(2) + 2));
			writePixelAt(OFFSET(LINEPOS(3) + 1));
			writePixelAt(OFFSET(LINEPOS(4)));
			writePixelAt(OFFSET(LINEPOS(4) + 2));
			
			writePixelAt(OFFSET(LINEPOS(5)));
			writePixelAt(OFFSET(LINEPOS(5) + 2));
			break;
		}
		case 89://'y':
		{
			writePixelAt(OFFSET(LINEPOS(1)));
			writePixelAt(OFFSET(2 + LINEPOS(1)));

			writePixelAt(OFFSET(LINEPOS(2)));
			writePixelAt(OFFSET(LINEPOS(2) + 2));
			writePixelAt(OFFSET(LINEPOS(3) + 1));
			writePixelAt(OFFSET(LINEPOS(4) + 1));
			
			writePixelAt(OFFSET(LINEPOS(5) + 1));
			break;
		}
		case 59://';':
		{
			writePixelAt(OFFSET(2 + LINEPOS(1)));
			
			writePixelAt(OFFSET(1 + LINEPOS(3)));
			writePixelAt(OFFSET(1 + LINEPOS(4)));
			writePixelAt(OFFSET(2 + LINEPOS(3)));
			writePixelAt(OFFSET(2 + LINEPOS(4)));
			
			writePixelAt(OFFSET(LINEPOS(5)));
			break;
		}
		case 58://':':
		{
			writePixelAt(OFFSET(1 + LINEPOS(2)));
			writePixelAt(OFFSET(1 + LINEPOS(4)));
			break;
		}
		default:
		{
			writePixelAt(x + (y << 7));
			break;
		}
	}
}

//! Convenience macro for keeping track of our current position in the text buffer
#define CURRENT_BUFFER_CHAR (*(TEXT_BUFFER + idx))

//! Commits the text buffer to the graphics buffer
static inline void drawTextBuffer()
{
	uint16_t offset = 4;
	uint8_t yOffset = 0;
	for (uint8_t idx = 0; idx < strlen(TEXT_BUFFER); ++idx)
	{
		if(CURRENT_BUFFER_CHAR == '\n')
		{
			yOffset += DISPLAY_WIDTH;
		}
		
		drawNumber((idx * offset) + yOffset , 0, CURRENT_BUFFER_CHAR);
	}
}

//! Trims the string @p input up to the specified index
//! \p input the char array we want to trim
//! \p newIdx the new starting index of our string
void removeStringStart(char* input, uint8_t newIdx)
{
	if (strlen(input) <= newIdx)
	{
		return;
	}

	strcpy(input, input+newIdx);
}

//! Defines how the text buffer handles line overflows
enum E_TEXT_NEWLINE_BEHAVIOUR
{
	OVERFLOW_CUT_OLD = 0, //! Trim old traces if current line full or exceeded
	OVERFLOW_NEWLINE = 1  //! Write a new line if current line full or exceeded
} TEXTMODE_OPTIONS;

//! Set this to change line overflow behaviour
static uint8_t TEXTMODE_BEHAVIOUR = OVERFLOW_CUT_OLD;

//! Prints a message to screen
//! \p message The string to be written on screen
static void trace2Buffer(const char* message)
{
	if (strlen(TEXT_BUFFER) > 32)
	{
		if (TEXTMODE_BEHAVIOUR == OVERFLOW_CUT_OLD)
		{
			removeStringStart(TEXT_BUFFER, 9);
		}
		
		if (TEXTMODE_BEHAVIOUR == OVERFLOW_NEWLINE)
		{
			strcat(TEXT_BUFFER, "\n");
		}
	}
	strcat(TEXT_BUFFER, message);
}

//! Prints an int to the screen
//! \p num the int value to be printed
void traceInt(uint16_t num)
{
	char number[8] = {0, 0};
	char reversed[8] = {0};
	uint8_t n = 0;
	uint16_t idx = 0;
	uint8_t fidx = 0;
	
	if (num == 0)
	{
		number[0] = '0';
		trace2Buffer(number);
		return;
	}
	
	while(num > 0)
	{
		n = num % 10;
		reversed[idx] = '0' + n;
		num /= 10;
		idx++;
	}
	
	do {
		number[fidx] = reversed[idx - 1];
		idx--;
		fidx++;
	} while (idx > 0);
	
	trace2Buffer(number);
}

//! Reads a byte from the graphics buffer
//! \memIdx the buffer location pointing to the first bit of the byte being read
uint8_t getByte(const int32_t memIdx)
{
	uint8_t byte = 0;
	for (int bitIdx = 0; bitIdx < 8; ++bitIdx)
	{
		uint32_t locationIdx = memIdx + (bitIdx << 7);
		uint8_t bit = readPixelAt(locationIdx);
		byte |= bit << bitIdx;
	}
	
	return byte;
}

//! Sets the hardware cursor at the offset of the currently active draw sector
//! and clears the buffer
void setYSector()
{
	ssd1306_setpos(0, CURRENT_DRAW_SECTOR << 4);
	memset(GRAPHICS_BUFFER, 0, 256);
}

static uint8_t vSectorIdx = 0;
//! Blits the draw sector.
//! Writes the display buffer for a given draw sector to
//! memory then flushes the buffer.
void blit()
{
	#ifndef _DESKTOP
	ssd1306_send_data_start();
	#endif

	while(vSectorIdx < 16)
	{
		for (uint32_t memIdx = vSectorIdx << 7; memIdx < ((vSectorIdx + 1) << 7); ++memIdx)
		{
			uint8_t byte = getByte(memIdx);
			ssd1306_send_byte(byte);
		}
		vSectorIdx += 8;
	}

	#ifndef _DESKTOP
	ssd1306_send_data_stop();
	#endif
	vSectorIdx = 0;
}

//! Bitfield used to track sectors that are scheduled for clearing
static uint8_t DIRTY_SECTORS = 0;

//! Cleans the sector at @p sId index
//! \param sId the index of the sector to be cleaned
void cleanSector(uint8_t sId)
{
	if ((DIRTY_SECTORS & (1 << sId)) == (1 << sId))
	{
		ssd1306_send_data_start();
		for (uint16_t i = 0; i < 256; ++i)
		{
			ssd1306_send_byte(0b00000000);
		}
		ssd1306_send_data_stop();
		
		DIRTY_SECTORS ^= (1 << sId);
	}
}

//! Clears graphics and display buffers, resets origin
void cls()
{
	memset(GRAPHICS_BUFFER, 0, 256);
	cleanSector(0);
	cleanSector(1);
	cleanSector(2);
	cleanSector(3);
	//ssd1306_clear();
	ssd1306_setpos(0, CURRENT_DRAW_SECTOR << 4);
}

//! Marks the sector with the id @p sId as dirty
//! \param sId the id of the dirty sector
void setDirtySector(uint8_t sId)
{
	DIRTY_SECTORS |= 1 << sId;
}

#endif /* GRAPHICS_H_ */