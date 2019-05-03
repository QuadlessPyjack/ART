/*
 * gapi.h
 *
 * Created: 15/04/2019 17:36:25
 *  Author: bogdan
 */ 

#ifndef GAPI_H_
#define GAPI_H_

#include "graphics.h"

typedef struct DrawCmd
{
	uint8_t id;
	uint8_t x;
	uint8_t y;
	char type;
	void *prev;
	void *next;
} DrawCmd;

#define DRAW_BUFFER_SIZE 4
//! Buffer size determines number of commands we can fit on the screen
static DrawCmd DRAW_COMMAND_BUFFER[DRAW_BUFFER_SIZE];

void gapiInit()
{
    //initGfxBuffer();
	memset(DRAW_COMMAND_BUFFER, 0, sizeof(DrawCmd) * DRAW_BUFFER_SIZE);
}

//! Adds a new draw command to the sector list
void gapiLoadCommand(DrawCmd dc)
{
	uint8_t sId = dc.y >> 4; // Sector height is currently 16
	
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
	}
}

void gapiDrawTarget(const uint8_t x, const uint8_t y)
{
	DrawCmd dc;
	dc.x = x;
	dc.y = y;
	dc.type = 't';
	dc.id = 1;
	
	gapiLoadCommand(dc);
}

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
