// THIS FILE CONTAINS THE CONFIGURATION FOR:
/*
 * 1. SYSTEM CLOCK
 * 2. SYSTICK SETTING
 * 3. DELAY FUNCTION
 * 4.
*/
#include "stm32f103xb.h"

volatile uint32_t ms_ticks;
//1. system clock setting
void sysclock_config(void)
{

	RCC->CR &=~(RCC_CR_HSEBYP); //1. DISABLE EXTERNAL USER CLOCK BYPASS TO AVOID STRAY READINGS
	RCC->CR |=(RCC_CR_HSEON);	//2. ENABLE EXTERNAL CLOCK SOURCE
	while (!(RCC->CR & RCC_CR_HSERDY));//WAIT FOR STABILIZATION
	RCC->CFGR &=~(RCC_CFGR_PLLXTPRE); //3. SET PRESCALER TO 0
	RCC->CFGR |= RCC_CFGR_PLLSRC; //4. SET THE PLL SOURCE AS HSE
	RCC->CFGR &=~(RCC_CFGR_PLLMULL_Msk);//5. SET PLL MUL TO 9
	RCC->CFGR |=(RCC_CFGR_PLLMULL9);
	FLASH->ACR |= FLASH_ACR_PRFTBE; //6. ENABLE PREFETCH BUFFER
	FLASH->ACR &=~(FLASH_ACR_LATENCY_Msk);
	FLASH->ACR |= (FLASH_ACR_LATENCY_2);//7. SET LATENCY TO 2
	RCC->CR |=RCC_CR_PLLON;
	while(!(RCC->CR & RCC_CR_PLLRDY));
	RCC->CFGR &=~(RCC_CFGR_HPRE_Msk);//AHB PRESCALER TO 1
	RCC->CFGR &=~(RCC_CFGR_PPRE2_Msk);//APB2 NOT DIVIDED
	RCC->CFGR &=~(RCC_CFGR_PPRE1_Msk);
	RCC->CFGR |=(RCC_CFGR_PPRE1_DIV2);//APB1 DIVIDED BY 2
	RCC->CFGR |=(RCC_CFGR_ADCPRE_DIV6);//ADC DIVIDED BY 6
	RCC->CFGR &=~(RCC_CFGR_SW_Msk);
	RCC->CFGR |=(RCC_CFGR_SW_PLL); //SET PLL AS SYSCLOCK SOURCE
	while ((RCC->CFGR & RCC_CFGR_SWS_Msk) != RCC_CFGR_SWS_PLL);

}

void systick_config(void)
{
	//DISAVLE THE TIMER
	SysTick ->CTRL =0;
	//SET RELOAD VALUE
	SysTick ->LOAD = 71999;
	//CLEAR CURRENT COUNTER VLAUE
	SysTick->VAL = 0;
	//SELECT CLOCK SOURCE
	SysTick ->CTRL |=(SysTick_CTRL_CLKSOURCE_Msk);
	//ENABLE INTERRUPT
	SysTick->CTRL |=(SysTick_CTRL_TICKINT_Msk);
	//START THE TIMER
	SysTick ->CTRL |=(SysTick_CTRL_ENABLE_Msk);
}

void SysTick_Handler(void)
{
	ms_ticks++;
}

uint32_t get_ticks(void)
{
	return ms_ticks;
}

void delay(uint32_t ms)
{
	uint32_t start = get_ticks();
	while((get_ticks() - start)<ms)
		{

		}
}

void delay_us(uint32_t us)
{
    // At 72 MHz, 1 microsecond = exactly 72 clock cycles
    uint32_t total_ticks = us * 72;

    uint32_t start_val = SysTick->VAL;
    uint32_t elapsed_ticks = 0;

    // Loop until the required hardware ticks pass
    while (elapsed_ticks < total_ticks)
    {
        uint32_t current_val = SysTick->VAL;

        // Handle normal down-counting vs timer wrap-around past 0
        if (current_val <= start_val)
        {
            elapsed_ticks = start_val - current_val;
        }
        else
        {
            // Account for reloading from 71999
            elapsed_ticks = start_val + (71999 - current_val);
        }
    }
}



