#ifndef P_SPI_H
#define P_SPI_H

#include <avr/io.h>
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
#endif  //P_SPI_H