#include <drivers/tm1637/tm1637.h>

void tm1637_init(tm1637Handle_t * handle, uint16_t dataPin, uint16_t clockPin, uint32_t dataPort, uint32_t clockPort){
    handle->clockPin = clockPin;
    handle->clockPort = clockPort;
    handle->dataPin = dataPin;
    handle->dataPort = dataPort;
    for (int i = 0; i < 6; i++)
    {
        handle->dataBuffer[i] = 0;
    }

    //init pins
}