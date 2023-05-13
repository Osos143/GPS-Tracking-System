#include "main.h"

/**
 * Initializes the LCD in 4-bit mode.
 */
void LCD4bits_Init(void)
{
    SYSCTL_RCGCGPIO_R |= 0x02;   // Enable clock for PORTB
    delayMs(10);                 // Delay 10 ms for enable the clock of PORTB
    GPIO_PORTB_DIR_R = 0xFF;     // Set PORTB as output pins
    GPIO_PORTB_DEN_R = 0xFF;     // Enable PORTB digital IO pins

    // Initialize the LCD
    LCD4bits_Cmd(0x28);          // 2 lines and 5x7 character (4-bit data, D4 to D7)
    LCD4bits_Cmd(0x06);          // Automatic Increment cursor (shift cursor to right)
    LCD4bits_Cmd(0x01);          // Clear display screen
    LCD4bits_Cmd(0x0F);          // Display on, cursor blinking
}
