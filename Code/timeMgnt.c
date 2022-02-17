/*******************************************************************************
*
*	Author:			Georg Bauer (geo_bauer@gmx.net)
*	Date:			18.04.2014
*
*	Project-Title:	ClockWise
*	Description:	Time Calculation
*
*	File-Title:		Time Management File
*
*******************************************************************************
*
* Status LED: The green  LED toggles every second
*
*******************************************************************************
*
*	Timer:
*	Timer 1 is used for counting seconds and calculate time
*
*	Interrupts:
*	Timer 1 interrupt service routine is every second active
*
*******************************************************************************
*/

//! Own header
#include "timeMgnt.h"
#include "system.h"
#include "gpios.h"
#include "ledMatrix.h"

//! Extern globals variables
extern volatile struct time systemTime;
extern volatile struct systemParameter systemConfig;
extern volatile struct row actualMatrix[12];
extern volatile uint8_t acutalDot;

//! Write Initial values
void initTimeMgnt(void)
{
	//! timer for counting seconds and calculate time
	// 16 bit timer/counter 1
	// clock select: prescaler 256 & preload 3036 -> 1s (1Hz) calculation see below
	TCCR1B |= (1 << CS12);
	// enable timer/counter 1 interrupt overflow
	TIMSK1 |= (1 << TOIE1);
	// preload timing value
	TCNT1 = 3036;
}

//! Interrupt Service Routine for when Timer/Counter 1 has an overflow
// this routine will called every 1s (1Hz)
// calculated by: (2^16 [16bit counter]  - 3036 [preload value]) * 256 [timer 1 clock divider] / 16MHz = 1s
ISR(TIMER1_OVF_vect)
{
	uint8_t newMonth = 0;
	uint8_t i = 0;
	static uint8_t state = 0;
	
	// preload timing value
	TCNT1 = 3036;
	
	// increment seconds
	systemTime.second++;
		
	// calculate other time parameter
	if (systemTime.second == 60)
	{
		systemTime.second = 0;
		systemTime.minute++;
		if (systemTime.minute == 60)
		{
			systemTime.minute = 0;
			systemTime.hour++;
			if (systemTime.hour == 24)
			{
				systemTime.hour = 0;
				systemTime.day++;
				
				// months with 31 days: january, march, may, july, august, october & december
				if (systemTime.day == 32)
				{
					newMonth = 1;
				}
				
				// months with 30 days: april, june, september & november
				if (systemTime.day == 31 && (systemTime.month == 4 || systemTime.month == 6 || systemTime.month == 9  || systemTime.month == 11))
				{
					newMonth = 1;
				}
				
				// month with 29 days: february
				if (systemTime.day == 30 && systemTime.month == 2)
				{
					newMonth = 1;
				}
				
				// month with 28 days: february in a lap year
				if (systemTime.day == 29 && systemTime.month == 2 && (((systemTime.year%4 == 0) && (systemTime.year%100 != 0)) || (systemTime.year%400 == 0)))
				{
					newMonth = 1;
				}
				
				// new month
				if (newMonth)
				{
					systemTime.day = 1;
					systemTime.month++;
					
					if (systemTime.month == 13)
					{
						systemTime.month = 1;
						systemTime.year++;
					}				
				}				
			}
		}
	}
	
	// toggle status led
	toggleStatusGreen();
	
	// if time signal is available
	// display time and dot action
	if (systemConfig.status & 0x01)
	{
		// if active dot led's for seconds (no fade)  xx01xxxxb
		// change every second the active dot
		if ((systemConfig.displaySetting & 0b00010000) && (~systemConfig.displaySetting & 0b00100000))
		{
			acutalDot = 0x01 << (systemTime.second % 5);
		}
		// if active dot led's for minutes (no fade)  xx00xxxxb
		// change every minute the active dot
		if ((~systemConfig.displaySetting & 0b00010000) && (~systemConfig.displaySetting & 0b00100000))
		{
			acutalDot = 0x01 << (systemTime.minute % 5);
		}
		
		// actualize 'actualMatrix' Register with system time
		actualizeMatrixWithSystemTime();
	}
	// if no time signal is available
	else
	{
		// all dots on
		acutalDot = 0b0001111;
		
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