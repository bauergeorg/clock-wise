/*******************************************************************************
*
*	Author:			Georg Bauer (geo_bauer@gmx.net)
*	Date:			23.02.2016
*
*	Project-Title:	ClockWise
*	Description:	***
*
*	File-Title:		Display Matrix Information
*
*******************************************************************************
*/

//! libraries
#include "displayMatrix.h"
#include "system.h"
#include "gpios.h"
#include "ledMatrix.h"

//! Extern global variables
extern volatile struct systemParameter systemConfig;

//! makes menu management
// input: is function called by a activation by switch than switchActiviation = 1, else switchActiviation = 0
void displayMatrixInformation(uint8_t switchActiviation)
{

	// if menu mode
	if(systemConfig.status & 0x08)
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
			if(!switchActiviation)
			{
				// actualize 'actualMatrix' Register with searching sequence
				actualizeMatrixWithSearchingSequence();
			}
		}
	}
}