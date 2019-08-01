/*
 * ADC_1.cpp
 *
 * Created: 25-04-2018 20:29:20
 * Author : Pranjal
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <P_LCD.h>


 
 
int main(void)
{
    /* Replace with your application code */
	init_LCD() ; 
	ADCSRA |= (1<<ADEN) ;
	ADCSRA |= (1<<ADPS0) ;
	ADCSRA |= (1<<ADPS2) ;
	ADMUX  |= (1<<ADLAR) ;
	ADMUX  |= (1<<REFS0) ;
	ADCSRA |= (1<<ADIE) ;
	sei() ;				 //enable GLobal interupts
	ADCSRA |= 1<<ADSC ;
    while (1) 
    {
		
    }
}
ISR(ADC_vect)
{ int result  ;
	result = ADCW; 
	clr_lcd() ;  
	lcd(result); 
	_delay_ms(200) ; 
	ADCSRA |= 1<<ADSC ;
	
}
