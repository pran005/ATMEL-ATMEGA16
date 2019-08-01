/*
 * WDT.cpp
 *
 * Created: 06-06-2018 22:25:01
 * Author : Pranjal
 */ 

#include <avr/io.h>
#include <util/delay.h>

void WDT_ON()
{
	WDTCR |= (1<<WDE)|(1<<WDP1)|(1<<WDP2) ; 									// 1 sec timeout 
}
void WDT_OFF()
{
	WDTCR |=  (1<<WDE)|(1<<WDTOE) ; 
	WDTCR = 0x00  ; 
}
int main(void)
{   
	WDT_ON() ; 
	DDRB  |= 0xff ;
	PORTB |= (1<<PORTB3) ; 
	_delay_ms(1000) ; 
	PORTB &= ~(1<<PORTB3) ;
    
    while (1) 
    {
    }
}

