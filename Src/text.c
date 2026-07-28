//this file is an api for displaying the characters
#include "lcd.h"
#include "text.h"

void write_text(uint8_t txt)
{
    rs_high();        // Tell LCD: "This byte is text to be printed"
    send_data(txt);   // Send the raw character code (e.g., 'A' -> 0x41)
    rs_low();         // Reset RS to low as a safe default state
}

void write_string(char *str)
{
	  // Loop through the string until the null-terminator (\0) is reached
	  while (*str)
	  {
	    write_text((uint8_t)*str); // Print current character
	        str++;                     // Move pointer to the next letter
	  }
}


