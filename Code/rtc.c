/*******************************************************************************
*
*	Author:			Georg Bauer
*	Date:			04.01.2016
*
*	Project-Title:	ClockWise
*	Description:	µC controlled clock with DCF77, RTC and a led-matrix
*
*	File-Title:		Real Time Clock (DS 1307)
*
*******************************************************************************
* see: https://www.electronicwings.com/avr-atmega/real-time-clock-rtc-ds1307-interfacing-with-atmega16-32
*/

//! libraries
#include "rtc.h"
#include "i2c.h"

#define Device_Write_address	0xD0	/* Define RTC DS1307 slave write address */
#define Device_Read_address		0xD1	/* Make LSB bit high of slave address for read */
#define TimeFormat12			0x40	/* Define 12 hour format */
#define AMPM					0x20

int second,minute,hour,day,date,month,year;

/* function for clock */
void RTC_Clock_Write(char _hour, char _minute, char _second, char AMPM)
{
	_hour |= AMPM;
	I2C_Start(Device_Write_address);/* Start I2C communication with RTC */
	I2C_Write(0);			/* Write 0 address for second */
	I2C_Write(_second);		/* Write second on 00 location */
	I2C_Write(_minute);		/* Write minute on 01(auto increment) location */
	I2C_Write(_hour);		/* Write hour on 02 location */
	I2C_Stop();				/* Stop I2C communication */
}

void RTC_Read_Clock(char read_clock_address)
{
	I2C_Start(Device_Write_address);/* Start I2C communication with RTC */
	I2C_Write(read_clock_address);	/* Write address to read */
	I2C_Repeated_Start(Device_Read_address);/* Repeated start with device read address */

	second = I2C_Read_Ack();	/* Read second */
	minute = I2C_Read_Ack();	/* Read minute */
	hour = I2C_Read_Nack();	/* Read hour with Nack */
	I2C_Stop();				/* Stop i2C communication */
}

/* function for calendar */
void RTC_Calendar_Write(char _day, char _date, char _month, char _year)
{
	I2C_Start(Device_Write_address);/* Start I2C communication with RTC */
	I2C_Write(3);			/* Write 3 address for day */
	I2C_Write(_day);			/* Write day on 03 location */
	I2C_Write(_date);		/* Write date on 04 location */
	I2C_Write(_month);		/* Write month on 05 location */
	I2C_Write(_year);		/* Write year on 06 location */
	I2C_Stop();				/* Stop I2C communication */
}


void RTC_Read_Calendar(char read_calendar_address)
{
	I2C_Start(Device_Write_address);
	I2C_Write(read_calendar_address);
	I2C_Repeated_Start(Device_Read_address);

	day = I2C_Read_Ack();		/* Read day */
	date = I2C_Read_Ack();	/* Read date */
	month = I2C_Read_Ack();	/* Read month */
	year = I2C_Read_Nack();	/* Read the year with Nack */
	I2C_Stop();				/* Stop i2C communication */
}


bool IsItPM(char hour_)
{
	if(hour_ & (AMPM))
	return 1;
	else
	return 0;
}

//! initialize real time clock via i2c 
void initRtc()
{
	//! port c: PC0=SCL, PC1=SDA
	// activate PC0 and PC1 as output
	DDRC |= (1 << PC1) | (1 << PC0);
	// set PC0 and PC1 as high
	PORTC |= (1 << PC1) | (1 << PC0);
}

//==============================================
//            RTC1307_READ_WRITE             //
//==============================================
int Write_RTC(char addr, char data)
{
	I2C_START_TX(0b11010000); //device add.
	i2c_transmit(add); //Reg. add.
	i2c_transmit(data1);
	i2c_stop();
	
	//uint8_t databyte;
	TWIStart();
	if (TWIGetStatus() != 0x08)
		return 1;
	// select device: device address
	TWIWrite(0b11010000);
	if (TWIGetStatus() != 0x18)
		return 1;
	//send start
	TWIStart();
	if (TWIGetStatus() != 0x10)
		return 1;
	//select devise and send read bit
	TWIWrite((EEDEVADR)|((uint8_t)((u16addr & 0x0700)>>7))|1);
	if (TWIGetStatus() != 0x40)
		return 1;
	data = TWIReadNACK();
	if (TWIGetStatus() != 0x58)
		return 1;
	TWIStop();
		return 0;
}
	
}


//! get time values from real time clock via i2c
uint8_t getTimeFromRtc()
{
	// return value: if rtc time received sucessfull value is true,
	// if the rtc communication failed the return value is false
	uint8_t rtcTimeSuccessfulReceived = 0;
	

	uint8_t hour, min, sec;
	rtc_get_time_s(&hour, &minute, &second);

	
	return rtcTimeSuccessfulReceived;
}


//! set time values from real time clock via i2c
uint8_t setTimeToRtc(char HH,char MM, char SS, char ampm)
{
	// return value: if rtc time set successful value is true,
	// if the rtc communication failed the return value is false
	uint8_t rtcTimeSuccessfulTransmitted = 0;
	
	sprintf(mystr,"%03d",SS);
	Write_RTC(0x00,((mystr[1] - 0x30) << 4) | (mystr[2] - 0x30));

	sprintf(mystr,"%03d",MM);
	Write_RTC(0x01,((mystr[1] - 0x30) << 4) | (mystr[2] - 0x30));
	 
	sprintf(mystr,"%03d",HH);
	if(ampm == 1)
	 {
		 Write_RTC(0x02,((((mystr[1] - 0x30) << 4) | (mystr[2] - 0x30)) | 0x40) | 0x20);
	 }
	else
	 {
		 Write_RTC(0x02,((((mystr[1] - 0x30) << 4) | (mystr[2] - 0x30)) | 0x40));
	 }
				
	return rtcTimeSuccessfulTransmitted;
}