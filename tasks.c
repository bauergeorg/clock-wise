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
extern volatile uint8_t acutalDot;

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
	uint8_t i = 0;
	static uint8_t state = 0;
	
	// toggle status led
	toggleStatusGreen();
	
	// if time signal is available
	// display time and dot action
	if (systemConfig.status & 0x01)
	{
		// change every minute the active dot
		acutalDot = 0x01 << (systemTime.minute % 5);

		// actualize 'actualMatrix' Register with system time
		actualizeMatrixWithSystemTime();
	}
	// if no time signal is available
	else
	{
		// check searching mode: square (0) or dots only (1)
		// searching mode: dots only (1)
		if(0x80 & systemConfig.displaySetting)
		{
			setMatrixDark();
			// display dot session
			switch(state)
			{
				case 0:
				// first dot
				acutalDot = 0b00010000;
				break;
				case 1:
				// second dot
				acutalDot = 0b00001000;
				break;
				case 2:
				// third dot
				acutalDot = 0b00000100;
				break;
				case 3:
				// forth dot
				acutalDot = 0b00000010;
				break;
				case 4:
				// third dot
				acutalDot = 0b00000100;
				break;
				case 5:
				// second dot
				acutalDot = 0b00001000;
				break;
				default:
				// no dot
				acutalDot = 0b00000000;
				break;		
			}
			
			// increment state
			state++;
			// reset state
			if (state == 6)
			{
				state = 0;
			}
		}
		
		// searching mode: square (0)
		else
		{	
			// display square session
			switch(state)
			{
				case 0:
				// first square
				actualMatrix[0].high	= 0b11111111;
				actualMatrix[0].low		= 0b11110000;
				actualMatrix[11].high	= 0b11111111;
				actualMatrix[11].low	= 0b11110000;
				for(i = 1; i<11; i++)
				{
					actualMatrix[i].high = 0b10000000;
					actualMatrix[i].low	 = 0b00010000;
				}
				acutalDot = 0b00010010;
				break;
				case 1:
				// second square
				actualMatrix[0].high	= 0b00000000;
				actualMatrix[0].low		= 0b00000000;
				actualMatrix[11].high	= 0b00000000;
				actualMatrix[11].low	= 0b00000000;
				actualMatrix[1].high	= 0b01111111;
				actualMatrix[1].low		= 0b11100000;
				actualMatrix[10].high	= 0b01111111;
				actualMatrix[10].low	= 0b11100000;
				for(i = 2; i<10; i++)
				{
					actualMatrix[i].high = 0b01000000;
					actualMatrix[i].low	 = 0b00100000;
				}
				acutalDot = 0b00001100;
				break;
				case 2:
				// third square
				actualMatrix[0].high	= 0b00000000;
				actualMatrix[0].low		= 0b00000000;
				actualMatrix[1].high	= 0b00000000;
				actualMatrix[1].low		= 0b00000000;
				actualMatrix[10].high	= 0b00000000;
				actualMatrix[10].low	= 0b00000000;
				actualMatrix[11].high	= 0b00000000;
				actualMatrix[11].low	= 0b00000000;
				actualMatrix[2].high	= 0b00111111;
				actualMatrix[2].low		= 0b11000000;
				actualMatrix[9].high	= 0b00111111;
				actualMatrix[9].low		= 0b11000000;
				for(i = 3; i<9; i++)
				{
					actualMatrix[i].high = 0b00100000;
					actualMatrix[i].low	 = 0b01000000;
				}
				acutalDot = 0b00010010;
				break;
				case 3:
				// fourth square
				actualMatrix[0].high	= 0b00000000;
				actualMatrix[0].low		= 0b00000000;
				actualMatrix[1].high	= 0b00000000;
				actualMatrix[1].low		= 0b00000000;
				actualMatrix[2].high	= 0b00000000;
				actualMatrix[2].low		= 0b00000000;
				actualMatrix[9].high	= 0b00000000;
				actualMatrix[9].low		= 0b00000000;
				actualMatrix[10].high	= 0b00000000;
				actualMatrix[10].low	= 0b00000000;
				actualMatrix[11].high	= 0b00000000;
				actualMatrix[11].low	= 0b00000000;
				actualMatrix[3].high	= 0b00011111;
				actualMatrix[3].low		= 0b10000000;
				actualMatrix[8].high	= 0b00011111;
				actualMatrix[8].low		= 0b10000000;
				for(i = 4; i<8; i++)
				{
					actualMatrix[i].high = 0b00010000;
					actualMatrix[i].low	 = 0b10000000;
				}
				acutalDot = 0b00001100;
				break;
				case 4:
				// finfth square
				actualMatrix[0].high	= 0b00000000;
				actualMatrix[0].low		= 0b00000000;
				actualMatrix[1].high	= 0b00000000;
				actualMatrix[1].low		= 0b00000000;
				actualMatrix[2].high	= 0b00000000;
				actualMatrix[2].low		= 0b00000000;
				actualMatrix[3].high	= 0b00000000;
				actualMatrix[3].low		= 0b00000000;
				actualMatrix[8].high	= 0b00000000;
				actualMatrix[8].low		= 0b00000000;
				actualMatrix[9].high	= 0b00000000;
				actualMatrix[9].low		= 0b00000000;
				actualMatrix[10].high	= 0b00000000;
				actualMatrix[10].low	= 0b00000000;
				actualMatrix[11].high	= 0b00000000;
				actualMatrix[11].low	= 0b00000000;
				actualMatrix[4].high	= 0b00001111;
				actualMatrix[4].low		= 0b00000000;
				actualMatrix[7].high	= 0b00001111;
				actualMatrix[7].low		= 0b00000000;
				for(i = 5; i<7; i++)
				{
					actualMatrix[i].high = 0b00001001;
					actualMatrix[i].low	 = 0b0000000;
				}
				acutalDot = 0b00010010;
				break;
				case 5:
				// sixth square
				actualMatrix[0].high	= 0b00000000;
				actualMatrix[0].low		= 0b00000000;
				actualMatrix[1].high	= 0b00000000;
				actualMatrix[1].low		= 0b00000000;
				actualMatrix[2].high	= 0b00000000;
				actualMatrix[2].low		= 0b00000000;
				actualMatrix[3].high	= 0b00000000;
				actualMatrix[3].low		= 0b00000000;
				actualMatrix[4].high	= 0b00000000;
				actualMatrix[4].low		= 0b00000000;
				actualMatrix[7].high	= 0b00000000;
				actualMatrix[7].low		= 0b00000000;
				actualMatrix[8].high	= 0b00000000;
				actualMatrix[8].low		= 0b00000000;
				actualMatrix[9].high	= 0b00000000;
				actualMatrix[9].low		= 0b00000000;
				actualMatrix[10].high	= 0b00000000;
				actualMatrix[10].low	= 0b00000000;
				actualMatrix[11].high	= 0b00000000;
				actualMatrix[11].low	= 0b00000000;
				actualMatrix[5].high	= 0b00000110;
				actualMatrix[5].low		= 0b00000000;
				actualMatrix[6].high	= 0b00000110;
				actualMatrix[6].low		= 0b00000000;
				acutalDot = 0b00001100;
				break;
				case 6:
				// finfth square
				actualMatrix[0].high	= 0b00000000;
				actualMatrix[0].low		= 0b00000000;
				actualMatrix[1].high	= 0b00000000;
				actualMatrix[1].low		= 0b00000000;
				actualMatrix[2].high	= 0b00000000;
				actualMatrix[2].low		= 0b00000000;
				actualMatrix[3].high	= 0b00000000;
				actualMatrix[3].low		= 0b00000000;
				actualMatrix[8].high	= 0b00000000;
				actualMatrix[8].low		= 0b00000000;
				actualMatrix[9].high	= 0b00000000;
				actualMatrix[9].low		= 0b00000000;
				actualMatrix[10].high	= 0b00000000;
				actualMatrix[10].low	= 0b00000000;
				actualMatrix[11].high	= 0b00000000;
				actualMatrix[11].low	= 0b00000000;
				actualMatrix[4].high	= 0b00001111;
				actualMatrix[4].low		= 0b00000000;
				actualMatrix[7].high	= 0b00001111;
				actualMatrix[7].low		= 0b00000000;
				for(i = 5; i<7; i++)
				{
					actualMatrix[i].high = 0b00001001;
					actualMatrix[i].low	 = 0b0000000;
				}
				acutalDot = 0b00010010;
				break;
				case 7:
				// fourth square
				actualMatrix[0].high	= 0b00000000;
				actualMatrix[0].low		= 0b00000000;
				actualMatrix[1].high	= 0b00000000;
				actualMatrix[1].low		= 0b00000000;
				actualMatrix[2].high	= 0b00000000;
				actualMatrix[2].low		= 0b00000000;
				actualMatrix[9].high	= 0b00000000;
				actualMatrix[9].low		= 0b00000000;
				actualMatrix[10].high	= 0b00000000;
				actualMatrix[10].low	= 0b00000000;
				actualMatrix[11].high	= 0b00000000;
				actualMatrix[11].low	= 0b00000000;
				actualMatrix[3].high	= 0b00011111;
				actualMatrix[3].low		= 0b10000000;
				actualMatrix[8].high	= 0b00011111;
				actualMatrix[8].low		= 0b10000000;
				for(i = 4; i<8; i++)
				{
					actualMatrix[i].high = 0b00010000;
					actualMatrix[i].low	 = 0b10000000;
				}
				acutalDot = 0b00001100;
				break;
				case 8:
				// third square
				actualMatrix[0].high	= 0b00000000;
				actualMatrix[0].low		= 0b00000000;
				actualMatrix[1].high	= 0b00000000;
				actualMatrix[1].low		= 0b00000000;
				actualMatrix[10].high	= 0b00000000;
				actualMatrix[10].low	= 0b00000000;
				actualMatrix[11].high	= 0b00000000;
				actualMatrix[11].low	= 0b00000000;
				actualMatrix[2].high	= 0b00111111;
				actualMatrix[2].low		= 0b11000000;
				actualMatrix[9].high	= 0b00111111;
				actualMatrix[9].low		= 0b11000000;
				for(i = 3; i<9; i++)
				{
					actualMatrix[i].high = 0b00100000;
					actualMatrix[i].low	 = 0b01000000;
				}
				acutalDot = 0b00010010;
				break;
				case 9:
				// second square
				actualMatrix[0].high	= 0b00000000;
				actualMatrix[0].low		= 0b00000000;
				actualMatrix[11].high	= 0b00000000;
				actualMatrix[11].low	= 0b00000000;
				actualMatrix[1].high	= 0b01111111;
				actualMatrix[1].low		= 0b11100000;
				actualMatrix[10].high	= 0b01111111;
				actualMatrix[10].low	= 0b11100000;
				for(i = 2; i<10; i++)
				{
					actualMatrix[i].high = 0b01000000;
					actualMatrix[i].low	 = 0b00100000;
				}
				acutalDot = 0b00001100;
				break;
				default:
				break;
			}

			// increment state
			state++;
			// reset state
			if (state == 10)
			{
				state = 0;
			}
			
		}
		
	}
}

//! Task half second
void taskHalfSecond(void)
{
	
}