#ifndef _CNETIO_OLED_
#define _CNETIO_OLED_

#include <commendef.h>

#define OLED_WIDTH 132
#define OLED_HEIGHT 64
#define OLED_BUFSIZE (OLED_WIDTH * OLED_HEIGHT / 8)

void oledInit(void);
void oledClear(void);
void oledFill(void);
void oledFillPattern(uint8_t pattern);
void oledRefresh(void);

#endif