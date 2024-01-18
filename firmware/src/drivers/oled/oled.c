#include <drivers/oled/oled.h>
#include <drivers/oled/font.h>
#include <string.h>
#include <core/clock.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/spi.h>

#define SPI_BASE SPI5
#define OLED_DC_PORT GPIOE
#define OLED_DC_PIN GPIO5  // PB0 | Data/Command
#define OLED_CS_PORT GPIOE
#define OLED_CS_PIN GPIO6  // PA4 | SPI Select
#define OLED_RST_PORT GPIOE
#define OLED_RST_PIN GPIO3  // PB1 | Reset display

#define OLED_SETCONTRAST 0x81
#define OLED_DISPLAYALLON_RESUME 0xA4
#define OLED_DISPLAYALLON 0xA5
#define OLED_NORMALDISPLAY 0xA6
#define OLED_INVERTDISPLAY 0xA7
#define OLED_DISPLAYOFF 0xAE
#define OLED_DISPLAYON 0xAF
#define OLED_SETDISPLAYOFFSET 0xD3
#define OLED_SETCOMPINS 0xDA
#define OLED_SETVCOMDETECT 0xDB
#define OLED_SETDISPLAYCLOCKDIV 0xD5
#define OLED_SETPRECHARGE 0xD9
#define OLED_SETMULTIPLEX 0xA8
#define OLED_SETLOWCOLUMN 0x00
#define OLED_SETHIGHCOLUMN 0x10
//#define OLED_SETHIGHCOLUMN 0x00
#define OLED_SETSTARTLINE 0x40
//#define OLED_SETSTARTLINE 0x10
#define OLED_MEMORYMODE 0x20
#define OLED_COMSCANINC 0xC0
#define OLED_COMSCANDEC 0xC8
#define OLED_SEGREMAP 0xA0
#define OLED_CHARGEPUMP 0x8D

static uint8_t oledBuffer[OLED_BUFSIZE];

#define OLED_OFFSET(x, y) (OLED_BUFSIZE - 1 - (x) - ((y) / 8) * OLED_WIDTH)
#define OLED_MASK(x, y) (1 << (7 - (y) % 8))

void oled_draw_pixel(int x, int y) {
  if ((x < 0) || (y < 0) || (x >= OLED_WIDTH) || (y >= OLED_HEIGHT)) {
    return;
  }
  oledBuffer[OLED_OFFSET(x, y)] |= OLED_MASK(x, y);
}

void oled_clear_pixel(int x, int y) {
  if ((x < 0) || (y < 0) || (x >= OLED_WIDTH) || (y >= OLED_HEIGHT)) {
    return;
  }
  oledBuffer[OLED_OFFSET(x, y)] &= ~OLED_MASK(x, y);
}

void oled_invert_pixel(int x, int y) {
  if ((x < 0) || (y < 0) || (x >= OLED_WIDTH) || (y >= OLED_HEIGHT)) {
    return;
  }
  oledBuffer[OLED_OFFSET(x, y)] ^= OLED_MASK(x, y);
}

static inline void SPISend(uint32_t base, const uint8_t *data, int len) {
  system_delay(1);
  for (int i = 0; i < len; i++) {
    spi_send(base, data[i]);
  }
  while (!(SPI_SR(base) & SPI_SR_TXE))
    ;
  while ((SPI_SR(base) & SPI_SR_BSY))
    ;
}

void oled_fill() { 
    memset(oledBuffer, 255, sizeof(oledBuffer));
}

void oled_clear() { 
    memset(oledBuffer, 0, sizeof(oledBuffer));
}

void oled_fill_pattern(uint8_t pattern) { 
    memset(oledBuffer, pattern, sizeof(oledBuffer));
}


void oled_refresh() {
    for(int i = 0; i < 8; i++){
        uint8_t s[3] = {0xB0 +  i,
                    0x00 | 2,
                    0x10};

        gpio_clear(OLED_CS_PORT, OLED_CS_PIN);  // SPI select
        SPISend(SPI_BASE, s, 3);
        gpio_set(OLED_CS_PORT, OLED_CS_PIN);  // SPI deselect

        gpio_set(OLED_DC_PORT, OLED_DC_PIN);    // set to DATA
        gpio_clear(OLED_CS_PORT, OLED_CS_PIN);  // SPI select
        SPISend(SPI_BASE, &oledBuffer[OLED_WIDTH * i], OLED_WIDTH);
        gpio_set(OLED_CS_PORT, OLED_CS_PIN);    // SPI deselect
        gpio_clear(OLED_DC_PORT, OLED_DC_PIN);  // set to CMD
    }
    
}

void oled_init() {
  static const uint8_t s[25] = {OLED_DISPLAYOFF,
                                OLED_SETDISPLAYCLOCKDIV,
                                0x80,
                                OLED_SETMULTIPLEX,
                                0x3F,  // 128x64
                                OLED_SETDISPLAYOFFSET,
                                0x00,
                                OLED_SETSTARTLINE | 0x00,
                                OLED_CHARGEPUMP,
                                0x14,
                                OLED_MEMORYMODE,
                                0x00,
                                OLED_SEGREMAP | 0x00,
                                OLED_COMSCANINC,
                                OLED_SETCOMPINS,
                                0x12,  // 128x64
                                OLED_SETCONTRAST,
                                0xCF,
                                OLED_SETPRECHARGE,
                                0xF1,
                                OLED_SETVCOMDETECT,
                                0x40,
                                OLED_DISPLAYALLON_RESUME,
                                OLED_NORMALDISPLAY,
                                OLED_DISPLAYON};

  gpio_clear(OLED_DC_PORT, OLED_DC_PIN);  // set to CMD
  gpio_set(OLED_CS_PORT, OLED_CS_PIN);    // SPI deselect

  // Reset the LCD
  gpio_set(OLED_RST_PORT, OLED_RST_PIN);
  system_delay(40);
  gpio_clear(OLED_RST_PORT, OLED_RST_PIN);
  system_delay(400);
  gpio_set(OLED_RST_PORT, OLED_RST_PIN);

  // init
  gpio_clear(OLED_CS_PORT, OLED_CS_PIN);  // SPI select
  SPISend(SPI_BASE, s, 25);
  gpio_set(OLED_CS_PORT, OLED_CS_PIN);  // SPI deselect

  oled_clear();
  oled_refresh();
}

int oled_write_char(char ch, int x, int y, FontDef_t* Font, uint8_t invert) {
	uint32_t i, b, j;
    invert &= 0x01;
	
	if (
		OLED_WIDTH <= (x + Font->FontWidth) ||
		OLED_HEIGHT <= (y + Font->FontHeight)
	) {
		return -1;
	}
	
	/* Go through font */
	for (i = 0; i < Font->FontHeight; i++) {
		b = Font->data[(ch - 32) * Font->FontHeight + i];
		for (j = 0; j < Font->FontWidth; j++) {
			if ((b << j) & 0x8000) {
                if (invert == 0x00){
                    oled_draw_pixel(x + j, y + i);
                }else{
                    oled_clear_pixel(x + j, y + i);
                }
                
			} else {
				if (invert == 0x01){
                    oled_draw_pixel(x + j, y + i);
                }else{
                    oled_clear_pixel(x + j, y + i);
                }
			}
		}
	}
    return (x + Font->FontWidth);
}

void oled_write_string(const char *text, int x, int y, FontDef_t* Font, uint8_t invert){
    int nextX = x;
    for (; *text; text++) {
        nextX = oled_write_char(*text, nextX, y, Font, invert);
        if (nextX == - 1){
            return;
        }
    }
}