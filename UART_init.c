#include "main.h"
#
/**
 * Initializes UART0 module on Port A with a baud rate of 9600.
 */
void UART_Init(void)
{
    // Enable clock for UART0 and Port A
    SYSCTL_RCGCUART_R |= 0x01;
    SYSCTL_RCGCGPIO_R |= 0x01;

    // Wait for the Port A to be ready
    while ((SYSCTL_PRGPIO_R & 0x01) == 0);

    // Disable UART0 while configuring
    UART0_CTL_R &= ~(0x0001);

    // Set the baud rate to 9600
    UART0_IBRD_R = 104;
    UART0_FBRD_R = 11;

    // Configure the UART for 8-bit data, no parity, one stop bit
    UART0_LCRH_R = 0x70;

    // Enable UART0, receive and transmit
    UART0_CTL_R |= 0x301;

    // Enable alternate function for PA0 and PA1
    GPIO_PORTA_AFSEL_R |= 0x03;

    // Configure PA0 and PA1 as UART pins
    GPIO_PORTA_PCTL_R = (GPIO_PORTA_PCTL_R & 0xFFFFFF00) + 0x00000011;

    // Enable digital function for PA0 and PA1
    GPIO_PORTA_DEN_R |= 0x03;

    // Disable analog function for PA0 and PA1
    GPIO_PORTA_AMSEL_R &= ~(0x03);
}
