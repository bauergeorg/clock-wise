/*******************************************************************************
*
*	Author:			Georg Bauer
*	Date:			06.01.2014
*
*	Project-Title:	ClockWise
*	Description:	Status LEDs, Switches and other LED outputs (char and dots)
*
*	File-Title:		General Purpose Input/Outputs - Header File
*
*******************************************************************************
*/

//! Libraries
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>

//! Functional prototypes
void initGpios(void);

void toggleStatusGreen(void);
void toggleStatusYellow(void);
void toggleStatusRed(void);
void switchOnStatusGreen(void);
void switchOnStatusYellow(void);
void switchOnStatusRed(void);
void switchOffStatusGreen(void);
void switchOffStatusYellow(void);
void switchOffStatusRed(void);

void switchOnDot1(void);
void switchOnDot2(void);
void switchOnDot3(void);
void switchOnDot4(void);
void switchOffDot1(void);
void switchOffDot2(void);
void switchOffDot3(void);
void switchOffDot4(void);

void switchOnChar(void);
void switchOffChar(void);