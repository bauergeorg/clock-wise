/*******************************************************************************
*
*	Author:			Georg Bauer (geo_bauer@gmx.net)
*	Date:			06.01.2014
*
*	Project-Title:	ClockWise
*	Description:	µC controlled clock with DCF77, RTC and a led-matrix
*
*	File-Title:		Real Time Clock (DS 1307)
*
*******************************************************************************
*/

/* libraries */
#include "rtc.h"

/* initialize real time clock via i2c */
void init_rtc()
{
	//! port c
	// activate PC0 and PC1 as output
	DDRC &= (1 << PC1) | (1 << PC0);
	// switch PC3 off
	PORTC &= (1 << PC1) | (1 << PC0);

}
