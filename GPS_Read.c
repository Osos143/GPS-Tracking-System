#include "main.h"

double GPStoDeg(double val)
{
    return (int)(val / 100) + (val - (int)(val / 100) * 100) / 60.0;
}

/**
 * Reads GPS data from UART and extracts latitude, longitude, and speed.
 */
void GPS_read2()
{
    char counter = 0;
    char receivedChar[1];
    char i = 0;
    char flag = 1;
    char c[1];

    // Look for GPS log name in UART data
    do
    {
        getCommandd(c, 1);
        while (c[0] != GPS_logName[i])
        {
            memset(c, 0, 1);
            getCommandd(c, 1);
        }
        i++;
    } while (i != 7);

    // Extract latitude, longitude and speed from UART data
    while (1)
    {
        if (flag)
        {
            getCommandd(receivedChar, 1);
            if (receivedChar[0] == ',')
            {
                counter++;
            }
        }
        switch (counter)
        {
            case 1:
                getCommandd(Valid, 1);
                break;
            case 2:
                getCommand(lat, ',');
                counter++;
                flag = 0;
                break;
            case 3:
                getCommandd(N_or_S, 1);
                flag = 1;
                break;
            case 4:
                getCommand(longi, ',');
                counter++;
                flag = 0;
                break;
            case 5:
                getCommandd(E_or_W, 1);
                flag = 1;
                break;
            case 6:
                getCommand(speedd, ',');
                counter++;
                flag = 0;
                break;
        }
        if (counter == 7)
        {
            break;
        }
    }

    // Convert latitude and longitude to decimal degrees format
    if (N_or_S[0] == 'N')
    {
        currentLat = atof(lat);
    }
    else
    {
        currentLat = -atof(lat);
    }

    if (E_or_W[0] == 'E')
    {
        currentLong = atof(longi);
    }
    else
    {
        currentLong = -atof(longi);
    }
}
