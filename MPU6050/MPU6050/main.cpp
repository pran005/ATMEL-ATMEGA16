/* 
 * MPU6050.cpp
 * FINAL CODE , STATUS : WORKING
 * Created: 18-06-2018 14:23:11
 * Always remember, OR MASKING may be lethal in I2C 
 * Author : Pranjal
 */ 
#define F_CPU 16000000UL
#include <avr/io.h>
#include <I2c.h>
#include <P_LCD.h>
#include <UART.h>
#define SMPLRT_DIV 0x19
#define CONFIG 0x1A 
#define GYRO_CONFIG 0x1B
#define ACCEL_CONFIG 0x1C
#define ACCEL_XOUT_H 0x3B 
#define INT_ENABLE 0x38 
#define PWR_MGMT_1 0x6B

int acc_x , acc_y , acc_z , gyro_x , gyro_y , gyro_z , temp ; 
void init_MPU()
{
	_delay_ms(150) ;															//start up time nearly 100ms
	I2c_start_karde(0xD0) ;														//MPU address
	I2c_write(SMPLRT_DIV) ; 
	I2c_write(0x07) ;															// 1khz , Sample = Gyro_rate/(1+SMPLRT_div)
	I2c_stop() ; 
	
	I2c_start_karde(0xD0) ; 
	I2c_write(PWR_MGMT_1) ;
	I2c_write(0x01)		  ;														//X_gyro ref
	I2c_stop() ;
	
	I2c_start_karde(0xD0) ;
	I2c_write(CONFIG) 	; 
	I2c_write(0x00)		;													    //GYRO_rate = 8khz
	I2c_stop()			; 
	
	I2c_start_karde(0xD0) ;
	I2c_write(GYRO_CONFIG);
	I2c_write(0x18) ;															// FSR = +\-2000 deg/sec 
	I2c_stop() ; 
	
	I2c_start_karde(0xD0);
	I2c_write(ACCEL_CONFIG) ;												    // FSR =  +\- 4g 
	I2c_write(0x08) ; 
	I2c_stop() ; 
	
	I2c_start_karde(0xD0) ; 
	I2c_write(INT_ENABLE) ; 
	I2c_write(0x01) ;
	I2c_stop() ;  
} 
void MPU_read_data()
{
	I2c_start_karde(0xD0);	
	I2c_write(ACCEL_XOUT_H);                                         // location address to read onwards from
	I2c_Repeat_start(0xD1);											 // SLA_R 
	 
	acc_x = (((int)I2c_Read_ack()<<8) | ((int)I2c_Read_ack())) ; 
	acc_y = (((int)I2c_Read_ack()<<8) | ((int)I2c_Read_ack())) ; 
	acc_z = (((int)I2c_Read_ack()<<8) | ((int)I2c_Read_ack())) ; 
	temp  = (((int)I2c_Read_ack()<<8) | ((int)I2c_Read_ack())) ; 
	gyro_x = (((int)I2c_Read_ack()<<8) | ((int)I2c_Read_ack())) ; 
	gyro_y = (((int)I2c_Read_ack()<<8) | ((int)I2c_Read_ack())) ; 
	gyro_z = (((int)I2c_Read_ack()<<8) | ((int)I2c_Read_nack())) ; 
	I2c_stop() ;   
}
int main(void)
{   
	float Ax=0 , Ay=0 , Az=0 , Gx, Gy , Gz , temperature ;
    init_I2c() ;
	init_MPU() ; 
	init_UART(9600) ; 
	init_LCD() ; 
    while (1) 
    {   MPU_read_data() ;
		 
		   
		Ax = acc_x/8192 ; 
		Ay = acc_y/8192 ; 
		Az = acc_z/8192 ; 
		
		Gx = gyro_x/16.4 ; 
		Gy = gyro_y/16.4 ; 
		Gz = gyro_z/16.4 ;
		
		temperature = (temp/340) + 36.54 ; 
		
		UART_Tx("Acc_x = ") ; 
		UART_Tx(Ax) ; 
		UART_Tx("g \t") ;
		
		UART_Tx("Acc_y = ") ;
		UART_Tx(Ay) ;
		UART_Tx("g \t") ;
		
		UART_Tx("Acc_z = ") ;
		UART_Tx(Az) ;
		UART_Tx("g ") ;
		
		UART_Tx("Temp=") ;
		UART_Tx(temperature) ;
		UART_Tx("   ") ; 
		
		UART_Tx("Gyro_x = ") ;
		UART_Tx(Gx) ;
		UART_Tx("deg/s") ;
		
		UART_Tx(" Gyro_y = ") ;
		UART_Tx(Gy) ;
		UART_Tx("deg/s \t") ;
		
		UART_Tx(" Gyro_z = ") ;
		UART_Tx(Gz) ;
		UART_Tx("deg/s \t") ;
		
		clr_lcd() ; 
		lcd(Ax) ; 
		lcd(" ") ; 
		lcd(temperature) ; 
		_delay_ms(200) ; 
		
		 
    }
}

