#include "main.h"

/**
 * Sends a character via UART0 module.
 *
 * @param data The character to send.
 */
void UART_OutChar(char data)
{
    // Wait until UART0 transmit FIFO is not full
    while ((UART0_FR_R & 0x20) != 0);    // Check if the transmit FIFO is full

    // Write the character to the transmit FIFO
    UART0_DR_R = data;             // Send the character by writing it to the transmit FIFO
}

/**
 * Receives a character via UART0 module.
 *
 * @return The received character.
 */
char UART_InChar(void)
{
    // Wait until UART0 receive FIFO is not empty
    while ((UART0_FR_R & 0x10) != 0);

    // Read the received character from the receive FIFO
    return (char)(UART0_DR_R & 0xFF);
}
