#ifndef UART
#define UART 


#include <avr/io.h>
#include <p_lcd.h>
#include <util/delay.h>


//async normal mode UBRR = F_CPU/(16*BAUD) - 1   | x2 for double speed mode
void init_UART(unsigned int baud)
{
	int baudrate = F_CPU/(16*baud) - 1 ;
	UBRRH = (baudrate >> 8) ;
	UBRRL = baudrate ;
	UCSRB |= (1<<TXEN) | (1<<RXEN) ;
	UCSRC |= (1<<URSEL)| (1<<USBS) | (1<<UCSZ0) | (1<<UCSZ1) ; 								//UCSZ1 and UCSZ0 are 1 by default i.e 8bit data
}
void UART_Tx(unsigned int data)
{
	while( !(UCSRA & (1<<UDRE)) ) ;																//polling until buffer empty
	UCSRB &= ~(1<<TXB8) ;
	//	if (data & 0x0100 )																		//check for 9th bit
	//	UCSRB |= 1<<TXB8 ;
	UDR = data ;
}
void UART_Tx(int data)
{
	while( !(UCSRA & (1<<UDRE)) ) ;																//polling until buffer empty
	UCSRB &= ~(1<<TXB8) ;
	//	if (data & 0x0100 )																		//check for 9th bit
	//	UCSRB |= 1<<TXB8 ;
	UDR = data ;
}
void UART_Tx(char data)
{
	while( !(UCSRA & (1<<UDRE)) ) ;																//polling until buffer empty
	UCSRB &= ~(1<<TXB8) ;
	//	if (data & 0x0100 )																		//check for 9th bit
	//	UCSRB |= 1<<TXB8 ;
	UDR = data ;
}

void UART_Tx (char *string)
{
	while( !(UCSRA & (1<<UDRE)) ) ;
	while (*string)
	UART_Tx(*string++) ;
	UART_Tx('\0') ;																			//can change to '\n' as per requirement
}
unsigned char UART_Rx()
{
	while (! (UCSRA & (1<<RXC)) ) ;
	return UDR ;
}
void UART_str (uint8_t size)
{
	char str[size] ;
	char c ; int i=0 ;
	do
	{
		c= UART_Rx() ;
		str[i++] = c ;
	}    while(c!='\0');
	lcd(str) ;
}
#endif //UART 
