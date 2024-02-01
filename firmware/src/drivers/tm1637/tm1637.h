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

void tm1637_init(tm1637Handle_t * handle, uint16_t dataPin, uint16_t clockPin, uint32_t dataPort, uint32_t clockPort);
void tm1637_clear(tm1637Handle_t * handle);
void tm1637_led_test(tm1637Handle_t * handle);
void tm1637_refresh(tm1637Handle_t * handle);
void tm1637_display_on(tm1637Handle_t * handle);
void tm1637_display_off(tm1637Handle_t * handle);

#endif