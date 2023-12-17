#ifndef CNET_SYS_DEF_H
#define CNET_SYS_DEF_H

#include <commen-def.h>

#define CPU_FREQ      (84000000)
#define SYSTICK_FREQ  (1000)

void system_setup(void);
void system_teardown(void);
uint64_t system_millis(void);
void system_delay(uint64_t milleseconds);

#endif // CNET_SYS_DEF_H
