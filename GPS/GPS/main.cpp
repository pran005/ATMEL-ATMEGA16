/* FINAL CODE 
 * STATUS : WORKING
 * GPS.cpp
 * Last Edited: 26-06-2018 21:54:12 
 * Created:     26-06-2018 18:17:03
 * Author : Pranjal
 */ 

#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <P_LCD.h> 
#include <UART.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>

#define  SREG _SFR_IO8(0x3F)
#define  size_buff 150

char lat[15] , longitude[15] , alt[15] , utc_time[15], sattelite[4] ;
char DMS[20] , dir[2] ;   
int data_ptr[20] ;
volatile int data_num , buff_index ;  
char Rx_buff[size_buff] ; 
char GGA_header[3] ;
bool GGA_flag = false ; 

void convert_time()
{
	unsigned int hour , min , sec ; 
	uint32_t value ; 
	value = atol(utc_time) ; 
	hour  = value/10000 ; 
	min	  = (value%10000)/100 ; 
	sec   = (value%10000)%100 ; 
	
	sprintf(utc_time,"%d:%d:%d",hour,min,sec) ;  
}
void co_ordinates(char *value)
{
	uint32_t degrees ; 
	double num, mins, i;
	double cordinate ;  
	num = atof(value) ; 
	degrees = num/100 ; 
	i = num/100 ;
	mins = (i - degrees)/0.6 ; 
	cordinate = (float)degrees + mins ;
	memset(DMS,0,20) ; 
	dtostrf(cordinate,8,4,DMS) ;    
	
}
void get_time()
{
	//cli() ; 
	memset(utc_time,0,15) ; 
	uint8_t index = 0 ;
	
	for (uint8_t i=0 ; Rx_buff[i] !=',' ; i++)
		{
			utc_time[index] = Rx_buff[i] ; 
			index++ ; 
		}
	convert_time();
	//sei();  	
} 
void lattitude(uint8_t index)
{
//	cli() ; 
	index++ ; 
	int i=0 ; 
	for (; Rx_buff[index] !=',' ; index++)
	{
		lat[i] = Rx_buff[index]  ; 
		i++  ; 
	}
	dir[0] = Rx_buff[++index]  ;
//	sei() ; 
}
void get_long(uint8_t index)
{
	//cli() ;
	index++ ;
	int i=0 ;
	for (; Rx_buff[index] !=',' ; index++)
	{
		longitude[i] = Rx_buff[index]  ;
		i++ ; 
	}
		dir[1] = Rx_buff[++index]  ;
		sattelite[0] = Rx_buff[index+5]  ;
		
		
	//sei() ;
}
void get_altitude(uint8_t index)
{
	//cli() ;
	index++ ;
	int i=0 ;
	for (; Rx_buff[index] !=',' ; index++)
	{
		alt[i] = Rx_buff[index]  ;
		i++ ; 
	}
	alt[i] = Rx_buff[++index];
	//sei() ;
}
ISR(USART_RXC_vect)
{
	uint8_t sreg = SREG ; 
	cli() ; 
	char Receive = UDR ;
	if(Receive=='$')
	{
		GGA_flag = false ;
		data_num = 0 ; 
		buff_index = 0 ; 
	}
	else if(GGA_header[0]=='G' && GGA_header[1]=='G' && GGA_header[2]=='A')
	{
		GGA_flag = true ;
		memset(GGA_header,0,3) ;  
	}
	else if(GGA_flag==true)
	{
		if(Receive==',')
			data_ptr[data_num++] = buff_index ; 
		Rx_buff[buff_index++] = Receive ; 	
	}
	else
	{
		//memmove(GGA_header, GGA_header+1 ,2) ;
		GGA_header[0] = GGA_header[1] ; GGA_header[1] = GGA_header[2] ; 
		GGA_header[2] = Receive ;  
	}
	SREG = sreg ; 
}
int main(void)
{ 
	buff_index = 0 ; 
	memset(Rx_buff,0,size_buff) ; 
	init_LCD() ;
	_delay_ms(4000) ; 
	init_UART(9600) ; 
	sei() ; 
    
    while (1) 
    {
		clr_lcd() ; 
		_delay_ms(1000) ; 
		get_time() ;
		lcd("UTC TIME : ") ; 
		cmd(0xC0) ; 
		lcd(utc_time) ; 
		_delay_ms(1000) ; 
		
		clr_lcd() ; 
		lattitude(data_ptr[0]) ; 
		co_ordinates(lat) ; 
		//lcd(lat) ;
		 lcd(DMS) ;
		 data_4bit(0xDF) ;   
		 lcd(dir[0]) ; 
		_delay_ms(1000) ;
		
		cmd(0xC0) ; 
		get_long(data_ptr[2]) ; 
		co_ordinates(longitude) ; 
		lcd(DMS) ;   
		data_4bit(0xDF) ; 
		lcd(dir[1]) ; 
		_delay_ms(1000) ; 
		
		clr_lcd() ; 
		get_altitude(data_ptr[7]) ;
		lcd("ALTITUDE: ") ;
		lcd(alt) ; 
		_delay_ms(1000) ; 
		cmd(0xC0) ; 
		lcd("No. of Sat: ");
		lcd(sattelite) ; 
		_delay_ms(1000) ;  
    }
}

