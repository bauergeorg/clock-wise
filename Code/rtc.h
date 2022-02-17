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
void initRtc(void);
uint8_t getTimeFromRtc(void);
uint8_t setTimeToRtc(void);