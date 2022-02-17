/*******************************************************************************
*
*	Author:			Georg Bauer (geo_bauer@gmx.net)
*	Date:			10.04.2014
*
*	Project-Title:	ClockWise
*	Description:	***
*
*	File-Title:		System File
*
*******************************************************************************
*/

//! Own header
#include "system.h"
#include "settings.h"

//! Own global variables
volatile struct systemParameter systemConfig;
volatile struct time systemTime;

//! Write Initial values
void initSystem(void)
{
	// set default system status
	// - xxxx.xxx0b no time value available
	// - xxxx.xx0xb searching dcf77 signal inactive
	// - xxxx.x0xxb rtc time is not available
	// - xxxx.0xxxb setting menu is inactive
	// - xxx1.xxxxb automatic time mode is active
	systemConfig.status = 0x10;
	// default light intensity
	systemConfig.lightIntensity = 10;
	// set value of potentiometer 		
	systemConfig.potentiometerValue = 10;
	// default display brightness
	systemConfig.displayBrightness = calcuateBrightness(systemConfig.lightIntensity, systemConfig.potentiometerValue);
	// set default system display settings
	// - xxxx.xxx0b straight pie
	// - xxxx.001xb original with birthday and horses@6pm
	// - x1xx.xxxxb automatic display brightness regulation is active
	// - 1xxx.xxxxb no sequence when searching signal
	systemConfig.displaySetting = 0xC2; // see above
	// set display status to dark
	systemConfig.displayStatus = DISPLAY_STATE_DARK;
	// system version 0.0.3
	systemConfig.version = 3;
	
	// set init time values
	// (wedding day from dad and mom) 
	systemTime.year		= 18;
	systemTime.month	= 12;
	systemTime.day		= 24;
	systemTime.hour		= 9;
	systemTime.minute	= 0;
	systemTime.second	= 0;
	systemTime.weekday	= 1; // monday
}

uint8_t calcuateBrightness(uint8_t lightIntensity, uint8_t potentiometerValue)
{
	int16_t brightness = 0;

	// automatic regulation of display brightness
	// (bit 6):	shows automatic display brightness variant
	// 0b automatic display brightness regulation is inactive
	// 1b automatic display brightness regulation is active
	if(systemConfig.displaySetting & 0x40)
	{
		brightness = lightIntensity + potentiometerValue;
	}
	else
	{
		brightness = potentiometerValue;
	}
	
	// gain and offset calculation (no overflow protection)
	brightness *= PWMVALUE_GAIN;
	brightness += PWMVALUE_OFFSET;
	
	
	if (brightness >= PWMVALUE_MAXIMUM)
	{
		brightness = PWMVALUE_MAXIMUM;
	}
	
	if (brightness <= PWMVALUE_MINIMUM)
	{
		brightness = PWMVALUE_MINIMUM;
	}
	return (uint8_t)brightness;
}

uint8_t calculateIntensity(uint8_t intensity)
{
	// because of overflow handling
	uint16_t intensity16;

	// inverting value
	intensity = ~intensity;
	intensity16 = intensity;
	
	// gain and offset calculation (no overflow protection)
	intensity16 *= INTENSITY_GAIN;
	intensity16 += INTENSITY_OFFSET;
	
	// limitation
	if (intensity16 >= INTENSITY_MAXIMUM)
	{
		intensity16 = INTENSITY_MAXIMUM;
	}
	
	if (intensity16 <= INTENSITY_MINIMUM)
	{
		intensity16 = INTENSITY_MINIMUM;
	}
	return (uint8_t)intensity16;	
}

uint8_t calculatePotiValue(uint8_t potiValue)
{
	// because of overflow handling
	uint16_t potiValue16;

	// inverting value
	potiValue = ~potiValue;
	potiValue16 = potiValue;
	
	// gain and offset calculation (no overflow protection)
	potiValue16 *= POTIVALUE_GAIN;
	potiValue16 += POTIVALUE_OFFSET;
	
	// limitation
	if (potiValue16 >= POTIVALUE_MAXIMUM)
	{
		potiValue16 = POTIVALUE_MAXIMUM;		
	}
	
	if (potiValue16 <= POTIVALUE_MINIMUM)
	{
		potiValue16 = POTIVALUE_MINIMUM;	
	}
	return (uint8_t)potiValue16;
}
