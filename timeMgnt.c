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
* Status LED: The green LED toggles every second
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
#include "taskMgnt.h"

//! Extern globals variables
extern volatile struct time systemTime;
extern volatile struct systemParameter systemConfig;

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
	
	// preload timing value
	TCNT1 = 3036;
	
	// increment seconds
	systemTime.second++;
	
	// calculate actual task
	calculateTaskTiming();
		
	// calculate other time parameter
	if (systemTime.second >= 60)
	{
		systemTime.second = 0;
		systemTime.minute++;
		if (systemTime.minute >= 60)
		{
			systemTime.minute = 0;
			systemTime.hour++;
			if (systemTime.hour >= 24)
			{
				systemTime.hour = 0;
				systemTime.day++;
				
				// weekday management
				systemTime.weekday++;
				if (systemTime.weekday >=8)
				{
					systemTime.weekday = 1;
				}				
				
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
					
					if (systemTime.month >= 13)
					{
						systemTime.month = 1;
						systemTime.year++;
					}				
				}				
			}
		}
	}
}