/*******************************************************************************
*
*	Author:			Georg Bauer (geo_bauer@gmx.net)
*	Date:			16.05.2014
*
*	Project-Title:	ClockWise
*	Description:	Management of second and minute tasks
*
*	File-Title:		Task Management
*
*******************************************************************************
* Next-Task Flag: variable "taskFlags" unint8
*	xxxxxxxxb structure of variable
*
*	-------xb (bit 0): shows request flag of half second task
*	------x-b (bit 1): shows request flag of second task
*	-----x--b (bit 2): shows request flag of minute task
*	----x---b (bit 3): shows request flag of 5-minute task
*	---x----b (bit 4): shows request flag of 10-minute task
*	--x-----b (bit 5): shows request flag of 15-minute task
*	-x------b (bit 6): shows request flag of 30-minute task
*	x-------b (bit 7): shows request flag of hour task
*
*******************************************************************************
*/

//! Libraries
#include "taskMgnt.h"
#include "tasks.h"
#include "settings.h"

//! Own global variables
volatile uint8_t taskFlags;

//! Initialize Task System
void initTasks(void)
{
	// set no request
	taskFlags = 0;
}

//! calculate task timing and set flags
void calculateTaskTiming(void)
{
	static uint8_t taskCounterHours		= 0;	// counts hours
	static uint8_t taskCounterMinutes	= 0;	// counts minutes
	static uint8_t taskCounterSeconds	= 0;	// counts seconds
	static uint8_t taskPreCounter		= 0;	// counts pre seconds	
	
	taskPreCounter++;
	/*if (taskPreCounter == TASK_PRECOUNTER/2-1)
		// set request flag for half second task
		taskFlags |= 0b00000010;
	*/
	if (taskPreCounter >= 0) //TASK_PRECOUNTER)
	{
		taskPreCounter = 0;
		taskCounterSeconds++;
		
		// set request flag for second task and half second task
		taskFlags |= 0b00000011;
		
		if (taskCounterSeconds >= 60)
		{
			taskCounterSeconds = 0;
			taskCounterMinutes++;
			
			// set request flag for minute task
			taskFlags |= 0b00000100;
			
			if (taskCounterMinutes >= 60)
			{
				taskCounterMinutes = 0;
				taskCounterHours++;
				// set request flag for hour task
				taskFlags |= 0b10000000;
				
				if (taskCounterHours >= 24)
				{
					taskCounterHours = 0;	
				}
				
			}
			
			// 5, 10, 15 and 30 minute task
			switch(taskCounterMinutes)
			{
				case 5:
					// set request flag for 5-minute task
					taskFlags |= 0b00001000;
					break;
				case 10:
					// set request flag for 10-minute task
					taskFlags |= 0b00010000;
					break;
				case 15:
					// set request flag for 15-minute task
					taskFlags |= 0b00100000;
					break;
				case 30:
					// set request flag for 30-minute task
					taskFlags |= 0b01000000;
					break;
				default:
					break;
			}
		}
	}	
}

//! check if a task flag / request is set
void checkForTask(void)
{
	// priority is from high frequent task to low frequent task
		
	// half second task
	if (taskFlags & 0b00000001)
	{
		// run task
		taskHalfSecond();
		// reset task flag
		taskFlags &= ~0b00000001;
	}
	
	// second task
	if (taskFlags & 0b00000010)
	{
		// run task
		taskSecond();
		// reset task flag
		taskFlags &= ~0b00000010;
	}
	
	// minute task
	if (taskFlags & 0b00000100)
	{
		// run task
		taskMinute();
		// reset task flag
		taskFlags &= ~0b00000100;
	}
	
	// 5-minute task
	if (taskFlags & 0b00001000)
	{
		// run task
		taskFiveMinute();
		// reset task flag
		taskFlags &= ~0b00001000;
	}
	
	// 10-minute task
	if (taskFlags & 0b00010000)
	{
		// run task
		taskTenMinute();
		// reset task flag
		taskFlags &= ~0b00010000;
	}
	
	// 15-minute task
	if (taskFlags & 0b00100000)
	{
		// run task
		taskFifteenMinute();
		// reset task flag
		taskFlags &= ~0b00100000;
	}
	
	// 30-minute task
	if (taskFlags & 0b01000000)
	{
		// run task
		taskThirteenMinute();
		// reset task flag
		taskFlags &= ~0b01000000;
	}
	
	// hour task
	if (taskFlags & 0b10000000)
	{
		// run task
		taskHour();
		// reset task flag
		taskFlags &= ~0b10000000;
	}
}