#ifndef _CNETIO_OLED_
#define _CNETIO_OLED_

#include <commendef.h>
#include <drivers/oled/font.h>

#define OLED_WIDTH 132
#define OLED_HEIGHT 64
#define OLED_XSTART 4
#define OLED_YSTART 1
#define OLED_BUFSIZE (OLED_WIDTH * OLED_HEIGHT / 8)

void oled_draw_pixel(int x, int y);
void oled_clear_pixel(int x, int y);
void oled_invert_pixel(int x, int y);

void oled_init(void);
void oled_clear(void);
void oled_fill(void);
void oled_fill_pattern(uint8_t pattern);
void oled_refresh(void);
int oled_write_char(char ch, int x, int y, FontDef_t* Font, uint8_t invert);
void oled_write_string(const char *text, int x, int y, FontDef_t* Font, uint8_t invert);

#endif