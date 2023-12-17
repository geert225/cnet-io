#ifndef CNET_PIN_DEF_H
#define CNET_PIN_DEF_H

#include <libopencm3\stm32\gpio.h>

//alle banken max 8 bits

#define INPUT_BANK_1_A_PORT   GPIOA
#define INPUT_BANK_1_A_PIN    GPIO0
#define INPUT_BANK_1_B_PORT   GPIOA
#define INPUT_BANK_1_B_PIN    GPIO1
#define INPUT_BANK_1_C_PORT   GPIOA
#define INPUT_BANK_1_C_PIN    GPIO2
#define INPUT_BANK_1_D_PORT   GPIOA
#define INPUT_BANK_1_D_PIN    GPIO3
#define INPUT_BANK_1_E_PORT   GPIOA
#define INPUT_BANK_1_E_PIN    GPIO4
#define INPUT_BANK_1_F_PORT   GPIOA
#define INPUT_BANK_1_F_PIN    GPIO5
#define INPUT_BANK_1_G_PORT   GPIOA
#define INPUT_BANK_1_G_PIN    GPIO6
#define INPUT_BANK_1_H_PORT   GPIOA
#define INPUT_BANK_1_H_PIN    GPIO7

#define RLY_BANK_1_A_PORT   GPIOA
#define RLY_BANK_1_A_PIN    GPIO8
#define RLY_BANK_1_B_PORT   GPIOA
#define RLY_BANK_1_B_PIN    GPIO9
#define RLY_BANK_1_C_PORT   GPIOA
#define RLY_BANK_1_C_PIN    GPIO10
#define RLY_BANK_1_D_PORT   GPIOA
#define RLY_BANK_1_D_PIN    GPIO11

#define PWM_BANK_1_A_PORT
#define PWM_BANK_1_A_PIN
#define PWM_BANK_1_B_PORT
#define PWM_BANK_1_B_PIN

#define ARGB_BANK_1_A_PORT
#define ARGB_BANK_1_A_PIN
#define ARGB_BANK_1_B_PORT
#define ARGB_BANK_1_B_PIN

#endif // CNET_PIN_DEF_H
