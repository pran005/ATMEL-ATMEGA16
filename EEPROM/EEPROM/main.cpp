/*
 * EEPROM.cpp
 *
 * Created: 08-06-2018 15:05:21
 * Author : Pranjal
 */ 

#include <avr/io.h>
#include <P_LCD.h>
#include <util/delay.h>
#include <avr/eeprom.h>

int main(void)
{   uint8_t data='a' ; 
	uint8_t read ;
	init_LCD() ; 
	//lcd(data) ; 
	/* Replace with your application code */
    while (1) 
    { 
		
		eeprom_update_byte((uint8_t*)0x0A,data) ; 
		read = eeprom_read_byte((const uint8_t*)0x0A) ; 
		lcd((char)read) ;  
		_delay_ms(800) ;
		clr_lcd(); 
    }
}

