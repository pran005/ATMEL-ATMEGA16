/*
 * Bluetooth-HC05.cpp
 * Status : Final DO NOT EDIT ! WORKING
 * Created: 15-06-2018 11:21:41
 * Author : Pranjal
 * Last Edited 15-06-2018 22:06:12
 */ 
#define F_CPU 16000000UL
#include <avr/io.h>
#include <P_LCD.h>
#include <UART.h>
#include <util/delay.h>
#include <string.h>

int main(void)
{	init_LCD() ; 
	init_UART(38400) ;
		
	 char c , str[50] ; 
	 
    //DDRA |= (1<<DDB7) ; 
	    while (1) 
    {	/*UART_Tx("HELLO") ; 
		memset(str,0,50) ;  
		UART_str(str) ;
		clr_lcd() ; 
	    lcd(str) ;
		
		UART_Tx(str) ;  
		_delay_ms(800) ; */ 
		c=UART_Rx() ; 
		if(c=='1')
		{	clr_lcd();
			lcd("PRAN") ;
			PORTA |= (1<<PORTA7);
			UART_Tx("ON") ;	}
		else 
		{	clr_lcd();
			lcd("no") ; 
			PORTA &= ~(1<<PORTA7);
			UART_Tx("OFF") ;
		}
		//_delay_ms(200) ; 
		_delay_ms(800) ;
    }}
}
 
 
