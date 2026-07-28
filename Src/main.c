
//LCD CHARACTER DISPLAY TEST PROGRAM

#include "stm32f103xb.h"
#include <stdint.h>
#include "lcd.h"
#include "systeminit.h"
#include "text.h"

int main(void)
{
    sysclock_config();
    __enable_irq();
    systick_config();
    pins_init();
    lcd_init();

    // 1. Start at top-left (Row 1, Column 0) and print your name
    lcd_goto(1, 0);
    write_string("WELCOME TO :OMOLLO'S LAB");


    while(1)
    {

        delay(500);         // Wait 400ms between steps
        lcd_scroll_left();
    }
}
