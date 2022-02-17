/*******************************************************************************
*
*	Author:			Georg Bauer (geo_bauer@gmx.net)
*	Date:			10.04.2014
*
*	Project-Title:	ClockWise
*	Description:	Variable description of basic's like time, status settings 
*
*	File-Title:		System File - Header File
*
*******************************************************************************
* Status description: variable 'status' uint8
*   ---xxxxxb structure of variable
*
*	----xxx0b no time information in system available
*			  - the searching sequence is displayed
*	----xxx1b time information in system available
*			  - a time signal is displayed (if no menu is selected)
*
*	----xx0xb searching for dcf77-signal is inactive
*	----xx1xb searching for dcf77-signal is active
*
*	----x0xxb rtc time is not available
*	----x1xxb rtc time is available
*
*	----0xxxb setting menu is inactive
*	----1xxxb setting menu is active
*
*	---0xxxxb automatic time mode is active
*	---1xxxxb manual time mode is active
*
*******************************************************************************
* Status description: variable 'menuState' uint8
*	see menu.c file
*
*******************************************************************************
* Display Settings: variable "displaySetting" unint8
*	vwxxyyyzb structure of variable
*
*		  zb (bit 0): shows pie variant
*		  0b original straight pie
*		  1b shifted pie (for 2 minutes)
*
*		yyyb (bit 3 to bit 1): shows character variant 
*		000b standard without birthday and horse information
*		001b birthday (01.01., 16.01., 07.08., 22.09., 08.12.) and horses@6pm
*		...b open for other variants
* 	
*	  xxb (bit 5 to bit 4):	open for other variants
*	  ..b open for other variants
*
*	 qb (bit 6): shows automatic display brightness variant
*	 0b automatic display brightness regulation is inactive 
*	 1b automatic display brightness regulation is active
* 
*	vb (bit 7): set's the display sequence while time searching mode
*	0b square & dot sequence when searching signal
*	1b only dot sequence when searching signal
*
*******************************************************************************
*/

//! libraries
#include <avr/io.h>
#include <stdint.h>
//#include <stdlib.h>

//! System Time Structure
struct time
{
	uint8_t  second;	// second
	uint8_t  minute;	// minute
	uint8_t  hour;		// hour
	uint8_t  day;		// day
	uint8_t  month;		// month	
	uint8_t  year;		// year
	uint8_t  weekday;	// weekday
};

//! System Parameter
struct systemParameter
{
	uint8_t status;					// See status description on top
	uint8_t lightIntensity;			// light intensity of measured value (range is 0 dark to 255 bright)
	uint8_t	potentiometerValue;		// set value of potentiometer (range is 0 low to 255 high)
	uint8_t displaySetting;			// see display settings description on top
	uint8_t displayBrightness;		// display brightness (range 0 dark to 255 bright)
	uint8_t menuStatus;				// actual menu status
	uint8_t version;				// software system version
};

//! Functional prototypes
void initSystem(void);
uint8_t calcuateBrightness(uint8_t lighIntensity, uint8_t potentiometerValue);
uint8_t calculateIntensity(uint8_t intensity);
uint8_t calculatePotiValue(uint8_t potiValue);
