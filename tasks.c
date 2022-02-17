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
#include "displayMatrix.h"
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
}

//! Task half second
void taskHalfSecond(void)
{
	// display information on matrix, called by second interrupt (time management)
	displayMatrixInformation(0);
}