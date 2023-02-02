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
void RTC_Clock_Write(char, char, char, char);
void RTC_Read_Clock(char);
void RTC_Calendar_Write(char, char, char, char);
void RTC_Read_Calendar(char);
uint8_t IsItPM(char);
void initRtc(void);
uint8_t getTimeFromRtc(void);
uint8_t setTimeToRtc(char, char, char, char);