/*
 * ESP8266.cpp
 * Final Code_TCP CLIENT DONOT DELETE 
 * Status : WORKING 
 * Created:		21-06-2018 18:14:00
 * Last Edited: 24-06-2018 1:56:32
 * Author : Pranjal
 */ 
#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <P_LCD.h>
#include <UART.h>
#include <util/delay.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define SREG _SFR_IO8(0x3F)  
#define def_buff_size 100 
#define def_time  10000

/*****THINGSPEAK*****/
#define DOMAIN "api.thingspeak.com"
#define PORT   "80"
#define WRITE_KEY "ACAYAXT6W3SLMVJB"
#define Channel_ID  "524595"

/***AP WIFI KEY***/

#define SSID "D-Link" 
#define password "shivanshi1*"

/******WIFI MODES******/
#define Station 1 
#define SoftAP  2 
#define Both    3 

/***Connection Modes***/ 
#define Single 0 
#define Multi  1

/*****ESP MODE*****/
#define Normal 0 
#define UART_passthru 1 

 
enum Responses{
	 starting,
	 waiting,
	 finished,
	 time_out,
	 error
	};
	
enum Wifi{
	WIFI_CONNECTED,
	CONNECTION_TIMEOUT,
	WRONG_PASSWORD,
	NOT_FOUND,
	CONNECTION_FAILED,
	UNKNOWN_ERROR	
	};
	
enum TCP_STATUS {
	CONNECTED_TO_AP,
	TRANSMISSION_ENGAGED,
	TRANSMISSION_DISCONNECTED,
	NOT_CONNECTED_TO_AP,
	TCP_UNKNOWN_ERROR
};	
int8_t stat ; 
volatile int numbytes=0 , point_loc=0 ;
int32_t timeout=0; 
char Rx_buff[def_buff_size] ; 

void check_response(char* act_response)
{
	uint8_t act_response_len = strlen(act_response) ; 
	uint16_t rx_res_len , countms=0 ; 
	char Rx_response[act_response_len] ; 
	while(1)
	{
		if(countms >= def_time) 
		{
			countms = 0 ; 
			stat= time_out ; //waiting 
			return ; 
		}
		if(stat==starting)
		{   
			 
			stat=waiting ;
			 
		}															 
		rx_res_len = strlen(Rx_buff) ;
		//lcd(rx_res_len) ;  
		if(rx_res_len)
		{
			_delay_ms(1) ; 
			countms++ ; 

			if(rx_res_len==strlen(Rx_buff))
			{
				for(uint16_t i=0; i<rx_res_len; i++)
				{
				     
					memmove(Rx_response,Rx_response+1,act_response_len-1) ; 
					Rx_response[act_response_len-1]= Rx_buff[i] ;  
					
					if(!(strncmp(Rx_response,act_response,act_response_len)))
					{
						countms = 0 ; 
						stat = finished ; 
						return ; 
					}
				}
			}
		}
		_delay_ms(1);
		countms++ ;
	}	
}
void clear_buff()
{
	memset(Rx_buff,0,def_buff_size); 
	numbytes = 0 ; point_loc=0; 
}
void read_response(char* expected_rx)
{   
	stat=starting ; 
	do 
	{	
		check_response(expected_rx) ;
		  
		
	} while (stat==waiting);
}
bool WaitforRx(char *expected_rx)
{
	read_response(expected_rx) ; 
	if(stat != time_out)
		return 1;  		 
	return 0; 
}
bool AT_Communicate(char* AT_Com, char* expected_response)
{
	clear_buff() ; 
	UART_Tx(AT_Com) ; 
	UART_Tx("\r\n") ; 
	return WaitforRx(expected_response);
}
bool Wifi_begin()
{
	for(uint8_t i = 0 ; i<5 ; i++)
	{
		
	if(AT_Communicate("AT","\r\nOK\r\n"))
		return 1 ;
	}
	return 0 ; 	
}
bool Close_TCP()
{
	return AT_Communicate("AT+CIPCLOSE=1","\r\nOK\r\n") ; 
}
bool ESP_Mode(uint8_t Mode)
{
	char AT[50] ; 
	memset(AT,0,50) ; 
	sprintf(AT,"AT+CIPMODE=%d",Mode) ; 
	AT[49] = 0 ; 
	return AT_Communicate(AT,"\r\nOK\r\n") ; 
}
bool Wifi_mode(uint8_t Mode)
{
	char AT[50] ;
	memset(AT,0,50) ;
	sprintf(AT,"AT+CWMODE=%d",Mode) ;
	AT[49] = 0 ;
	return AT_Communicate(AT,"\r\nOK\r\n") ;
}
bool TCP_mode(uint8_t Mode)
{
	char AT[50] ;
	memset(AT,0,50) ;
	sprintf(AT,"AT+CIPMUX=%d",Mode) ;
	AT[49] = 0 ;
	return AT_Communicate(AT,"\r\nOK\r\n") ;
}

uint8_t JoinAP(char*SSID,char* passkey)
{
	char AT[50] ;
	memset(AT,0,50) ;
	sprintf(AT, "AT+CWJAP_CUR=\"%s\",\"%s\"",SSID,passkey) ;
	AT[49] = 0 ;
    if(AT_Communicate(AT,"\r\nWIFI CONNECTED\r\n")) 
		return	WIFI_CONNECTED ; 
	else if(strstr(Rx_buff,"+CWJAP:1"))
		return CONNECTION_TIMEOUT;	
	else if(strstr(Rx_buff,"+CWJAP:2"))
		return WRONG_PASSWORD ;
	else if(strstr(Rx_buff,"+CWJAP:3"))
		return NOT_FOUND ; 
	else if(strstr(Rx_buff, "+CWJAP:4"))
		return CONNECTION_FAILED;
	else
		return UNKNOWN_ERROR;			 
}
uint8_t TCP_stat()
{
	AT_Communicate("AT+CIPSTATUS", "\r\nOK\r\n");
	if(strstr(Rx_buff, "STATUS:2"))
	return CONNECTED_TO_AP;
	else if(strstr(Rx_buff, "STATUS:3"))
	return TRANSMISSION_ENGAGED;
	else if(strstr(Rx_buff, "STATUS:4"))
	return TRANSMISSION_DISCONNECTED;
	else if(strstr(Rx_buff, "STATUS:5"))
	return NOT_CONNECTED_TO_AP;
	else
	return TCP_UNKNOWN_ERROR;
}
uint8_t TCP_start(uint8_t Connect_number,char* domain, char* port)
{
	bool response ; 
	char AT[70] ; 
	memset(AT,0,70) ; 
	AT[69]= 0 ; 
	if(AT_Communicate("AT+CIPMUX?", "CIPMUX:0"))														//check connection mode
		sprintf(AT,"AT+CIPSTART=\"TCP\",\"%s\",%s", domain, port);
	else
		sprintf(AT,"AT+CIPSTART=\"%d\",\"TCP\",\"%s\",%s",Connect_number, domain, port);
	response = AT_Communicate(AT,"CONNECT\r\n") ; 
	if(!response)
	{	
		if(stat==time_out)
			return time_out ; 
		return error ; 	
	}
	return finished;
}
uint8_t send_data(char* data)
{
	char AT[50] ; 
	memset(AT,0,50) ; 
	sprintf(AT,"AT+CIPSEND=%d",(strlen(data)+2));
	AT[49] = 0; 
	AT_Communicate(AT,"\r\nOK\r\n>") ; 
	if(!AT_Communicate(data,"\r\nSEND OK\r\n"))
	{
		if (stat==time_out)
			return time_out ; 
		else 
			return error ; 	
	}
	return finished ; 
}
uint8_t TCP_server(uint8_t number, uint8_t port)
{
	char AT[50] ;
	memset(AT,0,50) ;
	AT[49] = 0 ;
	sprintf(AT,"AT+CIPSERVER=%d,%d",number,port);
	AT_Communicate(AT,"\r\nOK\r\n") ;
		
}
void STATION_IP()
{
	char AT[50] = "AT+CIFSR" ;
	//memset(AT,0,50) ;
	AT[49] = 0 ;
	AT_Communicate(AT,"\r\nOK\r\n") ;
}
void close_SERVER()
{
	char AT[50] = "AT+CIPCLOSE=0" ;
	//memset(AT,0,50) ;
	AT[49] = 0 ;
	AT_Communicate(AT,"\r\nOK\r\n") ;
}

int data_flag()
{
	return numbytes - point_loc ;  
}
char Read_buff()
{
	if(numbytes>point_loc)
	{
		return Rx_buff[numbytes++];
	}
	else 
	{
		clear_buff() ;
		return 0 ; 
	}
}
uint8_t Read_data(char *string)
{
	uint16_t bytesRead=0 ; 
	_delay_ms(100) ; 
	while(data_flag()>0) 
	string[bytesRead++] = Read_buff() ; 
	return bytesRead ; 
}
ISR(USART_RXC_vect)
{ 
	
	uint8_t sreg = SREG ; 
  	cli() ; 
	Rx_buff[numbytes] = UDR ;
	numbytes++ ;  
	if(numbytes == def_buff_size)
	{	numbytes = 0 ;
		point_loc = 0 ; 
	}
	SREG = sreg ; 
	
}
int main(void)
{
	char str[60]  ;
	uint8_t connect_stat ; 
	uint16_t data=15 ; 
    init_UART(9600) ; 
	sei() ;
	init_LCD() ;   
	while(!Wifi_begin()) ;  
	ESP_Mode(Normal) ; 
	Wifi_mode(Both) ;
	TCP_mode(Single) ; 
	if(TCP_stat()==NOT_CONNECTED_TO_AP)
	JoinAP("D-Link","shivanshi1*") ; 
	clr_lcd();
	lcd("b4");
	TCP_start(0,DOMAIN,PORT) ; 
   	
    while (1) 
    {
		lcd("in while") ;
		 connect_stat=TCP_stat() ; 
		 if(connect_stat == NOT_CONNECTED_TO_AP)
		 JoinAP("D-Link","shivanshi1*") ;
		// TCP_server(1,80) ; 
		// STATION_IP() ; 
		 if(connect_stat == TRANSMISSION_DISCONNECTED)
		 TCP_start(0,DOMAIN,PORT) ; 
		 memset(str,0,50) ;  
		 sprintf(str,"GET /update?api_key=%s&field1=%d",WRITE_KEY,data++);
		 send_data(str) ; 
		 _delay_ms(15000) ; 
		 //close_SERVER() ; 
    }
}

