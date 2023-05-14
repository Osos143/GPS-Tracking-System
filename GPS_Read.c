#include "main.h"

double GPStoDeg(double val)
{
    // This function converts GPS coordinates from the format they are received (degrees, minutes, and seconds) to decimal degrees.
    // It takes in a value in the GPS format and returns the corresponding value in decimal degrees format.
    return (int)(val / 100) + (val - (int)(val / 100) * 100) / 60.0;
}

/**
 * Reads GPS data from UART and extracts latitude, longitude, and speed.
 */
void GPS_read2()
{
    // Define some variables to be used in the function.
    char counter = 0; // Counter for keeping track of which piece of GPS data is being extracted.
    char receivedChar[1]; // Buffer for storing a single character of received GPS data.
    char i = 0; // Counter for iterating through the GPS log name.
    char flag = 1; // Flag for determining whether the current character should be processed.
    char c[1]; // Buffer for storing a single character of GPS log name.

    // Look for GPS log name in UART data
    // This loop looks for the GPS log name in the received GPS data.
    // It searches for the characters of the GPS log name in order.
    // It waits for each character to arrive before moving on to the next one.
    // It assumes that the GPS log name is 7 characters long.
    do
    {
        getCommandd(c, 1); // Get a single character of GPS data from the UART.
        while (c[0] != GPS_logName[i]) // Wait until the current character matches the next character of the GPS log name.
        {
            memset(c, 0, 1); // Clear the buffer.
            getCommandd(c, 1); // Get another character of GPS data from the UART.
        }
        i++; // Move on to the next character of the GPS log name.
    } while (i != 7); // Keep looping until all characters of the GPS log name have been found.

    // Extract latitude, longitude and speed from UART data
    // This loop extracts the latitude, longitude, and speed from the received GPS data.
    // It does this by reading characters one at a time and interpreting them based on the current value of the counter.
    while (1)
    {
        if (flag) // If flag is set to 1, read a character of GPS data.
        {
            getCommandd(receivedChar, 1);
            if (receivedChar[0] == ',') // If the character is a comma, increment the counter.
            {
                counter++;
            }
        }
        switch (counter) // Process the character based on the current value of the counter.
        {
            case 1:
                getCommandd(Valid, 1); // Store the validity of the data.
                break;
            case 2:
                getCommand(lat, ','); // Store the latitude.
                counter++; // Move on to the next piece of data.
                flag = 0; // Wait for the next character to arrive.
                break;
            case 3:
                getCommandd(N_or_S, 1); // Store the hemisphere of the latitude.
                flag = 1; // Wait for the next character to arrive.
                break;
            case 4:
                getCommand(longi, ','); // Store the longitude.
                counter++; // Move on to the next piece of data.
                flag = 0; // Wait for the next character to arrive.
                break;
            case 5:
                getCommandd(E_or_W, 1); // Store the hemisphere of the longitude.
                flag = 1; // Wait for the next character to arrive.
                break;
            case 6:
                getCommand(speedd, ','); // Store the speed.
                counter++; // Move on to the next piece of data.
                flag = 0; // Wait for the next character to arrive.
                break;
        }
        if (counter == 7) // If all pieces of data have been extracted, exit the loop.
        {
            break;
        }
    }

    // Convert latitude and longitude to decimal degrees format
    // This section converts the latitude and longitude from the GPS format to decimal degrees format.
    // It does this by calling the GPStoDeg function defined earlier.
    if (N_or_S[0] == 'N')
    {
        currentLat = atof(lat); // If the latitude is in the northern hemisphere, convert it directly.
    }
    else
    {
        currentLat = -atof(lat); // If the latitude is in the southern hemisphere, convert it and negate the result.
    }

    if (E_or_W[0] == 'E')
    {
        currentLong = atof(longi); // If the longitude is in the eastern hemisphere, convert it directly.
    }
    else
    {
        currentLong = -atof(longi); // If the longitude is in the western hemisphere, convert it and negate the result.
    }
}
