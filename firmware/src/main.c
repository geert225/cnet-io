#include <commendef.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/spi.h>
#include <core/clock.h>
#include <string.h>
#include <oled/oled.h>

#define PORT_SPI5 GPIOF 
#define PIN_SPI5_SCK GPIO7
#define PIN_SPI5_MISO GPIO8
#define PIN_SPI5_MOSI GPIO9

#define PORT_OLED 	GPIOE
#define PIN_DS 		GPIO5 
#define PIN_CS 		GPIO6 
#define PIN_RESET 	GPIO3 

static void gpio_port_w_bit(uint32_t port, uint16_t pin, uint16_t value){
	uint16_t portData = gpio_port_read(port);
	value = value & 1;
	if (value == 1){
		portData = portData | pin;
	}else{
		portData = portData & ~(pin);
	}
	gpio_port_write(port, portData);
}


int main(void)
{
	uint8_t binCount = 0;
	uint64_t NextTime = 0;
	system_setup();
	rcc_periph_clock_enable(RCC_GPIOB);
	gpio_mode_setup(GPIO_PORT_B_BASE,GPIO_MODE_OUTPUT,GPIO_PUPD_NONE,GPIO7 | GPIO0 | GPIO14);

	rcc_periph_clock_enable(RCC_GPIOB);
	rcc_periph_clock_enable(RCC_GPIOF);
	rcc_periph_clock_enable(RCC_GPIOE);
	rcc_periph_clock_enable(RCC_SPI5);

	gpio_mode_setup(PORT_SPI5,GPIO_MODE_AF,GPIO_PUPD_NONE, PIN_SPI5_SCK | PIN_SPI5_MISO | PIN_SPI5_MOSI);
	gpio_mode_setup(PORT_OLED,GPIO_MODE_OUTPUT,GPIO_PUPD_NONE,PIN_DS | PIN_CS | PIN_RESET);
	gpio_set_af(PORT_SPI5, GPIO_AF5, PIN_SPI5_SCK | PIN_SPI5_MISO | PIN_SPI5_MOSI);
	
	spi_init_master(SPI5, SPI_CR1_BAUDRATE_FPCLK_DIV_8, SPI_CR1_CPOL_CLK_TO_0_WHEN_IDLE, SPI_CR1_CPHA_CLK_TRANSITION_1, SPI_CR1_DFF_8BIT, SPI_CR1_MSBFIRST);
	spi_enable_ss_output(SPI5);
	spi_enable(SPI5);

	oledInit();
	oledFill();
	oledRefresh();
	oledFillPattern(170);

	NextTime = system_get_ticks() + 1000;
    while (1)
	{
		
		if (system_get_ticks() >= NextTime){
			binCount++;
			if(binCount > 7){
				binCount = 0;
			}
			NextTime = system_get_ticks() + 1000;
			oledRefresh();
		}
		gpio_port_w_bit(GPIO_PORT_B_BASE, GPIO0, ((binCount) & 1));
		gpio_port_w_bit(GPIO_PORT_B_BASE, GPIO7, ((binCount >> 1) & 1));
		gpio_port_w_bit(GPIO_PORT_B_BASE, GPIO14, ((binCount >> 2) & 1));		
	}
	return 0;
}
