#include "TM4C123.h"   // Header file for the TM4C123 microcontroller
#include <stdint.h>    // Standard integer data types
#include <stdbool.h>   // Standard boolean data type
#include <string.h>    // String handling functions
#include <stdlib.h>
#include <stdio.h>     
#include <math.h>      
#include "tm4c123gh6pm.h" 

#define RS 0x01        // Bit mask for LCD RS pin
#define RW 0x02        // Bit mask for LCD RW pin
#define EN 0x04        // Bit mask for LCD EN pin

#define PI 3.141592653589793238  // Value of pi
#define EARTH_RADIUS 6371000.0   // Mean radius of the Earth in meters

// Define variables to hold data from GPS module
char GPS_logName[]="$GPRMC,";  // GPS command string
char GPS[80];                  // Buffer to hold GPS data
char Valid[1];                 // Validity of GPS data
char N_or_S[1];                // North or South hemisphere
char E_or_W[1];                // East or West hemisphere
float currentLong, currentLat,speed; // Current longitude, latitude, and speed
char lat[20]={0};              // Buffer to hold latitude data
char longi[20]={0};            // Buffer to hold longitude data
char speedd[10]={0};           // Buffer to hold speed data

// Function declarations
void delayUs(int);
void delayMs(int);
void LCD4bits_Init(void);
void LCD_Write4bits(unsigned char, unsigned char);
void LCD_WriteString(char*);
void LCD4bits_Cmd(unsigned char);
void LCD4bits_Data(unsigned char);

// Function to output a character through UART
void UART_OutChar(char data) {
    while ((UART0_FR_R & 0x20) != 0); // Wait until UART is ready to transmit
    UART0_DR_R = data;                // Write the character to the UART data register
}

// Function to read a character from UART
char UART_InChar(void) {
    while ((UART0_FR_R & 0x10) != 0); // Wait until there is data in the UART receive buffer
    return (char)(UART0_DR_R & 0xFF); // Read the received character from the UART data register
}

// Function to read a command from UART with a specified length
void getCommandd(char *command , int len ){ 
    char character;
    int i;
    for(i=0 ; i<len ; i++){
        character = UART_InChar(); // Read a character from the UART
        if(character != '\r') {    // Check if the character is not the carriage return character
            command[i] = character; // Store the character in the given buffer
            UART_OutChar(command[i]); // Output the character back to the UART
        } else if (character == '\r' || i == len) {
            break;  // If the character is the carriage return character or the length of the buffer is reached, break out of the loop
        }
    }
}

// Function to read a command from UART until a specified character is encountered
void getCommand(char *command , char stopchar ){ 
    char character[1];
    int i = 0;
    while(1) {
        getCommandd(character,1);   // Read a character from the UART
        if(character[0] != stopchar) {
            command[i] = character[0]; // Store the character in the given buffer
            i++;
        } else if (character[0] == stopchar) {
            break;  // If the character is the stop character, break out of the loop
        }
    }
}

// Function to convert GPS coordinates from degrees and minutes to decimal degrees
double GPStoDeg(double val) {
    return (int)(val / 100) + (val - (int)(val / 100) * 100) / 60.0; // Convert the given GPS coordinate from degrees and minutes to decimal degrees
}

// Function to read data from GPS module and extract latitude, longitude, and speed
void GPS_read2() {
    char counter = 0;
    char recievedChar[1];
    char i = 0;
    char flag = 1;
    char c[1];

    // Loop until the GPS command is found in the incoming data
    do {
        getCommandd(c,1);    // Read a character from the UART
        while(c[0] != GPS_logName[i]) { // Check if the character matches the current character in the GPS command string
            memset(c,0,1);              // Clear the character buffer
            getCommandd(c,1);           // Read a new character from the UART
        }
        i++;
    } while(i != 7);

    // Loop until all necessary data has been extracted from the GPS string
    while(1) {
        if(flag) {
            getCommandd(recievedChar,1); // Read a character from the UART
            if(recievedChar[0] == ',') { // Check if the character is a comma
                counter++;              // Increment the counter
            }
        }
        switch(counter) {
            case 1: getCommandd(Valid,1); break; // Read the validity of the GPS data
            case 2: getCommand(lat,','); counter++; flag=0; break; // Read the latitude
            case 3: getCommandd(N_or_S,1); flag=1; break; // Read the hemisphere of the latitude
            case 4: getCommand(longi,','); counter++; flag=0; break; // Read the longitude
            case 5: getCommandd(E_or_W,1); flag=1; break; // Read the hemisphere of the longitude
            case 6: getCommand(speedd,','); counter++; flag=0; break; // Read the speed
        }
        if(counter == 7) break; // If all necessary data has been extracted, break out of the loop
    }

    // Convert latitude and longitude from degrees and minutes to decimal degrees
    if(N_or_S[0] == 'N') {
        currentLat = atof(lat); // Convert the latitude to a floating-point value
    } else {
        currentLat = -atof(lat); // Convert the latitude to a floating-point value and negate it
    }
    if(E_or_W[0] == 'E') {
        currentLong = atof(longi); // Convert the longitude to a floating-point value
    } else {
        currentLong = -atof(longi); // Convert the longitude to a floating-point value and negate it
    }
}
void PortF_Init(void){ 
 SYSCTL_RCGCGPIO_R |= 0x20; // activate clock for Port F 
 while((SYSCTL_PRGPIO_R&0x20) == 0){}; // wait for clock to stabilize 
 GPIO_PORTF_LOCK_R = 0x4C4F434B; // unlock Port F 
 GPIO_PORTF_CR_R = 0x1F; // allow changes to PF4-0 
 GPIO_PORTF_AMSEL_R = 0x00; // disable analog on Port F 
 GPIO_PORTF_PCTL_R = 0x00000000; // configure PF4-0 as GPIO 
 GPIO_PORTF_DIR_R = 0x0E; // PF4,PF0 input, PF3,PF2,PF1 output 
 GPIO_PORTF_AFSEL_R = 0x00; // disable alt funct on Port F 
 GPIO_PORTF_DEN_R = 0x1F; // enable digital I/O on Port F 
}

// Function to convert degrees to radians
float degrees_to_radians(float degrees) {
    return degrees * PI / 180.0;
}

// Function to calculate distance between two GPS coordinates using Haversine formula
float haversine(float lat1, float lon1, float lat2, float lon2) {
    float dlat, dlon, a, c, d;
    
    // Convert GPS coordinates from degrees, minutes, seconds to decimal degrees
    lat1=GPStoDeg(lat1);
    lon1=GPStoDeg(lon1);
    lat2=GPStoDeg(lat2);
    lon2=GPStoDeg(lon2);
  
    // Convert latitude and longitude to radians
    dlat = degrees_to_radians(lat2 - lat1);
    dlon = degrees_to_radians(lon2 - lon1);

    // Calculate Haversine formula
    a = sin(dlat/2) * sin(dlat/2) + cos(degrees_to_radians(lat1)) * cos(degrees_to_radians(lat2)) * sin(dlon/2) * sin(dlon/2);
    c = 2 * atan2(sqrt(a), sqrt(1-a));
    d = EARTH_RADIUS * c;

    return d;
}

// Function to initialize UART communication
void UART_Init(void) {
    SYSCTL_RCGCUART_R |= 0x01;
    SYSCTL_RCGCGPIO_R |= 0x01;
    while ((SYSCTL_PRGPIO_R & 0x01) == 0);
    
    UART0_CTL_R &= ~(0x0001);
    UART0_IBRD_R = 104;
    UART0_FBRD_R = 11;
    UART0_LCRH_R = 0x70;
    UART0_CTL_R |= 0x301;
    GPIO_PORTA_AFSEL_R |= 0x03;
    GPIO_PORTA_PCTL_R = (GPIO_PORTA_PCTL_R & 0xFFFFFF00) + 0x00000011;
    GPIO_PORTA_DEN_R |= 0x03;
    GPIO_PORTA_AMSEL_R &= ~(0x03);
}

// GPS coordinates of the final destination
const double long_final = 03116.75297;
const double lat_final = 3003.84861;

int main(void){
    float distance_walked, prev_lat = 0, prev_lon = 0;
    float distance ;
    char* str, *str1;
    char concatenated[] = "Distance is ";
    char concatenated1[] = "Remaining is ";
    int Final_Disantce_int = 0, Remaining_Disantce_int = 0;
    char Final_Distance_string[5], Remaining_Distance_string[5];
    float Final_Disantce_float = 0;

    // Initialize UART communication, LED pins, and LCD
    UART_Init();
    PortF_Init();
    LCD4bits_Init();
		
    while(1){
        // Read GPS data
        memset(lat, 0, 20);
        memset(longi, 0, 20);
        memset(speedd, 0, 20);
        GPS_read2();
        
        // Calculate distance between current location and final destination
        distance = haversine(currentLat, currentLong, lat_final, long_final);
        distance -= 10;
        
        // Display remaining distance and change LED color based on distance
        if (distance < 200){
            Remaining_Disantce_int = (int)distance;
            sprintf(Remaining_Distance_string, "%d", Remaining_Disantce_int);
            strcpy(concatenated1, "Remaining is ");
            strcat(concatenated1, Remaining_Distance_string);
            
            str = concatenated1;
            LCD4bits_Cmd(0x01);
            LCD4bits_Cmd(0x80);
            delayMs(100);
            LCD_WriteString(str);
            delayMs(100);
            
            if (distance > 5){
                GPIO_PORTF_DATA_R &= ~0x0E;  // turn off all LEDs
                GPIO_PORTF_DATA_R |= 0x02;   // turn on red LED
            }
            else if ((distance < 5 && distance > 2)){
                GPIO_PORTF_DATA_R &= ~0x0E;  // turn off all LEDs
                GPIO_PORTF_DATA_R |= 0x04;   // turn on blue LED
                
                str = "YOU ALMOST THERE";
                LCD4bits_Cmd(0x01);
                LCD4bits_Cmd(0x80);
                delayMs(100);
                LCD_WriteString(str);
                delayMs(100);
            }
            else if (distance < 2){
                GPIO_PORTF_DATA_R &= ~0x0E;  // turn off all LEDs
                GPIO_PORTF_DATA_R |= 0x08;   // turn on green LED
                
                Final_Disantce_int = (int)Final_Disantce_float ;
                sprintf(Final_Distance_string, "%d", Final_Disantce_int);
                strcat(concatenated, Final_Distance_string);
                
                str1 = concatenated;
                LCD4bits_Cmd(0x01);
                LCD4bits_Cmd(0x80);
                delayMs(100);
                LCD_WriteString(str1);
                delayMs(100);
                
                break;
            }
        }
        
        // Calculate total distance walked and add to final distance
        if (prev_lat != 0 && prev_lon != 0) {
            distance_walked = haversine(currentLat, currentLong, prev_lat, prev_lon);
            if (distance_walked < 5){
                Final_Disantce_float += distance_walked;
            }
        }
        prev_lat = currentLat;
        prev_lon = currentLong;
    }
    return 0;
}

// Initialize the LCD by setting up the necessary GPIO pins and sending initialization commands to the LCD
void LCD4bits_Init(void)
{
    SYSCTL_RCGCGPIO_R |= 0x02;    // enable clock for PORTB
    delayMs(10);                 // delay 10 ms for enable the clock of PORTB
    GPIO_PORTB_DIR_R = 0xFF;             // let PORTB as output pins
    GPIO_PORTB_DEN_R= 0xFF;             // enable PORTB digital IO pins
    
    // Send initialization commands to the LCD
    LCD4bits_Cmd(0x28);          // 2 lines and 5x7 character (4-bit data, D4 to D7)
    LCD4bits_Cmd(0x06);          // Automatic Increment cursor (shift cursor to right)
    LCD4bits_Cmd(0x01);					 // Clear display screen
    LCD4bits_Cmd(0x0F);          // Display on, cursor blinking
}

// Write a 4-bit command or data to the LCD
void LCD_Write4bits(unsigned char data, unsigned char control)
{
    data &= 0xF0;                       // clear lower nibble for control 
    control &= 0x0F;                    // clear upper nibble for data
    GPIO_PORTB_DATA_R = data | control;         // Include RS value (command or data ) with data 
    GPIO_PORTB_DATA_R = data | control | EN;    // pulse EN
    delayUs(0);													// delay for pulsing EN
    GPIO_PORTB_DATA_R = data | control;					// Turn off the pulse EN
    GPIO_PORTB_DATA_R = 0;                      // Clear the Data 
}

// Write a null-terminated string to the LCD
void LCD_WriteString(char * str)
{  
    volatile int i = 0;          // volatile is important 
    
    // Write each character of the string until the end of the string
    while(*(str+i) != '\0')
    {
        LCD4bits_Data(*(str+i));    // Write each character of the string
        i++;                        // Increment for next character
    }
}

// Send a command to the LCD
void LCD4bits_Cmd(unsigned char command)
{
    // Send the upper nibble of the command first, followed by the lower nibble
    LCD_Write4bits(command & 0xF0 , 0);    // upper nibble first
    LCD_Write4bits(command << 4 , 0);			 // then lower nibble
    
    // Delay after certain commands
    if(command < 4)
        delayMs(2);       // commands 1 and 2 need up to 1.64ms
    else
        delayUs(40);      // all others need a 40us delay
}

// Send data to the LCD
void LCD4bits_Data(unsigned char data)
{
    // Send the upper nibble of the data first, followed by the lower nibble
    LCD_Write4bits(data & 0xF0 , RS);   // upper nibble first
    LCD_Write4bits(data << 4 , RS);     // then lower nibble
    
    // Delay for the LCD (MCU is faster than LCD)
    delayUs(40);
}

// Delay for n milliseconds by executing a nested loop
void delayMs(int n)
{  
    volatile int i,j;             // volatile is important for variables incremented in code
    for(i=0;i<n;i++)
        for(j=0;j<3180;j++)         // delay for 1 msec
        {}
}

// Delay for n microseconds by executing a nested loop
void delayUs(int n)             
{
    volatile int i,j;							// volatile is important for variables incremented in code
    for(i=0;i<n;i++)
        for(j=0;j<3;j++)            // delay for 1 micro second
        {}
}
