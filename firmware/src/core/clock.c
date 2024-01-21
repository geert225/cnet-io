#include "core/clock.h"

#include <libopencm3/cm3/systick.h>
#include <libopencm3/cm3/vector.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/syscfg.h>

#define SYSCFG_PMC_MII_RMII_SEL (0x1UL << (23U)) /*!< 0x00800000 */

static volatile uint64_t ticks = 0;

void sys_tick_handler(void) {
  ticks++;
}

static void rcc_setup(void) {
  rcc_clock_setup_pll(&rcc_hsi_configs[RCC_CLOCK_3V3_180MHZ]);
}

static void rcc_periph_setup(void){
  rcc_periph_clock_enable(RCC_SYSCFG);

	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_GPIOB);
	rcc_periph_clock_enable(RCC_GPIOC);
	rcc_periph_clock_enable(RCC_GPIOD);
	rcc_periph_clock_enable(RCC_GPIOE);
	rcc_periph_clock_enable(RCC_GPIOF);
	rcc_periph_clock_enable(RCC_GPIOG);

	rcc_periph_clock_enable(RCC_SPI5);

	rcc_periph_clock_enable(RCC_ETHMAC);
	rcc_periph_clock_enable(RCC_ETHMACRX);
	rcc_periph_clock_enable(RCC_ETHMACTX);

	rcc_periph_clock_enable(RCC_DMA1);
	rcc_periph_clock_enable(RCC_DMA2);
}

static void syscfg_pmc_setup(void){
  //SYSCFG_PMC &= ~(SYSCFG_PMC_MII_RMII_SEL); // set eth in MII
	SYSCFG_PMC |= SYSCFG_PMC_MII_RMII_SEL; // set eth in RMII
}

static void systick_setup(void) {
  systick_set_frequency(SYSTICK_FREQ, CPU_FREQ);
  systick_counter_enable();
  systick_interrupt_enable();
}

uint64_t system_get_ticks(void) {
  return ticks;
}

void system_setup(void) {
  rcc_setup();
  systick_setup();
  rcc_periph_setup();
  syscfg_pmc_setup();
}

void system_delay(uint64_t milleseconds) {
  uint64_t end_time = system_get_ticks() + milleseconds;
  while (system_get_ticks() < end_time) {
    // Spin
  }
}
