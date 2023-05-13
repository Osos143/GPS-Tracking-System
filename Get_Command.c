#include "main.h"
/**
 * Reads a command of a specified length from UART.
 *
 * @param command The buffer to store the command in.
 * @param len The length of the command to read.
 */
void getCommandd(char *command, int len)
{
    char character;
    int i;

    for (i = 0; i < len; i++)
    {
        character = UART_InChar();
        if (character != '\r')
        {
            command[i] = character;
            UART_OutChar(command[i]);
        }
        else if (character == '\r' || i == len)
        {
            break;
        }
    }
}

/**
 * Reads a command from UART until a stop character is encountered.
 *
 * @param command The buffer to store the command in.
 * @param stopchar The character that marks the end of the command.
 */
void getCommand(char *command, char stopchar)
{
    char character[1];
    int i = 0;

    while (1)
    {
        getCommandd(character, 1);
        if (character[0] != stopchar)
        {
            command[i] = character[0];
            i++;
            // UART_OutChar(command[i]);
        }
        else if (character[0] == stopchar)
        {
            break;
        }
    }
}
