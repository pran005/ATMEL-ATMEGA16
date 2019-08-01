/*6
 * SPI.cpp
 *
 * Created: 09-06-2018 12:18:34
 * Author : Pranjal
 */ 

#include <avr/io.h>
#include <LCD.h>
#include<P_SPI.h>
#define MOSI 5 
#define MISO 6 
#define SCK  7 
#define SS   4 
#define SS_low  PORTB &= ~(1<<4) 
#define SS_high PORTB |= (1<<4) 


void SPI_Master_init()
{
	DDRB |= (1<<MOSI)|(1<<SCK)|(1<<SS) ;
	DDRB &= ~(1<<MISO) ; 
	SPCR |= (1<<SPE)|(1<<MSTR)|(1<<SPR0) ; 
	//SPSR &= ~(1<<SPI2X);
	SS_low ;   
}
unsigned short SPI_Communicate(char data) 
{   
	char i ; 
	SPDR = data ;
	while(!(SPSR & (1<<SPIF))) ; 
	//return SPDR ;
	return SPDR ;  
}
void SPI_slave_init()
{	
	DDRB &= ~(1<<MOSI)|(1<<SCK)|(1<<SS) ;
	DDRB |=  (1<<MISO) ;  
	SPCR |=  (1<<SPE)  ;  
}
int main(void)
{	
    init_LCD() ; 
	SPI_Master_init() ; 
	char i=51 , j ; 
    while (1) 
    { 
		j=SPI_Communicate(i) ; 
		clr_lcd(); 
		//lcd("Sending ") ; 
		lcd(i++) ; 
		cmd(0xC0) ; 
		lcd(j) ; 
		//i++ ; 
		_delay_ms(500) ; 
		 
    }
}

