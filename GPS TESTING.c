#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "tm4c123gh6pm.h"

#define PI 3.141592653589793238
#define EARTH_RADIUS 6371.0 // Earth's radius in kilometers


double degrees_to_radians(double degrees) {
    return degrees * PI / 180.0;
}

double haversine(double lat1, double lon1, double lat2, double lon2) {
    double dlat, dlon, a, c, d;

    dlat = degrees_to_radians(lat2 - lat1);
    dlon = degrees_to_radians(lon2 - lon1);

    a = sin(dlat/2) * sin(dlat/2) + cos(degrees_to_radians(lat1)) * cos(degrees_to_radians(lat2)) * sin(dlon/2) * sin(dlon/2);
    c = 2 * atan2(sqrt(a), sqrt(1-a));
    d = EARTH_RADIUS * c;

    return d;
}

void UART_Init(void) {
    SYSCTL_RCGCUART_R |= 0x01;      // enable UART0
    SYSCTL_RCGCGPIO_R |= 0x01;      // enable port A
    while ((SYSCTL_PRGPIO_R & 0x01) == 0);   // wait for port A to be ready

    // configure PA0 and PA1 for UART0

		UART0_CTL_R &= ~(0x0001); // disable UART0 during setup
		UART0_IBRD_R = 104; // 16MHz/16=1MHz, 1MHz/104=9600 baud rate
		UART0_FBRD_R = 11; // fraction part: 0.166
		UART0_LCRH_R = 0x70; // 8-bit word length, enable FIFO
		UART0_CTL_R |= 0x301; // enable UART0, TX and RX
		GPIO_PORTA_AFSEL_R |= 0x03; // enable alternate functions PA0 and PA1
		GPIO_PORTA_PCTL_R = (GPIO_PORTA_PCTL_R & 0xFFFFFF00) + 0x00000011; // UART0 pins
		GPIO_PORTA_DEN_R |= 0x03; // enable digital pins PA0 and PA1
		GPIO_PORTA_AMSEL_R &= ~(0x03); // disable analog function
}

char UART_InChar(void) {
    while ((UART0_FR_R & 0x10) != 0);   // wait until RXFE is 0 (receive FIFO is not empty)
    return (char)(UART0_DR_R & 0xFF);   // return received data
}

void UART_OutChar(char data) {
    while ((UART0_FR_R & 0x20) != 0);   // wait until TXFF is 0 (transmit FIFO is not full)
    UART0_DR_R = data;                  // write data to transmit FIFO
}

void GPS_Init(void) {
    UART_Init();              // initialize UART for GPS communication

    // configure u-blox 6M GPS module to output NMEA sentences for GPRMC
    // see u-blox 6M data sheet for more information on NMEA sentences
    UART_OutChar('$');
    UART_OutChar('P');
    UART_OutChar('U');
    UART_OutChar('B');
    UART_OutChar('X');
    UART_OutChar(',');
    UART_OutChar('5');
    UART_OutChar(',');
    UART_OutChar('1');
    UART_OutChar(',');
    UART_OutChar('1');
    UART_OutChar(',');
    UART_OutChar('1');
    UART_OutChar(',');
    UART_OutChar('1');
    UART_OutChar('\r');
    UART_OutChar('\n');
}

void GPS_ReadData(double* latitude, double* longitude, bool* latitude_available, bool* longitude_available, char* status, bool* valid_data) {
    char data;
    char buffer[20];
    int i ;

    int comma_count = 0;
    int latitude_degrees = 0;
    double latitude_minutes = 0;
    char latitude_direction;
    int longitude_degrees = 0;
    double longitude_minutes = 0;
    char longitude_direction;
    bool latitude_found = false;
    bool longitude_found = false;

    while (1) {
        data = UART_InChar();

        if (data == '$') {      // start of NMEA sentence
            i = 0;
            comma_count = 0;
            latitude_found = false;
            longitude_found = false;
        }
        else if (data == ',') {     // comma separator between fields
            buffer[i] = '\0';
            i = 0;
            comma_count++;

            if (comma_count == 3) {     // latitude data
                latitude_degrees = (int)(atof(buffer) / 100);
                latitude_minutes = atof(buffer) - (latitude_degrees * 100);
                latitude_found = true;
            }
            else if (comma_count == 4) {    // latitude direction
                latitude_direction = buffer[0];
            }
            else if (comma_count == 5) {    // longitude data
                longitude_degrees = (int)(atof(buffer) / 100);
                longitude_minutes = atof(buffer) - (longitude_degrees * 100);
                longitude_found = true;
            }
            else if (comma_count == 6) {    // longitude direction
                longitude_direction = buffer[0];
            }
            else if (comma_count == 2) {    // status
                *status = buffer[0];
            }
        }
        else if (data == '\r') {    // end of NMEA sentence
            buffer[i] = '\0';

            if (strcmp(buffer, "GPRMC") == 0) {    // valid GPRMC sentence
                if (latitude_direction == 'S') {
                    latitude_degrees *= -1;
                    latitude_minutes *= -1;
                }

                if (longitude_direction == 'W') {
                    longitude_degrees *= -1;
                    longitude_minutes *= -1;
                }

                if (latitude_found) {
                    *latitude = (double)latitude_degrees + (latitude_minutes / 60);
                    *latitude_available = true;
                }
                else {
                    *latitude_available = false;
                }

                if (longitude_found) {
                    *longitude = (double)longitude_degrees + (longitude_minutes / 60);
                    *longitude_available = true;
                }
                else {
                    *longitude_available = false;
                }

                *valid_data = true;
                return;
            }
        }
        else if (data != '\n') {   // non-essential data (e.g. checksum)
            buffer[i++] = data;
        }
    }
}

  int __main() {

		double latitude, longitude, destination_latitude = 30.045333, destination_longitude = 31.398994, distance;
    bool latitude_available, longitude_available, valid_data;
    char status;

    GPS_Init();     // initialize GPS module

    // Wait until valid GPS data is available.
    while (1) {
        GPS_ReadData(&latitude, &longitude, &latitude_available, &longitude_available, &status, &valid_data);
         if (valid_data && latitude_available && longitude_available && status == 'A'){
            // Calculate the distance between the current location and the destination.
            distance = haversine(latitude, longitude, destination_latitude, destination_longitude)*1000;

            // Output the distance to the destination in meters.
            printf("Distance to destination: %.2lf m\n", distance);
        }
    }
}
