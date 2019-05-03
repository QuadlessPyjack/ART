#ifndef PAYLOAD_H
#define PAYLOAD_H

#include <stdint.h>
#include <string.h>

enum E_PAYLOAD
{
    X_COORDINATE = 0,
    Y_COORDINATE = 1,
    ROTATION = 2,
    CHECKSUM = 3,
    PAYLOAD_SIZE = 4
};

struct Payload
{
    char data[PAYLOAD_SIZE];
};


static inline uint8_t calculateEvenParity(const char &input)
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

static inline bool isEvenParityValid(const Payload &input, int &faultIdx)
{
    uint8_t checksumIdx = PAYLOAD_SIZE - 1;//sizeof(input.data) - 1;
    for(uint8_t idx = 0; idx < checksumIdx; ++idx)
    {
        auto parity = calculateEvenParity(input.data[idx]);
        if ((input.data[checksumIdx] & (0x01 << idx)) != (parity << idx))
        {
            faultIdx = idx;
            return false;
        }
    }
    faultIdx = -1;
    return true;
}


static inline void setEvenParityByte(Payload &input)
{
    uint8_t xParity = calculateEvenParity(input.data[X_COORDINATE]);
    uint8_t yParity = calculateEvenParity(input.data[Y_COORDINATE]);
    uint8_t rParity = calculateEvenParity(input.data[ROTATION]);

    input.data[CHECKSUM] |= (xParity << X_COORDINATE) | (yParity << Y_COORDINATE) | (rParity << ROTATION);
}

#endif // PAYLOAD_H
