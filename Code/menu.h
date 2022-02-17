/*******************************************************************************
*
*	Author:			Georg Bauer
*	Date:			04.01.2016
*
*	Project-Title:	ClockWise
*	Description:	µC controlled clock with DCF77, RTC and a led-matrix
*
*	File-Title:		Menu Management - Header File
*
*******************************************************************************
*/

//! libraries
#include <avr/io.h>
#include <stdint.h>

//! Functional prototypes
void menuMgnt(uint8_t switches);
void menuCancel(void);