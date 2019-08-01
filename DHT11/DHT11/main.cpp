/*
 * DHT11.cpp
 * FINAL DHT CODE
 * Status : Working , check sensor if doesn't
 * Left	  : To Scale decimal values
 * Created:		14-06-2018 00:07:39
 * Last edited: 14-06-2018	01:27:45
 * Author : Pranjal
 */ 
#define F_CPU 16000000UL 
#include <avr/io.h>
#include <P_LCD.h>
#include <util/delay.h>
#define	 DHT 7 
uint8_t data=0 ;

void start()
{
	DDRD |= (1<<DHT) ; 
	PORTD &= ~(1<<DHT) ; 
	_delay_ms(20) ; 
	PORTD |= (1<<DHT) ;
}
void read()
{
	DDRD &= ~(1<<DHT) ; 
	while(PIND & (1<<DHT)) ;											/* JAB TAK mcu ka HIGH chale*/
    while(!(PIND & (1<<DHT))) ;											// jab tak DHT low rakhe 
	while(PIND & (1<<DHT)) ;											// jab tak DHT high rakhe
}
uint8_t DHT_data()
{   
	 
	for(int i=0 ; i<8 ; i++)
	{
		while(!(PIND & (1<<DHT))) ; 
		_delay_us(30) ; 
		if(PIND & (1<<DHT))													//low k bad >30us high i.e  1
		data = ((data<<1) | (0x01)) ;
		else 
		data = data<<1 ;
		while(PIND & (1<<DHT)) ;											// poll until its high   
	}  
		return data ; 
	}

int main(void)
{   
    uint8_t temperature,humidity ;
    unsigned int hum,temp,checksum ;
 
    init_LCD() ; 
	lcd("HELLO") ; 
    while (1) 
    {  clr_lcd() ; 
		start() ; 
		read () ;
		humidity = DHT_data() ; 
		hum = DHT_data() ;
		temperature = DHT_data() ; 
		temp = DHT_data() ;
		checksum = DHT_data() ; 
		if((temp+hum+temperature+humidity)!= checksum) 
		lcd("ERROR") ; 	
		lcd(humidity) ; 
		lcd(".") ;
		lcd(hum) ; 
		cmd(0xC0) ; 
		lcd(temperature) ; 
		lcd(".") ;
		lcd(temp) ;
		_delay_ms(2200) ; 
    }
}

