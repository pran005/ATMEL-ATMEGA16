/*
 * Capture.cpp
 *
 * Created: 06-06-2018 14:16:50
 * Author : Pranjal
 */
#define F_CPU 1000000UL
#include <avr/io.h>
#include <P_LCD.h>
#include <avr/interrupt.h>

int main(void)
{   unsigned int rise1,rise2,fall,period ,on ; 
	float duty; int freq;
	init_LCD();
	PORTD = 0xff ; 
        while (1) 		
    {
	   
	   TCCR1A = 0 ;
	   TCNT1 = 0 ;
	   TIFR |= (1<<ICF1) ;
	   
	   TCCR1B |= (1<<ICES1)|(1<<CS10) ;
	   while(!(TIFR&(1<<ICF1))) ;							//rise
	   rise1=ICR1 ;
	   TIFR |= (1<<ICF1) ;
	   
	   TCCR1B &= ~(1<<ICES1) ;
	   TCCR1B |= (1<<CS10) ;							   //fall
	   while(!(TIFR&(1<<ICF1))) ;
	   fall= ICR1 ;
	   TIFR |= (1<<ICF1) ;
	   
	   TCCR1B |= (1<<ICES1)|(1<<CS10) ;				  //riseagain
	   while(!(TIFR&(1<<ICF1))) ;
	   rise2=ICR1 ;
	   TIFR |= (1<<ICF1) ;
	   TCCR1B = 0 ;
	   
	   if(rise1<fall && fall<rise2)
	   {
		   period = rise2-rise1 ;
		   on = fall - rise1 ;
		   
		   freq =  F_CPU/period ;
		   duty = ((float)on/(float)period)*100 ;
		   clr_lcd() ;
		   lcd(freq) ;
		   cmd(0xC0) ;
		   lcd(duty) ;
		   lcd(" %") ;
		   _delay_ms(800) ;
	   }

}
}
