#include <oled/oled.h>
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

void oledDrawPixel(int x, int y) {
  if ((x < 0) || (y < 0) || (x >= OLED_WIDTH) || (y >= OLED_HEIGHT)) {
    return;
  }
  oledBuffer[OLED_OFFSET(x, y)] |= OLED_MASK(x, y);
}

void oledClearPixel(int x, int y) {
  if ((x < 0) || (y < 0) || (x >= OLED_WIDTH) || (y >= OLED_HEIGHT)) {
    return;
  }
  oledBuffer[OLED_OFFSET(x, y)] &= ~OLED_MASK(x, y);
}

void oledInvertPixel(int x, int y) {
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

void oledFill() { 
    memset(oledBuffer, 255, sizeof(oledBuffer));
}

void oledClear() { 
    memset(oledBuffer, 0, sizeof(oledBuffer));
}

void oledFillPattern(uint8_t pattern) { 
    memset(oledBuffer, pattern, sizeof(oledBuffer));
}


void oledRefresh() {
    uint8_t pageCount = (OLED_HEIGHT / 8);
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

void oledInit() {
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
                                OLED_SEGREMAP | 0x01,
                                OLED_COMSCANDEC,
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

  oledClear();
  oledRefresh();
}