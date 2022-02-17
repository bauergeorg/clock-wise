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
*   ------xxb structure of variable
*
*	------x0b no time information in system available - searching sequence
*			  is displayed
*	------x1b time information in system available - a time is displayed
*
*	------0xb searching for dcf77-signal is inactive
*	------1xb searching for dcf77-signal is active
*
* 
*******************************************************************************
* Display Settings: variable "displaySetting" unint8
*	-xyyzzzzb structure of variable
*
*		zzzzb (bit 3 to bit 0): shows character variant 
*		0001b birthday (01.01., 16.01., 07.08., 22.09., 08.12.) and horses@6pm
*		0010b birthday (01.01., 16.01., 07.08., 22.09., 08.12.)	
* 	
*	  yyb (bit 5 to bit 4):		shows second  variant <<<??????
*	  00b display of led for seconds is inactive
*     01b display of led for seconds is active (no fade)
*	  10b fade from led to led is active "left to right" - not actual included!
*	  11b fade from led to led is active "night-rider"	 - not actual included!
*
*	-xb (bit 6):				shows automatic display brightness variant
*	 0b automatic display brightness regulation is inactive 
*	 1b automatic display brightness regulation is active 
*	
*******************************************************************************
*/

//! libraries
#include <avr/io.h>
#include <stdint.h>
//#include <stdlib.h>


/*//! Own global variables
extern struct systemParameter systemConfig;
extern struct time systemTime;*/


//! System Time Structure
struct time
{
	uint8_t  second;	// second
	uint8_t  minute;	// minute
	uint8_t  hour;		// hour
	uint8_t  day;		// day
	uint8_t  month;		// month	
	uint8_t year;		// year
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
};

//! Functional prototypes
void initSystem(void);
uint8_t calcuateBrightness(uint8_t lighIntensity, uint8_t potentiometerValue);
uint8_t calculateIntensity(uint8_t intensity);
uint8_t calculatePotiValue(uint8_t potiValue);
