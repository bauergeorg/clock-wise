/*******************************************************************************
*
*	Author:			Georg Bauer (geo_bauer@gmx.net)
*	Date:			06.01.2014
*
*	Project-Title:	ClockWise
*	Description:	µC controlled clock with DCF77, RTC and a led-matrix
*
*	File-Title:		Real Time Clock (DS 1307) - Header File
*
*******************************************************************************
*/

/* libraries */
#include <avr/io.h>
#include <stdint.h>

/* function declarations */
void init_rtc(void);