/*******************************************************************************
*
*	Author:			Georg Bauer
*	Date:			04.01.2016
*
*	Project-Title:	ClockWise
*	Description:	�C controlled clock with DCF77, RTC and a led-matrix
*
*	File-Title:		Real Time Clock (DS 1307)
*
*******************************************************************************
* see: https://www.electronicwings.com/avr-atmega/real-time-clock-rtc-ds1307-interfacing-with-atmega16-32
* Define 24 hour format!
*/

//! Own header
#include "system.h"
#include "rtc.h"
#include "i2c.h"

#define Device_Write_address	0xD0	/* Define RTC DS1307 slave write address */
#define Device_Read_address		0xD1	/* Make LSB bit high of slave address for read */

int second=0, minute=0, hour=0, weekday=0, day=0, month=0, year=0;

//! Extern globals variables
extern volatile struct time systemTime;
extern volatile struct systemParameter systemConfig;

/* function for clock */
void RTC_Clock_Write(uint8_t wr_hour, uint8_t wr_minute, uint8_t wr_second)
{
	uint8_t second_ones;
	uint8_t second_tens;
	uint8_t second_converted;
	second_ones = wr_second % 10;
	second_tens = (wr_second - second_ones) / 10;
	second_converted = ((second_tens << 4) & 0x70) | (second_ones & 0x0F);
	
	uint8_t minute_ones;
	uint8_t minute_tens;
	uint8_t minute_converted;
	minute_ones = wr_minute % 10;
	minute_tens = (wr_minute - minute_ones ) / 10;
	minute_converted = ((minute_tens << 4) & 0x70) | (minute_ones & 0x0F);
	
	uint8_t hour_ones;
	uint8_t hour_tens;
	uint8_t hour_converted;
	hour_ones = wr_hour % 10;
	hour_tens = (wr_hour - hour_ones ) / 10;
	hour_converted = ((hour_tens << 4) & 0x30) | (hour_ones & 0x0F);	// 24 hour format!
	
	I2C_Start(Device_Write_address);	/* Start I2C communication with RTC */
	I2C_Write(0);						/* Write 0 address for second */
	I2C_Write(second_converted);		/* Write second on 00 location */
	I2C_Write(minute_converted);		/* Write minute on 01(auto increment) location */
	I2C_Write(hour_converted);			/* Write hour on 02 location */
	I2C_Stop();							/* Stop I2C communication */
}

void RTC_Clock_Read(void)
{
	I2C_Start(Device_Write_address);			/* Start I2C communication with RTC */
	I2C_Write(0);								/* Write address to read */
	I2C_Repeated_Start(Device_Read_address);	/* Repeated start with device read address */

	uint8_t second_unconverted;
	uint8_t second_ones;
	uint8_t second_tens;

	uint8_t minute_unconverted;
	uint8_t minute_ones;
	uint8_t minute_tens;

	uint8_t hour_unconverted;	
	uint8_t hour_ones;
	uint8_t hour_tens;

	second_unconverted = I2C_Read_Ack();		/* Read second */
	minute_unconverted = I2C_Read_Ack();		/* Read minute */
	hour_unconverted = I2C_Read_Nack();			/* Read hour with Nack */
	
	second_ones = second_unconverted & 0x0F;
	second_tens = (second_unconverted & 0x70) >> 4;
	second = second_ones + second_tens * 10;
	
	minute_ones = minute_unconverted & 0x0F;
	minute_tens = (minute_unconverted & 0x70) >> 4;
	minute = minute_ones + minute_tens * 10;
	
	hour_ones = hour_unconverted & 0x0F;
	hour_tens = (hour_unconverted & 0x30) >> 4; // 24 hour format!
	hour = hour_ones + hour_tens * 10;
	
	I2C_Stop();									/* Stop i2C communication */
}

/* function for calendar */
void RTC_Calendar_Write(uint8_t wr_weekday, uint8_t wr_day, uint8_t wr_month, uint8_t wr_year)
{
	uint8_t weekday_converted;
	weekday_converted = wr_weekday & 0x07;
	
	uint8_t day_ones;
	uint8_t day_tens;
	uint8_t day_converted;
	day_ones = wr_day % 10;
	day_tens = (wr_day - day_ones ) / 10;
	day_converted = ((day_tens << 4) & 0x30) | (day_ones & 0x0F);
	
	uint8_t month_ones;
	uint8_t month_tens;
	uint8_t month_converted;
	month_ones = wr_month % 10;
	month_tens = (wr_month - month_ones ) / 10;
	month_converted = ((month_tens << 4) & 0x10) | (month_ones & 0x0F);

	uint8_t year_ones;
	uint8_t year_tens;
	uint8_t year_converted;
	year_ones = wr_year % 10;
	year_tens = (wr_year - year_ones ) / 10;
	year_converted = ((year_tens << 4) & 0xF0) | (year_ones & 0x0F);

	I2C_Start(Device_Write_address);	/* Start I2C communication with RTC */
	I2C_Write(3);						/* Write 3 address for day */
	I2C_Write(weekday_converted);		/* Write day on 03 location */
	I2C_Write(day_converted);			/* Write date on 04 location */
	I2C_Write(month_converted);			/* Write month on 05 location */
	I2C_Write(year_converted);			/* Write year on 06 location */
	I2C_Stop();							/* Stop I2C communication */
}

void RTC_Calendar_Read(void)
{
	I2C_Start(Device_Write_address);
	I2C_Write(3);
	I2C_Repeated_Start(Device_Read_address);

	uint8_t weekday_unconverted;

	uint8_t day_unconverted;
	uint8_t day_ones;
	uint8_t day_tens;

	uint8_t month_unconverted;
	uint8_t month_ones;
	uint8_t month_tens;

	uint8_t year_unconverted;
	uint8_t year_ones;
	uint8_t year_tens;

	weekday_unconverted = I2C_Read_Ack();	/* Read day */
	day_unconverted = I2C_Read_Ack();		/* Read date */
	month_unconverted = I2C_Read_Ack();		/* Read month */
	year_unconverted = I2C_Read_Nack();		/* Read the year with Nack */
	
	weekday = weekday_unconverted & 0x07;

	day_ones = day_unconverted & 0x0F;
	day_tens = (day_unconverted & 0x30) >> 4;
	day = day_ones + day_tens * 10;
	
	month_ones = month_unconverted & 0x0F;
	month_tens = (month_unconverted & 0x10) >> 4;
	month = month_ones + month_tens * 10;
	
	year_ones = year_unconverted & 0x0F;
	year_tens = (year_unconverted & 0xF0) >> 4;
	year = year_ones + year_tens * 10;
	
	I2C_Stop();					/* Stop i2C communication */
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

//! check if rtc device has valid time/calendar values
// set system status
// - xxxx.x0xxb rtc time is not available
// - xxxx.x1xxb rtc time is available
void checkRtcTime()
{
	// read values
	RTC_Clock_Read();
	RTC_Calendar_Read();
		
	// check if rtc time is available
	if((second == 0) && (minute == 0) && (hour == 0))
	{
		// set system status
		// - xxxx.x0xxb rtc time is not available
		systemConfig.status &= ~0x04;
	}
	else
	{
		// set system status
		// - xxxx.x1xxb rtc time is available
		systemConfig.status |= 0x04;
	}
}

//! Update sytem time and status
// set system status
// - xxxx.xxx1b time value available
// - xxxx.x1xxb rtc time is available
void updateTimeWithRtcValues(void)
{
	// read values
	getTimeFromRtc();
	
	// update system time
	systemTime.hour = hour;
	systemTime.minute = minute;
	systemTime.second = second;

	systemTime.day = day;
	systemTime.month = month;
	systemTime.year = year;
	systemTime.weekday = weekday;
	
	// set default system status
	// - xxxx.xxx1b time value available
	// - xxxx.x1xxb rtc time is available
	systemConfig.status |= 0x05;

}

//! get time values from real time clock via i2c
void getTimeFromRtc(void)
{
	RTC_Clock_Read();		/* Read clock with second add. i.e location is 0 */
	RTC_Calendar_Read();	/* Read calendar */
}


//! set time values from real time clock via i2c
void setTimeToRtc(uint8_t HH, uint8_t MM, uint8_t SS, uint8_t WEEKDAY, uint8_t DAY, uint8_t MONTH, uint8_t YEAR)
{
	RTC_Clock_Write(HH, MM, SS);
	RTC_Calendar_Write(WEEKDAY, DAY, MONTH, YEAR);	
}
