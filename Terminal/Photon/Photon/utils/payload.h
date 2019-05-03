/*
 * payload.h
 *
 * Created: 06/03/2019 17:18:36
 *  Author: bogdan
 */ 


#ifndef PAYLOAD_H_
#define PAYLOAD_H_

#include <stdint.h>
#include <string.h>

#include "../consts.h"

//! Byte indexes for the Payload
enum E_PAYLOAD
{
	X_COORDINATE = 0, //! the X coordinate byte of the packet
	Y_COORDINATE = 1, //! the Y coordinate byte of the packet
	ROTATION = 2,     //! the Z/rotation coordinate byte of the packet
	METADATA = 3,     //! the metadata byte of the packet
	PAYLOAD_SIZE = 4  //! the size of the payload struct
};

//! The struct used to read data from BLE
typedef struct Payload
{
	char data[PAYLOAD_SIZE];
} Payload;

//! Calculates the even parity for the given byte
//! \param input The byte for which to calculate the even parity
static inline uint8_t calculateEvenParity(const char input)
{
	uint8_t setBitCount = 0;
	for (uint8_t bitIdx = 0; bitIdx < 8; ++bitIdx)
	{
		if ((input & (0x01 << bitIdx)) == (0x01 << bitIdx))
		{
			++setBitCount;
		}
	}

	return (setBitCount % 2 == 0) ? 0 : 1;
}

//! Checks if all parity bits in the payload are valid
//! \param input The payload to check
//! \param faultIdx The location of the faulty byte or -1 if no fault found
//! \return TRUE if all parity bits are valid, FALSE otherwise
static inline int isEvenParityValid(const Payload* input, int* faultIdx)
{
	uint8_t checksumIdx = METADATA;//sizeof(input.data) - 1;
	for(uint8_t idx = 0; idx < checksumIdx; ++idx)
	{
		uint8_t parity = calculateEvenParity(input->data[idx]);
		if ((input->data[checksumIdx] & (0x01 << idx)) != (parity << idx))
		{
			*faultIdx = idx;
			return FALSE;
		}
	}
	*faultIdx = -1;
	return TRUE;
}

//! Sets the parity bits for the given payload
//! \param input The payload for which to generate the parity bits
static inline void setEvenParityByte(Payload* input)
{
	uint8_t xParity = calculateEvenParity(input->data[X_COORDINATE]);
	uint8_t yParity = calculateEvenParity(input->data[Y_COORDINATE]);
	uint8_t rParity = calculateEvenParity(input->data[ROTATION]);

	input->data[METADATA] |= (xParity << X_COORDINATE) | (yParity << Y_COORDINATE) | (rParity << ROTATION);
}

#endif /* PAYLOAD_H_ */