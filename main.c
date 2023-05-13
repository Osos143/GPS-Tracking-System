#include "main.h"

/**

main - Entry point for the program
This program reads GPS data and calculates the distance between the current location
and a final location. It then displays the remaining distance on an LCD screen and
changes the color of an LED based on the distance. Finally, it calculates the total
distance walked and displays it on the LCD screen.
Return: 0 on success
*/

int main(void)
{
float distance_walked, prev_lat = 0, prev_lon = 0;
float distance;
char *str, *str1;
char concatenated[] = "Distance is ";
char concatenated1[] = "Remaining is ";
int Final_Disantce_int = 0, Remaining_Disantce_int = 0;
char Final_Distance_string[5], Remaining_Distance_string[5];
float Final_Disantce_float = 0;
	
	//Initialize UART, PortF, and LCD4bits
UART_Init();
PortF_Init();
LCD4bits_Init();

while (1)
{
    //Reset arrays
    memset(lat, 0, 20);
    memset(longi, 0, 20);
    memset(speedd, 0, 20);

    //Read GPS data and calculate distance between current location and final location
    GPS_read2();
    distance = haversine(currentLat, currentLong, lat_final, long_final);
    Remaining_Disantce_int = (int)distance;
    sprintf(Remaining_Distance_string, "%d", Remaining_Disantce_int);
    strcpy(concatenated1, "Remaining is ");
    strcat(concatenated1, Remaining_Distance_string);

    //Display remaining distance on LCD
    str = concatenated1;
    LCD4bits_Cmd(0x01);
    LCD4bits_Cmd(0x80);
    delayMs(100);
    LCD_WriteString(str);
    delayMs(100);

    //Change LED color based on distance
    if (distance > 5)
    {
        GPIO_PORTF_DATA_R &= ~0x0E;
        GPIO_PORTF_DATA_R |= 0x02;
    }
    else if ((distance < 5 && distance > 2))
    {
        GPIO_PORTF_DATA_R &= ~0x0E;
        GPIO_PORTF_DATA_R |= 0x04;
        str = "YOU ALMOST THERE";
        LCD4bits_Cmd(0x01);
        LCD4bits_Cmd(0x80);
        delayMs(100);
        LCD_WriteString(str);
        delayMs(100);
    }
    else if (distance < 2)
    {
        GPIO_PORTF_DATA_R &= ~0x0E;
        GPIO_PORTF_DATA_R |= 0x08;
        Final_Disantce_int = (int)Final_Disantce_float;
        sprintf(Final_Distance_string, "%d", Final_Disantce_int);
        strcat(concatenated, Final_Distance_string);

        //Display final distance on LCD
        str1 = concatenated;
        LCD4bits_Cmd(0x01);
        LCD4bits_Cmd(0x80);
        delayMs(100);
        LCD_WriteString(str1);
        delayMs(100);

        break;
    }

    //Calculate total distance walked
    if (prev_lat != 0 && prev_lon != 0)
    {
        distance_walked = haversine(currentLat, currentLong, prev_lat, prev_lon);
        Final_Disantce_float += distance_walked;
    }

    prev_lat = currentLat;
    prev_lon = currentLong;
}

return 0;
}
