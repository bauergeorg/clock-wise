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
*/

//! libraries
#include "rtc.h"

//! initialize real time clock via i2c 
void initRtc()
{
	//! port c
	// activate PC0 and PC1 as output
	DDRC |= (1 << PC1) | (1 << PC0);
	// set PC0 and PC1 as high
	PORTC |= (1 << PC1) | (1 << PC0);
}


//! get time values from real time clock via i2c
uint8_t getTimeFromRtc()
{
	// return value: if rtc time received sucessful value is true,
	// if the rtc communication failed the return value is false
	uint8_t rtcTimeSuccessfulReceived = 0;
	
	return rtcTimeSuccessfulReceived;
}

//! set time values from real time clock via i2c
uint8_t setTimeToRtc()
{
	// return value: if rtc time set successful value is true,
	// if the rtc communication failed the return value is false
	uint8_t rtcTimeSuccessfulTransmitted = 0;
		
	return rtcTimeSuccessfulTransmitted;
}