#ifndef P_LCD
#define P_LCD 



#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>
#include<string.h>

void cmd(unsigned char cmd)
{
	int a , b ;
	a = cmd & 0xf0 ; //MSB
	PORTB = a | 0x04 ; // 0101 // --- Rs(1) R/W(0) E(1)
	_delay_ms(100) ;
	PORTB &= ~(0x04) ; // clear Enable (0)
	
	b = cmd<<4 ; //access LSB , 76543210 << 4 = 3210 ;
	PORTB = b | 0x04 ; // 0101 // ---- RS(1) RW(0) E(1)
	_delay_ms(100) ;
	PORTB &= ~(0x04) ;
	
}

void data_4bit(unsigned char x)
{
	char a  ,b ;
	a = x & 0xf0 ;
	PORTB = a | 0x05 ;
	_delay_ms(100) ;
	PORTB &= ~(0x04) ;
	
	b = x<<4 ;
	PORTB = b | 0x05 ;
	_delay_ms(100) ;
	PORTB &= ~(0x04) ;
	
}

void lcd (char *string)
{
	while(*string)
	data_4bit(*string++) ;
}
void lcd (int m)
{
	i=0, flag=0,r  ;
	char str[100] ;	
	if(m==0)
	{ str[i] = '0' ;
		i++ ;
		str[i]= '\0' ;
	}
	if(m<0)
	{ m=-m ;
		flag=1 ;
	}
	while(m!=0)
	{
		r=m%10 ;
		str[i++] = r+ '0' ;
		m=m/10 ;
	}
	if(flag==1)
	str[i++] = '-' ;
	str[i++] = '\0' ;
	strrev(str) ;
	lcd(str) ; 
}
void lcd(double lcd_num,unsigned short fdigit==6)
{
	int floating;
	int natural = (int)lcd_num;
	double tfloating;
	if(lcd_num >= 0) {
		tfloating = (lcd_num-(double)natural);
	}
	else {
		tfloating = -(lcd_num-(double)natural);
	}

	//--------------------------------
	if(lcd_num<0 && natural==0)
	{
		LCD_putc('-');
	}
	//--------------------------------

	lcd(natural);
	data_4bit('.');

	tfloating *= pow((double)10,fdigit);
	floating = (int)tfloating;
	//--------------------------------
	while(fdigit && floating)	{
		
		--fdigit;
		floating /= 10;
	}

	while(fdigit)	{

		--fdigit;
		data_4bit('0');
	}

void init_LCD(void)
{	DDRB = 0xff ;
	
	/* Replace with your application code */
	cmd(0x02) ;                             //return home
	cmd(0x28) ;								//4 bit mode
	cmd(0x0e) ;								//display on , cursor blinking
	cmd(0x06) ;								//cursor++
	cmd(0x01) ;							    //cls
	cmd(0x80) ;								//cursor beginning to 1st line 
	
	//data_4bit('a');
}
#endif //P_LCD