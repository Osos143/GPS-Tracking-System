#ifndef MAIN_H
#define MAIN_H

/**
 * @file main.h
 * @brief Header file for a C program that reads GPS data and calculates the distance between the current location and a predefined location.
 *
 * This header file includes several standard libraries such as `stdint.h`, `stdbool.h`, `string.h`, `stdlib.h`, `stdio.h`, and `math.h`. It also includes a device-specific header file `TM4C123.h` and `tm4c123gh6pm.h`.
 *
 * This header file defines several macros, including `RS`, `RW`, `EN`, `PI`, and `EARTH_RADIUS`. It also declares several global variables, including `GPS_logName`, `GPS`, `Valid`, `N_or_S`, `E_or_W`, `currentLong`, `currentLat`, `speed`, `lat`, `longi`, `speedd`, `str`, `concatenated`, `Final_Disantce_int`, and `Final_Distance_string`.
 *
 * The header file also declares several function prototypes, including `UART_Init()`, `delayUs()`, `delayMs()`, `LCD4bits_Init()`, `LCD_Write4bits()`, `LCD_WriteString()`, `LCD4bits_Cmd()`, `LCD4bits_Data()`, `PortF_Init()`, `degrees_to_radians()`, `haversine()`, `UART_OutChar()`, `UART_InChar()`, `getCommandd()`, `getCommand()`, `GPStoDeg()`, and `GPS_read2()`.
 *
 */
 
	//includes
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "TM4C123.h"
#include "tm4c123gh6pm.h"


	//Macros
#define RS 0x01				 	//RS -> PB0 (0x01)
#define RW 0x02         //RW -> PB1 (0x02)
#define EN 0x04  		 	 	//EN -> PB2 (0x04)

#define PI 3.141592653589793238
#define EARTH_RADIUS 6371000.0 // Earth's radius in kilometers

	//Global variables
const double long_final = 03116.79784, lat_final = 3003.90511;

char GPS_logName[] = "$GPRMC,";
char GPS[80];
char Valid[1];
char N_or_S[1];
char E_or_W[1];
float currentLong, currentLat, speed;
char lat[20] = {0};
char longi[20] = {0};
char speedd[10] = {0};

	//Prototypes of the functions
void UART_Init(void);
void delayUs(int);
void delayMs(int);
void LCD4bits_Init(void);
void LCD_Write4bits(unsigned char, unsigned char);
void LCD_WriteString(char*);
void LCD4bits_Cmd(unsigned char);
void LCD4bits_Data(unsigned char);
void PortF_Init(void);
float degrees_to_radians(float degrees);
float haversine(float lat1, float lon1, float lat2, float lon2);
void UART_OutChar(char data);
char UART_InChar(void);
void getCommandd(char *command, int len);
void getCommand(char *command, char stopchar);
double GPStoDeg(double val);
void GPS_read2(void);

#endif
