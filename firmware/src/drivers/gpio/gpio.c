#include <drivers/gpio/gpio.h>
#include <libopencm3/stm32/gpio.h>

void gpio_write_bit(uint32_t gpioport, uint16_t gpios, uint16_t value){
    (value == 0) ? gpio_clear(gpioport, gpios) : gpio_set(gpioport, gpios);
}