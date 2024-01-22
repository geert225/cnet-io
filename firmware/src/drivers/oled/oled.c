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

#define OLED_OFFSET(x, y) (OLED_BUFSIZE - 1 - (x) - ((y) / 8) * OLED_WIDTH)
#define OLED_MASK(x, y) (1 << (7 - (y) % 8))

void oled_draw_pixel(oledHandle_t * handle, int x, int y) {
  if ((x < 0) || (y < 0) || (x >= OLED_WIDTH) || (y >= OLED_HEIGHT)) {
    return;
  }
  handle->dataBuffer[OLED_OFFSET(x, y)] |= OLED_MASK(x, y);
}

void oled_clear_pixel(oledHandle_t * handle, int x, int y) {
  if ((x < 0) || (y < 0) || (x >= OLED_WIDTH) || (y >= OLED_HEIGHT)) {
    return;
  }
  handle->dataBuffer[OLED_OFFSET(x, y)] &= ~OLED_MASK(x, y);
}

void oled_invert_pixel(oledHandle_t * handle, int x, int y) {
  if ((x < 0) || (y < 0) || (x >= OLED_WIDTH) || (y >= OLED_HEIGHT)) {
    return;
  }
  handle->dataBuffer[OLED_OFFSET(x, y)] ^= OLED_MASK(x, y);
}

static inline void oled_spi_send(uint32_t base, const uint8_t *data, int len) {
  system_delay(1);
  for (int i = 0; i < len; i++) {
    spi_send(base, data[i]);
  }
  while (!(SPI_SR(base) & SPI_SR_TXE))
    ;
  while ((SPI_SR(base) & SPI_SR_BSY))
    ;
}

void oled_fill(oledHandle_t * handle) { 
    memset(handle->dataBuffer, 255, sizeof(handle->dataBuffer));
}

void oled_clear(oledHandle_t * handle) { 
    memset(handle->dataBuffer, 0, sizeof(handle->dataBuffer));
}

void oled_fill_pattern(oledHandle_t * handle, uint8_t pattern) { 
    memset(handle->dataBuffer, pattern, sizeof(handle->dataBuffer));
}


void oled_refresh(oledHandle_t * handle) {
    for(int i = 0; i < 8; i++){
        uint8_t s[3] = {0xB0 +  i,
                    0x00 | 2,
                    0x10};
        gpio_clear(handle->chipSelectPort, handle->chipSelectPin);  // SPI select
        oled_spi_send(handle->spiPeriph, s, 3);
        gpio_set(handle->chipSelectPort, handle->chipSelectPin);  // SPI deselect

        gpio_set(handle->datatypePort, handle->dataTypePin);    // set to DATA
        gpio_clear(handle->chipSelectPort, handle->chipSelectPin);  // SPI select
        oled_spi_send(handle->spiPeriph, &handle->dataBuffer[OLED_WIDTH * i], OLED_WIDTH);
        gpio_set(handle->chipSelectPort, handle->chipSelectPin);    // SPI deselect
        gpio_clear(handle->datatypePort, handle->dataTypePin);  // set to CMD
    }
}

void oled_init(oledHandle_t * handle, uint32_t spiPeriph, uint16_t mosiPin, uint16_t sckPin, uint16_t chipSelectPin, uint16_t dataTypePin, uint16_t resetPin, uint32_t mosiPort, uint32_t sckPort, uint32_t chipSelectPort, uint32_t datatypePort, uint32_t resetPort) {
  handle->spiPeriph = spiPeriph;
  handle->mosiPin = mosiPin;
  handle->sckPin = sckPin;
  handle->chipSelectPin = chipSelectPin;
  handle->dataTypePin = dataTypePin;
  handle->resetPin = resetPin;
  handle->mosiPort = mosiPort;
  handle->sckPort = sckPort;
  handle->chipSelectPort = chipSelectPort;
  handle->datatypePort = datatypePort;
  handle->resetPort = resetPort;
  memset(handle->dataBuffer, 0, sizeof(handle->dataBuffer));

  gpio_mode_setup(handle->mosiPort,GPIO_MODE_AF,GPIO_PUPD_NONE, handle->mosiPin);
  gpio_set_af(handle->mosiPort, GPIO_AF5, handle->mosiPin);
  gpio_mode_setup(handle->sckPort,GPIO_MODE_AF,GPIO_PUPD_NONE, handle->sckPin);
  gpio_set_af(handle->sckPort, GPIO_AF5, handle->sckPin);

	gpio_mode_setup(handle->chipSelectPort,GPIO_MODE_OUTPUT,GPIO_PUPD_NONE,handle->chipSelectPin);
  gpio_mode_setup(handle->datatypePort,GPIO_MODE_OUTPUT,GPIO_PUPD_NONE,handle->dataTypePin);
  gpio_mode_setup(handle->resetPort,GPIO_MODE_OUTPUT,GPIO_PUPD_NONE,handle->resetPin);

  spi_init_master(handle->spiPeriph, SPI_CR1_BAUDRATE_FPCLK_DIV_8, SPI_CR1_CPOL_CLK_TO_0_WHEN_IDLE, SPI_CR1_CPHA_CLK_TRANSITION_1, SPI_CR1_DFF_8BIT, SPI_CR1_MSBFIRST);
	spi_enable_ss_output(handle->spiPeriph);
	spi_enable(handle->spiPeriph);
  system_delay(1);

  //init structure for display
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

  gpio_clear(handle->datatypePort, handle->dataTypePin);  // set to CMD
  gpio_set(handle->chipSelectPort, handle->chipSelectPin);    // SPI deselect

  // Reset the LCD
  gpio_set(handle->resetPort, handle->resetPin);
  system_delay(40);
  gpio_clear(handle->resetPort, handle->resetPin);
  system_delay(400);
  gpio_set(handle->resetPort, handle->resetPin);

  // init
  gpio_clear(handle->chipSelectPort, handle->chipSelectPin);  // SPI select
  oled_spi_send(handle->spiPeriph, s, 25);
  gpio_set(handle->chipSelectPort, handle->chipSelectPin);  // SPI deselect
  oled_refresh(handle);
}

int oled_write_char(oledHandle_t * handle, char ch, int x, int y, FontDef_t* Font, uint8_t invert) {
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
                    oled_draw_pixel(handle, x + j, y + i);
                }else{
                    oled_clear_pixel(handle, x + j, y + i);
                }
                
			} else {
				if (invert == 0x01){
                    oled_draw_pixel(handle, x + j, y + i);
                }else{
                    oled_clear_pixel(handle, x + j, y + i);
                }
			}
		}
	}
    return (x + Font->FontWidth);
}

void oled_write_string(oledHandle_t * handle, const char *text, int x, int y, FontDef_t* Font, uint8_t invert){
    int nextX = x;
    for (; *text; text++) {
        nextX = oled_write_char(handle, *text, nextX, y, Font, invert);
        if (nextX == - 1){
            return;
        }
    }
}