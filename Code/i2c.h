/*
 * i2c.h
 *
 * Created: 19.01.2023 12:25:05
 *  Author: bauer
 */ 


/*
 * I2C_Master_H_file.h
 * www.electronicwings.com
 */ 

#include "settings.h"							/* Include header file */

//#define F_CPU 16000000UL						/* Define CPU clock Frequency e.g. here its 8MHz */
#include <avr/io.h>								/* Include AVR std. library file */
#include <util/delay.h>							/* Include inbuilt defined Delay header file */
#include <math.h>								/* Include math function */
#define SCL_CLK 100000L							/* Define SCL clock frequency */
#define BITRATE(TWSR)	((F_CPU/SCL_CLK)-16)/(2*pow(4,(TWSR&((1<<TWPS0)|(1<<TWPS1))))) /* Define bit rate */

void I2C_Init();
uint8_t  I2C_Start(char);
uint8_t  I2C_Repeated_Start(char);
void I2C_Stop();
void I2C_Start_Wait(char );
uint8_t  I2C_Write(char);
char I2C_Read_Ack();
char I2C_Read_Nack();
