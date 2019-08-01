/*
 * SPI_slave.cpp
 *
 * Created: 09-06-2018 13:58:33
 * Author : Pranjal
 */ 

#include <avr/io.h>
#include <LCD.h>
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
	SPSR &= ~(1<<SPI2X);
	SS_low ;
}
unsigned char SPI_Communicate(char data)
{
	
	SPDR = data ;
	while(!(SPSR & (1<<SPIF))) ;
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
	SPI_slave_init() ; 
	char recieve,data = 255 ;  
    while (1) 
    {
		recieve = SPI_Communicate(data);
		clr_lcd() ; 
		lcd(recieve) ; 
		cmd(0xC0) ; 
		lcd(data--) ;  
    }
}

