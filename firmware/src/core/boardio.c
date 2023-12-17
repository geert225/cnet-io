#include <boardio.h>

#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>

void boardio_setup_inputs(void){
    rcc_periph_clock_enable(RCC_GPIOA);
    //rcc_periph_clock_enable(RCC_GPIOB);
    //rcc_periph_clock_enable(RCC_GPIOC);
    //rcc_periph_clock_enable(RCC_GPIOD);
    gpio_mode_setup(INPUT_BANK_1_A_PORT, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, INPUT_BANK_1_A_PIN);
    gpio_mode_setup(INPUT_BANK_1_B_PORT, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, INPUT_BANK_1_B_PIN);
    gpio_mode_setup(INPUT_BANK_1_C_PORT, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, INPUT_BANK_1_C_PIN);
    gpio_mode_setup(INPUT_BANK_1_D_PORT, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, INPUT_BANK_1_D_PIN);
    gpio_mode_setup(INPUT_BANK_1_E_PORT, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, INPUT_BANK_1_E_PIN);
    gpio_mode_setup(INPUT_BANK_1_F_PORT, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, INPUT_BANK_1_F_PIN);
    gpio_mode_setup(INPUT_BANK_1_G_PORT, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, INPUT_BANK_1_G_PIN);
    gpio_mode_setup(INPUT_BANK_1_H_PORT, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, INPUT_BANK_1_H_PIN);
}
//todo
uint16_t boardio_read_inputs(){
    uint16_t retval = 0;
    retval |= gpio_get(INPUT_BANK_1_A_PORT,INPUT_BANK_1_A_PIN);
    return retval;
}