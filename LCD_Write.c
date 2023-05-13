#include "main.h"

/**
 * Writes 4 bits of data to the LCD.
 *
 * @param data The 4 bits of data to write.
 * @param control The control signal (RS) value.
 */
void LCD_Write4bits(unsigned char data, unsigned char control)
{
    data &= 0xF0;                            // Clear lower nibble for control
    control &= 0x0F;                         // Clear upper nibble for data
    GPIO_PORTB_DATA_R = data | control;      // Include RS value (command or data) with data
    GPIO_PORTB_DATA_R = data | control | EN; // Pulse EN
    delayUs(0);                              // Delay for pulsing EN
    GPIO_PORTB_DATA_R = data | control;      // Turn off the pulse EN
    GPIO_PORTB_DATA_R = 0;                   // Clear the data
}

/**
 * Writes a string to the LCD.
 *
 * @param str The string to write.
 */
void LCD_WriteString(char *str)
{
    volatile int i = 0;          // Volatile is important

    while (*(str + i) != '\0')   // Until the end of the string
    {
        LCD4bits_Data(*(str + i)); // Write each character of string
        i++;                       // Increment for next character
    }
}

/**
 * Writes a command to the LCD.
 *
 * @param command The command to write.
 */
void LCD4bits_Cmd(unsigned char command)
{
    LCD_Write4bits(command & 0xF0, 0); // upper nibble first
    LCD_Write4bits(command << 4, 0);   // Then lower nibble

    if (command < 4)
    {
        delayMs(2); // Commands 1 and 2 need up to 1.64ms
    }
    else
    {
        delayUs(40); // all others need 40 us
    }
}

/**
 * Writes data to the LCD.
 *
 * @param data The data to write.
 */
void LCD4bits_Data(unsigned char data)
{
    LCD_Write4bits(data & 0xF0, RS); // Upper nibble first
    LCD_Write4bits(data << 4, RS);   // Then lower nibble
    delayUs(40);                     // Delay for LCD (MCU is faster than LCD)
}
