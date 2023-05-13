#include "main.h" 

/**
 * Initializes port f for input and output.
 */
void PortF_Init(void)
{ 
    // Activate clock for Port F
    SYSCTL_RCGCGPIO_R |= 0x20;

    // Wait for clock to stabilize
    while ((SYSCTL_PRGPIO_R & 0x20) == 0){};

    // Unlock Port F
    GPIO_PORTF_LOCK_R = 0x4C4F434B;

    // Allow changes to PF4-0
    GPIO_PORTF_CR_R = 0x1F;

    // Disable analog on Port F
    GPIO_PORTF_AMSEL_R = 0x00;

    // Configure PF4-0 as GPIO
    GPIO_PORTF_PCTL_R = 0x00000000;

    // CONFIGURE PF4, PF0 as input and PF3, PF2, PF1 as output
    GPIO_PORTF_DIR_R = 0x0E;

    // Disable alternate function on Port F
    GPIO_PORTF_AFSEL_R = 0x00;

    // Enable digital I/O on Port F
    GPIO_PORTF_DEN_R = 0x1F;
}
