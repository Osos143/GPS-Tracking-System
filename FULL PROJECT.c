#include "TM4C123.h"    
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "tm4c123gh6pm.h"
#define RS 0x01
#define RW 0x02
#define EN 0x04

#define PI 3.141592653589793238
#define EARTH_RADIUS 6371000.0 

char GPS_logName[]="$GPRMC,";
char GPS[80] ;
char Valid [1];
char N_or_S [1];
char E_or_W [1];
float currentLong, currentLat,speed ;
char lat [20]={0} ;
 char longi [20]={0} ;
 char speedd [10]={0} ;
 
//Functions Declaration
void delayUs(int);   														   //Delay in Micro Seconds
void delayMs(int);   														   //Delay in Milli Seconds
void LCD4bits_Init(void);													 //Initialization of LCD Dispaly
void LCD_Write4bits(unsigned char, unsigned char); //Write data as (4 bits) on LCD
void LCD_WriteString(char*);											 //Write a string on LCD 
void LCD4bits_Cmd(unsigned char);									 //Write command 
void LCD4bits_Data(unsigned char);								 //Write a character
 
void UART_OutChar(char data) {
    while ((UART0_FR_R & 0x20) != 0);
    UART0_DR_R = data;
}	 

char UART_InChar(void) {
    while ((UART0_FR_R & 0x10) != 0);
		return (char)(UART0_DR_R & 0xFF);
}

void getCommandd(char *command , int len ){ 
		char character ;
		int i ;
		for(i=0 ; i<len ; i++){
				character=UART_InChar();
				if( character!='\r'){
						command[i]=character ;
						UART_OutChar(command[i]) ;
				}else if (character=='\r' || i==len ){
						break ;
				}
				
		}
	
}
 
 void getCommand(char *command , char stopchar ){ 
		char character [1] ;
		int i = 0  ;
		while(1){
				getCommandd(character,1);
				if( character[0]!=stopchar){
						command[i]=character[0] ;
					  i++ ;
						//UART_OutChar(command[i]) ;
				}else if (character[0]==stopchar ){
						break ;
				}
				
		}
	
}
 	
double GPStoDeg(double val)
{
	return (int)(val / 100) + (val - (int)(val / 100) * 100) / 60.0;
    
} //before lat and long is passet to get_distance 

void GPS_read2(){
	char counter =0 ;
	char  recievedChar [1];
	char i=0 ;
	char flag =1 ;
	char c [1];
	do{
		getCommandd(c,1);
		while(c[0]!=GPS_logName[i]){
			memset(c,0,1);
			getCommandd(c,1);
		}
		i++;
		}while(i!=7);
	while(1){
			if(flag){
				
				
				getCommandd(recievedChar,1);
				if(recievedChar[0]==','){counter++;}
			}
			switch(counter){
				case 1 : getCommandd(Valid,1); break ;
				case 2 : getCommand(lat,',');counter++;flag=0;break;
				case 3 : getCommandd(N_or_S,1);flag=1;break;
				case 4 : getCommand(longi,',');counter++;flag=0;break;
				case 5 : getCommandd(E_or_W,1);flag=1;break;
				case 6 : getCommand(speedd,',');counter++;flag=0;break;
			
			}
			if(counter==7) break;
	
	}
	if(N_or_S[0]=='N')
						currentLat=atof(lat);
					else
						currentLat=-atof(lat) ;
	if(E_or_W[0]=='E')
						currentLong=atof(longi);
					else
						currentLong=-atof(longi);

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

float degrees_to_radians(float degrees) {
    return degrees * PI / 180.0;
}

float haversine(float lat1, float lon1, float lat2, float lon2) {
    float dlat, dlon, a, c, d;
	lat1=GPStoDeg(lat1);
	lon1=GPStoDeg(lon1);
	lat2=GPStoDeg(lat2);
	lon2=GPStoDeg(lon2);
  
    dlat = degrees_to_radians(lat2 - lat1);
    dlon = degrees_to_radians(lon2 - lon1);

    a = sin(dlat/2) * sin(dlat/2) + cos(degrees_to_radians(lat1)) * cos(degrees_to_radians(lat2)) * sin(dlon/2) * sin(dlon/2);
    c = 2 * atan2(sqrt(a), sqrt(1-a));
    d = EARTH_RADIUS * c;

    return d;
}

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

const double long_final=03116.75297,lat_final=3003.84861  ; 


int main(void){
		//char str[20];
	  float distance_walked ,prev_lat = 0, prev_lon = 0;
		float distance ;
	
		char* str,*str1;
	char concatenated[]=	"Distance is ";
		char concatenated1[]= "Remaining is ";
		int Final_Disantce_int = 0,Remaining_Disantce_int=0;
		char Final_Distance_string[5],Remaining_Distance_string[5];
float Final_Disantce_float=0;

	
		UART_Init();
	  PortF_Init();
		LCD4bits_Init();
		
		while(1){
				memset(lat,0,20);
				memset(longi,0,20);
			  memset(speedd,0,20);
				GPS_read2() ;
				distance =  haversine(currentLat ,currentLong ,lat_final ,long_final) ;
			distance-=10;
			if (distance <200){
			Remaining_Disantce_int = (int)distance;
				sprintf(Remaining_Distance_string,"%d",Remaining_Disantce_int);
			strcpy( concatenated1,"Remaining is ");
			strcat( concatenated1,Remaining_Distance_string);
			
			str = concatenated1 ;  						//Write any string you want to display on LCD
			LCD4bits_Cmd(0x01);								//Clear the display
			LCD4bits_Cmd(0x80);               //Force the cursor to beginning of 1st line
			delayMs(100);											//delay 100 ms for LCD (MCU is faster than LCD)
			LCD_WriteString(str);							//Write the string on LCD
			delayMs(100);											//Delay 100 ms to let the LCD diplays the data
		
				if(distance >5){
			GPIO_PORTF_DATA_R &= ~0x0E;  // turn off all LEDs
			GPIO_PORTF_DATA_R |= 0x02;   // turn on red LED
		}else if ((distance<5 && distance >2)){
			GPIO_PORTF_DATA_R &= ~0x0E;  // turn off all LEDs
      GPIO_PORTF_DATA_R |= 0x04;   // turn on blue LED
				str = "YOU ALMOST THERE" ;  						//Write any string you want to display on LCD
			LCD4bits_Cmd(0x01);								//Clear the display
			LCD4bits_Cmd(0x80);               //Force the cursor to beginning of 1st line
			delayMs(100);											//delay 100 ms for LCD (MCU is faster than LCD)
			LCD_WriteString(str);							//Write the string on LCD
			delayMs(100);											//Delay 100 ms to let the LCD diplays the data
		
		}else if (distance <2){
			GPIO_PORTF_DATA_R &= ~0x0E;  // turn off all LEDs
			GPIO_PORTF_DATA_R |= 0x08;   // turn on green LED
			Final_Disantce_int=(int)Final_Disantce_float ;
			sprintf(Final_Distance_string,"%d",Final_Disantce_int);
			strcat( concatenated,Final_Distance_string);
			
			str1 = concatenated ;  						//Write any string you want to display on LCD
			LCD4bits_Cmd(0x01);								//Clear the display
			LCD4bits_Cmd(0x80);               //Force the cursor to beginning of 1st line
			delayMs(100);											//delay 100 ms for LCD (MCU is faster than LCD)
			LCD_WriteString(str1);							//Write the string on LCD
			delayMs(100);											//Delay 100 ms to let the LCD diplays the data
		
			break;
		}}
		  // calculate total distance walked
            if (prev_lat != 0 && prev_lon != 0) {
                distance_walked = haversine(currentLat ,currentLong , prev_lat, prev_lon);
							if (distance_walked < 5){
                Final_Disantce_float += distance_walked;}
            }
						
            prev_lat = currentLat;
            prev_lon = currentLong;
		}
			return 0;
	}

void LCD4bits_Init(void)
{
	SYSCTL_RCGCGPIO_R |= 0x02;    //enable clock for PORTB
	delayMs(10);                 //delay 10 ms for enable the clock of PORTB
  GPIO_PORTB_DIR_R = 0xFF;             //let PORTB as output pins
	GPIO_PORTB_DEN_R= 0xFF;             //enable PORTB digital IO pins
	LCD4bits_Cmd(0x28);          //2 lines and 5x7 character (4-bit data, D4 to D7)
	LCD4bits_Cmd(0x06);          //Automatic Increment cursor (shift cursor to right)
	LCD4bits_Cmd(0x01);					 //Clear display screen
	LCD4bits_Cmd(0x0F);          //Display on, cursor blinking
}


void LCD_Write4bits(unsigned char data, unsigned char control)
{
	data &= 0xF0;                       //clear lower nibble for control 
	control &= 0x0F;                    //clear upper nibble for data
	GPIO_PORTB_DATA_R = data | control;         //Include RS value (command or data ) with data 
	GPIO_PORTB_DATA_R = data | control | EN;    //pulse EN
	delayUs(0);													//delay for pulsing EN
	GPIO_PORTB_DATA_R = data | control;					//Turn off the pulse EN
	GPIO_PORTB_DATA_R = 0;                      //Clear the Data 
}

void LCD_WriteString(char * str)
{  
	volatile int i = 0;          //volatile is important 
	
	while(*(str+i) != '\0')       //until the end of the string
	{
		LCD4bits_Data(*(str+i));    //Write each character of string
		i++;                        //increment for next character
	}
}

void LCD4bits_Cmd(unsigned char command)
{
	LCD_Write4bits(command & 0xF0 , 0);    //upper nibble first
	LCD_Write4bits(command << 4 , 0);			 //then lower nibble
	
	if(command < 4)
		delayMs(2);       //commands 1 and 2 need up to 1.64ms
	else
		delayUs(40);      //all others 40 us
}

void LCD4bits_Data(unsigned char data)
{
	LCD_Write4bits(data & 0xF0 , RS);   //upper nibble first
	LCD_Write4bits(data << 4 , RS);     //then lower nibble
	delayUs(40);												//delay for LCD (MCU is faster than LCD)
}

void delayMs(int n)
{  
	volatile int i,j;             //volatile is important for variables incremented in code
	for(i=0;i<n;i++)
		for(j=0;j<3180;j++)         //delay for 1 msec
		{}
}

void delayUs(int n)             
{
	volatile int i,j;							//volatile is important for variables incremented in code
	for(i=0;i<n;i++)
		for(j=0;j<3;j++)            //delay for 1 micro second
		{}
}
