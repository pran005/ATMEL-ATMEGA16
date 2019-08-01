/* FINAL Code
 * HCSR04.cpp
 * Status : WOrking, tolerably accurate
 * Created:		 16-06-2018 15:04:27
 * Last Edited : 16-06-2018 15:35:43
 * Author : Pranjal
 */ 
#define F_CPU 16000000UL 
#include <avr/io.h>
#include <P_LCD.h>
#include <avr/interrupt.h>

int overflow=0 ; 

int main(void)
{	
	long count ; 
	double distance ; 
    DDRA |= (1<<4) ;
	PORTD|= (1<<PORTD6) ;  
	sei() ; 
	TIMSK |= (1<<TOIE1) ; 
	TCCR1A = 0x00 ;
	init_LCD() ;  
    while (1) 
    {
		PORTA |= (1<<PORTA4) ; 
		_delay_us(10) ; 
		PORTA &= ~(1<<PORTA4) ;
		TCNT1 = 0 ;												//clr count 
		TCCR1B |= (1<<ICES1)|(1<<CS10) ;							//Rising Edge , pre-scalar = 1 
		TIFR |= (1<<ICF1)|(1<<TOV1) ;							//CLEAR ICF-TOV flag 
		while(!(TIFR & (1<<ICF1))) ;
		
		TCNT1 = 0 ;
		TCCR1B = 0x00 ;  
		TCCR1B &= ~(1<<ICES1) ;								   //falling edge  
		TCCR1B |= (1<<CS10) ; 
		TIFR |= (1<<ICF1)|(1<<TOV1) ;
		overflow = 0 ; 
		 
		while(!(TIFR & (1<<ICF1))) ; 
		count = ICR1 + (overflow*65535) ; 
		distance = ((double)count)*0.0010625 ;
		clr_lcd() ;  
		lcd(distance) ; 
		_delay_ms(800) ; 
    }
}

ISR(TIMER1_OVF_vect)
{
	overflow++ ; 
}