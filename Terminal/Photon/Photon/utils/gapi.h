/*
 * gapi.h
 *
 * Created: 15/04/2019 17:36:25
 *  Author: bogdan
 */ 

#ifndef GAPI_H_
#define GAPI_H_

#include "graphics.h"

//! Struct used to describe a graphical operation to be performed on the screen
typedef struct DrawCmd
{
	//! \warning UNUSED. If set, overrides previous draw commands with the same id
	uint8_t id;
	//! The x coordinate on the screen where the command should take effect
	uint8_t x;
	//! The y coordinate on the screen where the command should take effect
	uint8_t y;
	//! Used to determine what to execute when reading this command
	char type;
	//! Linked list pointer: move to previous command in the sector
	void *prev;
	//! Linked list pointer: move to next command in the sector
	void *next;
} DrawCmd;

#define DRAW_BUFFER_SIZE 4
//! Buffer size determines number of commands we can execute on the screen in one frame
static DrawCmd DRAW_COMMAND_BUFFER[DRAW_BUFFER_SIZE];

//! Initializes the graphics and command buffers
void gapiInit()
{
	initGfxBuffer();
	memset(DRAW_COMMAND_BUFFER, 0, sizeof(DrawCmd) * DRAW_BUFFER_SIZE);
}

//! Adds a new draw command to the sector list
//! \param dc the draw command to add
void gapiLoadCommand(DrawCmd dc)
{
	uint8_t sId = dc.y >> 4; // Sector height is currently 16
	if (sId != 0)
	{
		dc.y = dc.y % 16;
		if (dc.y < 4) return; // graphic overlaps two sectors - not supported
	}
	
	if (DRAW_COMMAND_BUFFER[sId].next == NULL)
	{
		// No command assigned to sector. Allocate and assign.
		
		DrawCmd *cmd = malloc(sizeof(dc));
		*cmd = dc;
		DRAW_COMMAND_BUFFER[sId].next = cmd;

		cmd->next = NULL;
		cmd->prev = &DRAW_COMMAND_BUFFER[sId];

		return;
	}
	
	DrawCmd *first = malloc(sizeof(DrawCmd));
	*first = *((DrawCmd*)DRAW_COMMAND_BUFFER[sId].next);
	
	free(DRAW_COMMAND_BUFFER[sId].next);
	
	DrawCmd *new = malloc(sizeof(dc));
	*new = dc;
	
	DRAW_COMMAND_BUFFER[sId].next = new; // void* can point to anything, cast not needed
	new->next = first;
	new->prev = &DRAW_COMMAND_BUFFER[sId];

	first->prev = new;
}

//! Interprets the DrawCmd type field and executes the corresponding graphics routine
//! To be replaced with void ptr
void interpretDrawCommand(const DrawCmd dc)
{
	switch (dc.type)
		{
			case 't': // draw target command
			{
				drawTarget(dc.x, dc.y);
				break;
			}
			case 'm': // draw marker command
			{
				drawMarker(dc.x, dc.y);
				break;
			}
			case 0: // NOP command
			{
				break;
			}
			default:
			break;
		}
}

//! API update loop
//! processes the command buffer and calls relevant draw functions
void gapiUpdate()
{
	for (uint8_t idx = 0; idx < DRAW_BUFFER_SIZE; ++idx)
	{
		DrawCmd *dcPtr = DRAW_COMMAND_BUFFER[idx].next;
		DrawCmd *prev = NULL;
		DrawCmd *toBeRemoved = NULL;

		if (dcPtr == NULL)
		{
			continue;
		}
		
		if (CURRENT_DRAW_SECTOR != idx)
		{
			CURRENT_DRAW_SECTOR = idx;
			setDirtySector(idx);
			setYSector();
		}

		do
		{
			interpretDrawCommand(*dcPtr);
			// Get the pointers we need to free this node
			toBeRemoved = dcPtr;
			prev = dcPtr->prev;

			// Advance pointer to next node
			dcPtr = dcPtr->next;
			
			if (dcPtr == NULL)
			{
				prev->next = NULL;
				free(toBeRemoved);
				break;
			}

			// Update links
			prev->next = dcPtr;
			dcPtr->prev = prev;

			// Free node
			free(toBeRemoved);
		} while(dcPtr->next != NULL);
		blit();
	}
}

//! Wrapper for drawTarget()
//! \param x X coordinate for the target
//! \param y Y coordinate for the target
void gapiDrawTarget(const uint8_t x, const uint8_t y)
{
	DrawCmd dc;
	dc.x = x;
	dc.y = y;
	dc.type = 't';
	dc.id = 1;
	
	gapiLoadCommand(dc);
}

//! Wrapper for drawMarker()
//! \param x X coordinate for the marker
//! \param y Y coordinate for the marker
void gapiDrawMarker(const uint8_t x, const uint8_t y)
{
	DrawCmd dc;
	dc.x = x;
	dc.y = y;
	dc.type = 'm';
	dc.id = 2;
	
	gapiLoadCommand(dc);
}

#endif /* GAPI_H_ */