
#ifndef LCD_H_
#define LCD_H_
#include "stdint.h"

void lcd_init(void);
void pins_init(void);
void lcd_goto(uint8_t row, uint8_t col);
void rs_high(void);
void rs_low(void);
void enable_high(void);
void enable_low(void);
void send_data(uint8_t data);
void lcd_scroll_left(void);
void lcd_scroll_right(void);


#endif /* LCD_H_ */
