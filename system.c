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
struct systemParameter systemConfig;
struct time systemTime;

//! Write Initial values
void initSystem(void)
{
	// set default system status
	// - x0b no time value available
	// - 1xb searching dcf77 signal
	systemConfig.status =  0x02;
	// default light intensity
	systemConfig.lightIntensity = 10;
	// set value of potentiometer 		
	systemConfig.potentiometerValue = 10;
	// default display brightness
	systemConfig.displayBrightness = calcuateBrightness(systemConfig.lightIntensity, systemConfig.potentiometerValue);
	// set default system display settings
	// - xxxx0001b birthday and horses
	// - xx01xxxxb active dot led's for seconds (no fade)
	// - xx00xxxxb active dot led's for minutes (no fade)
	// - x1xxxxxxb automatic display brightness is active
	systemConfig.displaySetting = 0x51; //0x11 without automatic brightness regulation

	// set init time values
	// (wedding day from dad and mom) 
	systemTime.year = 14;
	systemTime.month = 5;
	systemTime.day = 1;
	systemTime.hour = 0;
	systemTime.minute = 0;
	systemTime.second = 0;
	systemTime.weekday = 0;
}

uint8_t calcuateBrightness(uint8_t lightIntensity, uint8_t potentiometerValue)
{
	int16_t brightness = 0;

	// automatic regulation of display brightness
	// (bit 6):	shows automatic display brightness variant
	// 0b automatic display brightness regulation is inactive
	// 1b automatic display brightness regulation is active
	if(systemConfig.displaySetting & 0b01000000)
	{
		// mean value
		brightness = (lightIntensity >> 1) + (potentiometerValue >> 1);
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
	// inverting value
	intensity = ~intensity;
	
	// gain and offset calculation (no overflow protection)
	intensity *= INTENSITY_GAIN;
	intensity += INTENSITY_OFFSET;
	
	// limitation
	if (intensity >= INTENSITY_MAXIMUM)
	{
		intensity = INTENSITY_MAXIMUM;
	}
	
	if (intensity <= INTENSITY_MINIMUM)
	{
		intensity = INTENSITY_MINIMUM;
	}
	return intensity;	
}

uint8_t calculatePotiValue(uint8_t potiValue)
{
	// inverting value
	potiValue = ~potiValue;
	
	// gain and offset calculation (no overflow protection)
	potiValue *= POTIVALUE_GAIN;
	potiValue += POTIVALUE_OFFSET;
	
	// limitation
	if (potiValue >= POTIVALUE_MAXIMUM)
	{
		potiValue = POTIVALUE_MAXIMUM;		
	}
	
	if (potiValue <= POTIVALUE_MINIMUM)
	{
		potiValue = POTIVALUE_MINIMUM;	
	}
	return potiValue;
}
