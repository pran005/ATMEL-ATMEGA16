/* FINAL CODE DND  
 * STATUS : FULL DUPLEX WORKING 
 * SIM900_GSM.cpp
 * Last Edited : 26-06-2018 02:08:34 
 * Created: 25-06-2018 13:02:49
 * Author : Pranjal
 */ 
#define  F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <UART.h>
#include <P_LCD.h>
#include <avr/interrupt.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define SREG _SFR_IO8(0x3F)
#define buff_size 160
#define def_timeout 60000

enum Responses{
	starting,
	waiting,
	finished,
	timeout,
	error
};

char Rx_buff[buff_size] ; 
volatile int buff_ptr= 0,ptr_loc=0 ;
int msg_index = 0 ; 
uint16_t timecount = 0 ; 
int8_t stat ; 
int flag= 0 ;
char message[30] ; char mob_no[15] ;
  
void check_response(char * expected_resp)
{
	uint8_t expected_len = strlen(expected_resp) ; 
	uint16_t response_len ;
	char response[response_len] ; 
	while(1)
	{
		if(timecount>=def_timeout)
		{
			timecount = 0 ; 
			stat=timeout ; 
			return  ; 
		}
		if(stat==starting)
			stat=waiting ; 
		response_len=strlen(Rx_buff) ; 
		if(response_len)	
		{
			_delay_ms(2) ; 
			timecount++  ; 
			if(response_len==strlen(Rx_buff))
			{
				for(uint16_t i = 0 ; i<response_len ; i++)
				{
					memmove(response,response+1,response_len-1) ; 
					response[response_len-1] = Rx_buff[i]  ;
					
					if(strstr(response,expected_resp))
					{
						timecount = 0 ; 
						stat = finished ; 
						return ; 
					}
				}
			}
		}
		 _delay_ms(2) ; 
		   timecount++ ; 
	} 	
}
void clear_buff()
{
	memset(Rx_buff,0,strlen(Rx_buff));
	buff_ptr = 0 ; ptr_loc=0 ; 
}
void read_response(char* expected_rx)
{
	stat=starting ;
	do
	{
		check_response(expected_rx) ;
				
	} while (stat==waiting);
}

bool WaitforRx(char * expected_resp)
{
	if(stat!=timeout)
		return 1 ; 
	return false ; 
}
bool AT_Communicate(char* AT_cmd , char * expected_rx)
{
	clear_buff() ; 
	UART_Tx(AT_cmd) ; 
	UART_Tx("\r") ;
	return WaitforRx(expected_rx) ; 
}
int data_flag()
{
	return buff_ptr-ptr_loc ; 
}
char read_buffer()
{
	if(buff_ptr>ptr_loc)
		return Rx_buff[ptr_loc++] ; 
	else
	{
		clear_buff() ; 
		return 0 ; 
	}	
	
}
bool GSM_begin()
{
	for(uint8_t i = 0 ; i<5 ; i++)
	{
		if(AT_Communicate("AT","OK\r\n"))  
			return 1; 
	}
	return 0 ; 
}
bool Msg_mode(uint8_t mode) 
{    
	clear_buff() ; 
	char AT[20] ;
	memset(AT,0,20) ; 
	sprintf(AT,"AT+CMGF=%d",mode) ; 
	return AT_Communicate(AT,"OK") ;
}
bool Expect_Msg()
{
	  char msg_num[4] ;
	  int i=0 ;
	  //clear_buff() ;  clear buff will clear the Rx_buff, so CMTI: is lost !! 													
	  _delay_ms(500) ;
	  buff_ptr = 0 ;
	  while(1)
	  {
		  if(Rx_buff[buff_ptr]=='\r' || Rx_buff[buff_ptr]=='\n')
		  buff_ptr++ ;
		  else
		  break ;
	  }
	  if(strstr(Rx_buff,"CMTI:"))
	  {
		  while(Rx_buff[buff_ptr] != ',')
		  buff_ptr++ ;
		  buff_ptr++ ;
		  while(Rx_buff[buff_ptr]!= '\r')
		  {
			  msg_num[i] = Rx_buff[buff_ptr] ;
			  buff_ptr++ ;
			  i++ ;
		  }
		  msg_index = atoi(msg_num) ;
		  clear_buff();
		  return 1 ;
	  }
	  else
	  return 0 ;
}
void delete_msg(uint8_t position)
{	clear_buff() ; 
	char AT[20] ; 
	memset(AT,0,20) ; 
	sprintf(AT,"AT+CMGD=%d",position) ;
	AT_Communicate(AT,"OK") ; 
}
void delete_all()
{	
	clear_buff() ; 
	AT_Communicate("AT+CMGDA=\"DEL ALL\"","OK") ;
}
bool call(char * num)
{	clear_buff() ; 
	char AT[20] ;
	memset(AT,0,20) ;
	sprintf(AT,"ATD%s;",num) ; 
	return AT_Communicate(AT,"OK") ; 
}
bool Hangup()
{	clear_buff() ; 
	return AT_Communicate("ATH","OK") ; 
}
void send_msg(char* num , char* sms)
{
	clear_buff() ; 
	char AT[35] ; 
	sprintf(AT,"AT+CMGS=\"%s\"",num) ; 
	(AT_Communicate(AT,">"));
	_delay_ms(200) ; 
	while(1)
	{
		if(Rx_buff[buff_ptr]==0x3E)
		{
			buff_ptr = 0;
			memset(Rx_buff,0,strlen(Rx_buff));
			UART_Tx(sms); 
			UART_Tx(0x1a); 
			break;
		}
		buff_ptr++;
	}
	_delay_ms(300) ; 
	clear_buff() ; 
}
void read_msg(uint8_t pos)
{
	//clear_buff() ;  buffer khali karna jaanlewa sabit ho sakta hai
	char AT[20] ; 
	sprintf(AT,"AT+CMGR=%d",pos) ;
	AT_Communicate(AT,"+CMGR:") ; 
	_delay_ms(500) ;  
	if(!(strstr(Rx_buff,"+CMGR:"))) 
	{
		clr_lcd();
		lcd("no msg") ;
	}
	else 
	{	buff_ptr=0;
		while(1)
		{
							
				if(Rx_buff[buff_ptr]=='\r' || Rx_buff[buff_ptr]=='\n')
				buff_ptr++ ;
				else
				break ;
				
		}
		while(Rx_buff[buff_ptr] != ',')
		{
			buff_ptr++ ; 
		}
		buff_ptr+=2 ;
		
		for(int i=0; i<=12 ;i++)
		{
			mob_no[i] = Rx_buff[buff_ptr] ; 
			buff_ptr++ ; 
		} 
		do
		{
			buff_ptr++;
		}while(Rx_buff[buff_ptr-1]!= '\n');
		clr_lcd() ; 
		int i=0;
		while(Rx_buff[buff_ptr]!= '\r' && i<31)
		{
			lcd(Rx_buff[buff_ptr]) ; 
			message[i]=Rx_buff[buff_ptr];
			
			buff_ptr++;
			i++;
			if(i==15)
			cmd(0xC0);
		}
		flag=0 ; 
		clear_buff() ; 
	}  
	
}
uint8_t read_data(char *string)
{
	int readbytes=0 ;
	while(data_flag()>0)
	string[readbytes++] = read_buffer() ;
	return readbytes;
}
void get_IMEI()
{
	char str[15] ;
	memset(str,0,15);
	memset(Rx_buff,0,160) ;
	AT_Communicate("AT+GSN","\r\nOK") ; 
	read_data(str) ;
	clr_lcd();
	lcd("IMEI : ") ;
	cmd(0xC0) ;
	lcd(str) ;
	_delay_ms(800) ;
}
ISR(USART_RXC_vect)
{
	uint8_t sreg = SREG ; 
	Rx_buff[buff_ptr++] = UDR ; 
	if(buff_ptr==buff_size)
	buff_ptr = 0 ;
	flag=1 ;  
	SREG = sreg ; 
}
int main(void)
{
	buff_ptr = 0 ;
    int msg ;
	memset(message, 0, 30);  
	init_UART(9600) ; 
	init_LCD() ; 
	sei() ; 
	
	GSM_begin() ; 
	Msg_mode(1) ;
	_delay_ms(1000) ; 
	
    while (1) 
    {
		if(flag==1){
			//lcd("in if") ; 
			msg= Expect_Msg() ; 
			if(msg== true)
			{
				clr_lcd() ; 
				lcd("NEW MSG") ; 
				_delay_ms(1000);
				clr_lcd() ; 
				read_msg(msg_index) ;  
				_delay_ms(3000);
				
				
				if(strstr( message,"call me"))
				{
					
					call("9887175064"); 
					clr_lcd() ; 
					lcd("calling") ; 
					_delay_ms(15000);
					Hangup(); 
				}
				else if(strstr( message,"reply me"))
				{
					
					send_msg("9887175064","HEY!"); 
					clr_lcd() ;
					lcd("sending...") ;
					_delay_ms(15000);
				}
				
				
				clr_lcd() ; 
				delete_msg(msg_index) ;
				_delay_ms(1000);
				
			}
			
			
			msg=0;
			flag=0;
			clr_lcd() ; 
		}
		clr_lcd() ; 
		lcd("HUKUM Kro!");
		_delay_ms(700) ; 
		memset(mob_no, 0, 15);
		memset(message, 0, 30);
    }
}

