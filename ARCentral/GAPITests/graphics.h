#ifndef GRAPHICS_H
#define GRAPHICS_H

//! Mock header for testing GAPI

#include <stdlib.h>
#include <memory.h>

#include <stdint.h>
#include <stdio.h>

void drawTarget(uint8_t x, uint8_t y)
{
    printf("T: %d", x);
    printf(" %d \n", y);
}
void drawMarker(uint8_t x, uint8_t y)
{
    printf("M: %d", x);
    printf(" %d \n", y);
}

#endif // GRAPHICS_H
