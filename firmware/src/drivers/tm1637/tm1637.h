#ifndef _CNETIO_TM1637_
#define _CNETIO_TM1637_

#include <commendef.h>

typedef struct 
{
    uint16_t dataPin;
    uint16_t clockPin;
    uint32_t dataPort;
    uint32_t clockPort;
    uint8_t dataBuffer[6];
} tm1637Handle_t;

#endif