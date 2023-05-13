#include "main.h"

/**
 * Delays execution for the specified number of milliseconds.
 *
 * @param n The number of milliseconds to delay
 */
void delayMs(int n)
{
    volatile int i, j; // Volatile is important for variables incremented in code

    for (i = 0; i < n; i++) // Loop 'n' times for the desired delay
    {
        for (j = 0; j < 3180; j++) // Delay for 1 msec (approx.)
        {
            // No operation performed, loop used to introduce delay
        }
    }
}

/**
 * Delays execution for the specified number of microseconds.
 *
 * @param n The number of microseconds to delay.
 */
void delayUs(int n)
{
    volatile int i, j; // Volatile is important for variables incremented in code

    for (i = 0; i < n; i++) // Loop 'n' times for the desired delay
    {
        for (j = 0; j < 3; j++) // Delay for 1 microsecond (approx.)
        {
            // No operation performed, loop used to introduce delay
        }
    }
}
