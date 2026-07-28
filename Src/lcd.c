/*
 * FILE FOR INITIALIZING THE LCD
 *
 */

#include "systeminit.h"
#include "stm32f103xb.h"
#include "stdint.h"
#include "lcd.h"

void pins_init(void)
{
    // 1. Enable Clocks for GPIOA and GPIOB
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN;

    // 2. Configure Control Pins (A0 = E, B0 = RS)
    // Clear CRL bits for pin 0 on both ports
    GPIOA->CRL &= ~(0xFU << 0U);
    GPIOB->CRL &= ~(0xFU << 0U);

    // Set both to General Purpose Push-Pull Output, Max Speed 2MHz (0x2)
    GPIOA->CRL |= (0x2U << 0U);
    GPIOB->CRL |= (0x2U << 0U);

    // 3. Configure Data Lines (A4, A5, A6, A7)
    // Clear bits 16 to 31 in GPIOA->CRL
    GPIOA->CRL &= ~(0xFFFFU << 16U);
    // Set A4-A7 as General Purpose Push-Pull Output, Max Speed 2MHz (0x2222)
    GPIOA->CRL |= (0x2222U << 16U);

    // Ensure initial control states are LOW
    GPIOB->BSRR = GPIO_BSRR_BR0; // RS Low
    GPIOA->BSRR = GPIO_BSRR_BR0; // E Low
}
// Private helper function to pulse the Enable pin
static void pulse_enable_pin(void)
{
    GPIOA->BSRR = GPIO_BSRR_BS0; // Enable HIGH

    // Maintain pulse width (~140ns+ at 72MHz)
    for (volatile int i = 0; i < 15; i++) {
        __NOP();
    }

    GPIOA->BSRR = GPIO_BSRR_BR0; // Enable LOW

    // Data hold/execution time safety gap
    for (volatile int i = 0; i < 15; i++) {
        __NOP();
    }
}

void rs_high(void)
{
    GPIOB->BSRR = GPIO_BSRR_BS0;
}

void rs_low(void)
{
    GPIOB->BSRR = GPIO_BSRR_BR0;
}

void enable_high(void)
{
    GPIOA->BSRR = GPIO_BSRR_BS0;
}

void enable_low(void)
{
    GPIOA->BSRR = GPIO_BSRR_BR0;
}

/*
 * Distributes a full 8-bit command or data byte across the 4-bit bus
 * Sends High Nibble first, then Low Nibble automatically.
 */
void send_data(uint8_t data)
{
    // --- STAGE 1: SEND HIGH NIBBLE (Bits 7, 6, 5, 4) ---
    GPIOA->ODR &= ~(0xFU << 4U);             // Clear data pins A4-A7
    GPIOA->ODR |= ((data & 0xF0U));          // Align and apply high bits
    pulse_enable_pin();                      // Clock it in

    // --- STAGE 2: SEND LOW NIBBLE (Bits 3, 2, 1, 0) ---
    GPIOA->ODR &= ~(0xFU << 4U);             // Clear data pins A4-A7
    GPIOA->ODR |= ((data & 0x0FU) << 4U);    // Shift up and apply low bits
    pulse_enable_pin();                      // Clock it in

    delay_us(50);                            // Execution time margin for standard instructions
}

/*
 * Special helper for initialization stages where only a SINGLE
 * 4-bit nibble must be clocked in before the chip shifts to 4-bit mode.
 */
static void send_init_nibble(uint8_t nibble)
{
    GPIOA->ODR &= ~(0xFU << 4U);
    GPIOA->ODR |= ((nibble & 0x0FU) << 4U);
    pulse_enable_pin();
}

void lcd_init(void)
{
    // Ensure pins are ready before communicating
    pins_init();

    rs_low(); // All initialization calls are configuration commands

    // Hardware Power-On Stabilization Window
    delay(50);

    // Force the chip out of an uncertain/confused state into 8-bit alignment
    send_init_nibble(0x3);
    delay(5);

    send_init_nibble(0x3);
    delay_us(150);

    send_init_nibble(0x3);
    delay_us(150);

    // Activate 4-Bit Interface Mode
    send_init_nibble(0x2);
    delay(2);

    // From this point forward, the chip expects complete 8-bit packages (Dual-Nibble)
    send_data(0x28); // Function Set: 4-bit mode, 2 lines, 5x8 font [1]
    send_data(0x0C); // Display ON, Cursor OFF, Blink OFF [1]
    send_data(0x01); // Clear Display Memory [1]
    delay(2);        // Clearing display requires a longer hardware execution pause [1]
    send_data(0x06); // Entry Mode Set: Increment cursor automatically, No shift [1]
}
void lcd_goto(uint8_t row, uint8_t col)
{
    uint8_t address;

    if (row == 1) {
        address = 0x00 + col; // Row 1 starts at 0x00
    } else {
        address = 0x40 + col; // Row 2 starts at 0x40
    }

    rs_low();
    // The "Set DDRAM Address" command requires Bit 7 to be 1 (0x80)
    send_data(0x80 | address);
}
// Shifts the entire contents of the screen one space to the left
void lcd_scroll_left(void)
{
    rs_low();
    send_data(0x18); // Command 0x18: Shift display left
}

// Shifts the entire contents of the screen one space to the right
void lcd_scroll_right(void)
{
    rs_low();
    send_data(0x1C); // Command 0x1C: Shift display right
}


