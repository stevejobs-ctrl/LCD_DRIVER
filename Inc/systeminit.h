

#ifndef SYSTEMINIT_H_
#define SYSTEMINIT_H_
#include <stdint.h>

void sysclock_config(void);
void systick_config(void);
void SysTick_Handler(void);
uint32_t get_ticks(void);
void delay(uint32_t ms);
void delay_us(uint32_t us);

#endif
