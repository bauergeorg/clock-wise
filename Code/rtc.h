/*******************************************************************************
*
*	Author:			Georg Bauer
*	Date:			04.01.2016
*
*	Project-Title:	ClockWise
*	Description:	µC controlled clock with DCF77, RTC and a led-matrix
*
*	File-Title:		Real Time Clock (DS 1307) - Header File
*
*******************************************************************************
*/

//! libraries
#include <avr/io.h>
#include <stdint.h>

//! function declarations
void RTC_Clock_Write(char, char, char);
void RTC_Clock_Read(void);
void RTC_Calendar_Write(char, char, char, char);
void RTC_Calendar_Read(void);
uint8_t IsItPM(char);
void initRtc(void);
void updateTimeWithRtcValues(void);
void checkRtcTime(void);
void getTimeFromRtc(void);
void setTimeToRtc(char, char, char);