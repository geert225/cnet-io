#ifndef _CNETIO_OLED_
#define _CNETIO_OLED_

#include <commendef.h>
#include <drivers/oled/font.h>

#define OLED_WIDTH 132
#define OLED_HEIGHT 64
#define OLED_XSTART 4
#define OLED_YSTART 1
#define OLED_BUFSIZE (OLED_WIDTH * OLED_HEIGHT / 8)

typedef struct 
{
    uint32_t spiPeriph;
    uint16_t mosiPin;
    uint16_t sckPin;
    uint16_t chipSelectPin;
    uint16_t dataTypePin;
    uint16_t resetPin;
    uint32_t mosiPort;
    uint32_t sckPort;
    uint32_t chipSelectPort;
    uint32_t datatypePort;
    uint32_t resetPort;
    uint8_t dataBuffer[OLED_BUFSIZE];
} oledHandle_t;

void oled_draw_pixel(oledHandle_t * handle, int x, int y);
void oled_clear_pixel(oledHandle_t * handle, int x, int y);
void oled_invert_pixel(oledHandle_t * handle, int x, int y);

void oled_init(oledHandle_t * handle, uint32_t spiPeriph, uint16_t mosiPin, uint16_t sckPin, uint16_t chipSelectPin, uint16_t dataTypePin, uint16_t resetPin, uint32_t mosiPort, uint32_t sckPort, uint32_t chipSelectPort, uint32_t datatypePort, uint32_t resetPort);
void oled_clear(oledHandle_t * handle);
void oled_fill(oledHandle_t * handle);
void oled_fill_pattern(oledHandle_t * handle, uint8_t pattern);
void oled_refresh(oledHandle_t * handle);
int oled_write_char(oledHandle_t * handle, char ch, int x, int y, FontDef_t* Font, uint8_t invert);
void oled_write_string(oledHandle_t * handle, const char *text, int x, int y, FontDef_t* Font, uint8_t invert);

#endif