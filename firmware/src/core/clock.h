#ifndef _CNETIO_CLOCK_
#define _CNETIO_CLOCK_

#include <commendef.h>

#define CPU_FREQ      (180000000)
#define SYSTICK_FREQ  (1000)

void system_setup(void);
uint64_t system_get_ticks(void);
void system_delay(uint64_t milleseconds);

#endif
