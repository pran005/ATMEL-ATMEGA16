/*
 * ADXL.cpp
 * Status : WORKING , DO NOT DELETE , Bluetooth Data Logger (MULTI-Channel-ADC) 
 * Created: 15-06-2018 16:39:14
 * Author : Pranjal
 * Last Edited : 15-06-2018 22:07:15 
 */ 
#define F_CPU 16000000UL
#include <avr/io.h>
#include <P_LCD.h>
#include <UART.h>

void init_ADC()
{
	DDRA = 0x80 ; 
	ADCSRA = 0x87 ;								// FCPU/128 , ADEN
	ADMUX |= (1<<REFS0)  ;						// VREF = AVcc
}

int ADC_val(char channel)
{	 //ADCW &= 0x00 ; 
	ADMUX =(channel & 0x07) ;
	ADCSRA |=(1<<ADSC) ;
	while(!(ADCSRA&(1<<ADIF))) ;
	ADCSRA |= (1<<ADIF);
	_delay_ms(1);
	return ADCW ;
}
void bhej (int m)
{
	int i=0, flag=0,r  ;
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
	UART_Tx(str) ;
}


int main(void)
{  init_ADC() ; 
	init_LCD() ;
	init_UART(38400); 
	int x[3] ;  
    
    while (1) 
    {	//clr_lcd() ; 
		//x=ADC_val(0);
		//UART_Tx("x="); 
		//bhej(x) ; 
		//lcd(x) ; 
	    //y=ADC_val(1);
		//UART_Tx("y=") ; 
		//bhej(y) ; 
		//_delay_ms(200) ; 
		//lcd(" ") ;
		//lcd(y);
		//z=ADC_val(2) ;
		//UART_Tx("z=") ; 
		//bhej(z) ;  
		for(int i=0 ; i<3 ; i++)																//BAAP MOVE , MUST REMEMBER!
		{
			x[i] = ADC_val(i) ; 
			switch(i){
				case 0 : UART_Tx(" X= ") ; 
				break ; 
				case 1 : UART_Tx(" Y= ") ; 
				break ; 
				case 2 : UART_Tx(" Z= ") ; 
				break ; 
				//case 3 : UART_Tx("M= ") ; 
				//break ; 
			}
			
			bhej(x[i]) ; 
			_delay_ms(200) ; 
		} 
		//cmd(0xC0) ;  
		//lcd(z);
		//_delay_ms(200) ;  
    }
}

