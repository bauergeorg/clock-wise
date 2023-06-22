/*******************************************************************************
*
*	Author:			Georg Bauer 
*	Date:			19.06.2014
*
*	Project-Title:	ClockWise
*	Description:	Summary of tasks
*
*	File-Title:		Tasks
*
*******************************************************************************
*/

//! Libraries
#include "taskMgnt.h"
#include "settings.h"
#include "system.h"
#include "gpios.h"
#include "adc.h"
#include "rtc.h"
#include "displayMatrix.h"
#include "ledMatrix.h"
#include "usart0.h"

//! Extern global variables
extern volatile struct systemParameter systemConfig;
extern volatile struct time systemTime;
extern volatile struct row actualMatrix[12];

//! Task hour
void taskHour(void)
{
	
}

//! Task 13 Minute
void taskThirteenMinute(void)
{
	
}

//! Task 15 minute
void taskFifteenMinute(void)
{
	
}

//! Task 10 minute
void taskTenMinute(void)
{
	
}

//! Task 5 minute
void taskFiveMinute(void)
{

}

//! Task minute
void taskMinute(void)
{
	// read data from rtc and set local time
	//updateTimeWithRtcValues();
}

//! Task second
void taskSecond(void)
{
	// toggle status led
	toggleStatusGreen();
	
	usart0ReceiveTransmit(0x0d); // CR
	usart0ReceiveTransmit(0x0a); // LF
	
	usart0ReceiveTransmit(0x43); // C
	usart0ReceiveTransmit(0x75); // u
	usart0ReceiveTransmit(0x72); // r
	usart0ReceiveTransmit(0x72); // r
	usart0ReceiveTransmit(0x65); // e
	usart0ReceiveTransmit(0x6e); // n
	usart0ReceiveTransmit(0x74); // t
	usart0ReceiveTransmit(0x20); // (space)
	usart0ReceiveTransmit(0x54); // T
	usart0ReceiveTransmit(0x69); // i
	usart0ReceiveTransmit(0x6d); // m
	usart0ReceiveTransmit(0x65); // e
	usart0ReceiveTransmit(0x3a); // :
	usart0ReceiveTransmit(0x20); // (space)
	usart0ReceiveTransmit(systemTime.hour);
	usart0ReceiveTransmit(systemTime.minute);
	usart0ReceiveTransmit(systemTime.second);
	usart0ReceiveTransmit(0x0d); // CR
	usart0ReceiveTransmit(0x0a); // LF

	usart0ReceiveTransmit(0x43); // C
	usart0ReceiveTransmit(0x75); // u
	usart0ReceiveTransmit(0x72); // r
	usart0ReceiveTransmit(0x72); // r
	usart0ReceiveTransmit(0x65); // e
	usart0ReceiveTransmit(0x6e); // n
	usart0ReceiveTransmit(0x74); // t
	usart0ReceiveTransmit(0x20); // (space)
	usart0ReceiveTransmit(0x44); // D
	usart0ReceiveTransmit(0x61); // a
	usart0ReceiveTransmit(0x74); // t
	usart0ReceiveTransmit(0x65); // e
	usart0ReceiveTransmit(0x3a); // :
	usart0ReceiveTransmit(0x20); // (space)
	usart0ReceiveTransmit(systemTime.year);
	usart0ReceiveTransmit(systemTime.month);
	usart0ReceiveTransmit(systemTime.day);
	usart0ReceiveTransmit(systemTime.weekday);
	usart0ReceiveTransmit(0x0d); // CR
	usart0ReceiveTransmit(0x0a); // LF
	
}

//! Task half second
void taskHalfSecond(void)
{
	// display information on matrix, called by half second interrupt (time management)
	displayMatrixInformation(0);
}