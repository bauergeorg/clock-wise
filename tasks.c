/*******************************************************************************
*
*	Author:			Georg Bauer (geo_bauer@gmx.net)
*	Date:			19.06.2014
*
*	Project-Title:	ClockWise
*	Description:	Summary of tasks
*
*	File-Title:		Tasks
*
*******************************************************************************
*/

//! Libraries
#include "taskMgnt.h"
#include "settings.h"
#include "system.h"
#include "gpios.h"
#include "adc.h"
#include "ledMatrix.h"

//! Extern global variables
extern volatile struct systemParameter systemConfig;
extern volatile struct time systemTime;
extern volatile struct row actualMatrix[12];

//! Task hour
void taskHour(void)
{
	
}

//! Task hour
void taskThirteenMinute(void)
{
	
}

//! Task hour
void taskFifteenMinute(void)
{
	
}

//! Task 10 minute
void taskTenMinute(void)
{
	
}

//! Task 5 minute
void taskFiveMinute(void)
{
	// read light intensity value of adc
	systemConfig.lightIntensity = calculateIntensity(adcRead(0));
	// real potentiometer value of adc
	systemConfig.potentiometerValue = calculatePotiValue(adcRead(1));
	// calculate display brightness value
	systemConfig.displayBrightness = calcuateBrightness(systemConfig.lightIntensity, systemConfig.potentiometerValue);
}

//! Task minute
void taskMinute(void)
{

}

//! Task second
void taskSecond(void)
{
	// toggle status led
	toggleStatusGreen();
	
	// if menu mode
	if(systemConfig.status & 0x04)
	{
		// display menu stat
		actualizeMatrixInMenuMode();
	}
	else
	{
		// if time signal is available
		// display time and dot action
		if(systemConfig.status & 0x01)
		{
			// actualize 'actualMatrix' Register with system time
			actualizeMatrixWithSystemTime();
		}
		// if no time signal is available
		else
		{
			// actualize 'actualMatrix' Register with searching sequence
			actualizeMatrixWithSearchingSequence();
		}
	}
}

//! Task half second
void taskHalfSecond(void)
{
	
}