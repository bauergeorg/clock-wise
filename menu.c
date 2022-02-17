/*******************************************************************************
*
*	Author:			Georg Bauer (geo_bauer@gmx.net)
*	Date:			04.01.2016
*
*	Project-Title:	ClockWise
*	Description:	µC controlled clock with DCF77, RTC and a led-matrix
*
*	File-Title:		Menu Management, selectable via switches
*					(press okey and cancel at the same time)
*
*******************************************************************************
* Status description: variable 'menuStatus' uint8
*	000d	default state (press okey and cancel at the same time)
*	001d	wait to get into the menu
*	010d	- show version
*	100d	- time mode
*	110d		- automatic mode
*	111d			- search automatic dcf77 time
*	120d		- manual mode
*	121d			- set time
*	122d				- set year manual
*	123d				- set month manual
*	124d				- set day manual
*	125d				- set hour manual
*	126d				- set minutes manual
*	127d				- set minutes manual
*	128d				- set seconds manual
*	129d				- set weekday manual
*	130d				- okay - take values
*	131d				- cancel - reject values
*	200d	- display settings
*	210d		- brightness
*	211d			- automatic
*	212d			- manual
*	220d		- pie
*	221d			- straight
*	222d			- shift
*	230d		- character variant
*	231d			- standard
*	232d			- brihday and horse mode		
*	240d		- searching sequence mode
*	241d			- square
*	242d			- dots only
*
*******************************************************************************
*/

//! libraries
#include "menu.h"
#include "system.h"
#include "gpios.h"
#include "displayMatrix.h"

//! Extern global variables
extern volatile struct systemParameter systemConfig;
extern volatile struct time systemTime;

//! makes menu management
// input: switch 
void menuMgnt(uint8_t switches)
{
	// declaration of switch variables
	uint8_t okSwitch, cancelSwitch, upSwitch, downSwitch;
	cancelSwitch = switches & 0x01;
	downSwitch = (switches & 0x02) >> 1;
	upSwitch = (switches & 0x04) >> 2;
	okSwitch = (switches & 0x08) >> 3;
	
	switch(systemConfig.menuStatus)
	{
		// default status
		case 0:
		{
			// do nothing
			break;	
		}
		
		// default status: wait to get into ne menu
		case 1:
		{
			// set new menu status: show version
			systemConfig.menuStatus = 10;
			// set default system status
			// - xxxx.1xxxb setting menu is active
			systemConfig.status |= 0x08;
		
			// test
			switchOnStatusRed();
			// actualize matrix information
			displayMatrixInformation(0);
			break;
		}
		// show version
		case 10:
		{
			// up switch is pressed
			if(upSwitch)
			{
				// set new menu status: time mode
				systemConfig.menuStatus = 100;
			}
			// down switch is pressed
			if(downSwitch)
			{
				// set new menu status: display settings
				systemConfig.menuStatus = 200;
			}
			// cancel switch is pressed
			if(cancelSwitch)
			{
				// call menu cancel routine
				menuCancel();
			}
			break;
		}
		
		// time mode
		case 100:
		{
			// ok switch is pressed
			if(okSwitch)
			{
				// set new menu status: automatic mode
				systemConfig.menuStatus = 110;		
			}
			// down switch is pressed
			if(downSwitch)
			{
				// set new menu status: show version
				systemConfig.menuStatus = 10;
			}
			// up switch is pressed
			if(upSwitch)
			{
				// set new menu status: display settings
				systemConfig.menuStatus = 200;
			}
			// cancel switch is pressed
			if(cancelSwitch)
			{
				// call menu cancel routine
				menuCancel();
			}
			break;
		}
		
		// automatic mode
		case 110:
		{
			// ok switch is pressed
			if(okSwitch)
			{
				// set new menu status: search time
				//systemConfig.menuStatus = 111;
			}
			// up or down switch is pressed
			if(upSwitch || downSwitch)
			{
				// set new menu status: manual mode
				systemConfig.menuStatus = 120;
			}
			// cancel switch is pressed
			if(cancelSwitch)
			{
				// set new menu status: time mode
				systemConfig.menuStatus = 100;
			}
			break;
		}
		
		// manual mode
		case 120:
		{
			// ok switch is pressed
			if(okSwitch)
			{
				// set new menu status: set time
				systemConfig.menuStatus = 121;
			}
			// up or down switch is pressed
			if(upSwitch || downSwitch)
			{
				// set new menu status: automatic mode
				systemConfig.menuStatus = 110;
			}
			// cancel switch is pressed
			if(cancelSwitch)
			{
				// set new menu status: time mode
				systemConfig.menuStatus = 100;
			}
			break;
		}
		
		// set time
		case 121:
		{
			// ok switch is pressed
			if(okSwitch)
			{
				// set new menu status: set hour manual
				systemConfig.menuStatus = 122;
			}
			// up or down switch is pressed
			if(upSwitch || downSwitch)
			{
				// set new menu status: automatic mode
				//systemConfig.menuStatus = 110;
			}
			// cancel switch is pressed
			if(cancelSwitch)
			{
				// set new menu status: manual mode
				systemConfig.menuStatus = 120;
			}
			break;
		}
		
		// set hour manual
		case 122:
		{
			// ok switch is pressed
			if(okSwitch)
			{
				// set new menu status: set minute manual
				//systemConfig.menuStatus = 123;
			}
			// up or down switch is pressed
			if(upSwitch || downSwitch)
			{
				// set new menu status: automatic mode
				//systemConfig.menuStatus = 110;
			}
			// cancel switch is pressed
			if(cancelSwitch)
			{
				// set new menu status: manual mode
				systemConfig.menuStatus = 121;
			}
			break;
		}
		
		// display settings		
		case 200:
		{
			// ok switch is pressed
			if(okSwitch)
			{
				// set new menu status: brightness
				//systemConfig.menuStatus = 210;
			}
			// up switch is pressed
			if(upSwitch)
			{
				// set new menu status: show version
				systemConfig.menuStatus = 10;
			}
			// down switch is pressed
			if(downSwitch)
			{
				// set new menu status: time mode
				systemConfig.menuStatus = 100;
			}
			// cancel switch is pressed
			if(cancelSwitch)
			{
				// call menu cancel routine
				menuCancel();	
			}
			break;
		}
		
		// out of state? return to default state
		default:
		{
			// set default state
			systemConfig.menuStatus = 0;
			break;
		}	
	}
	
	// actualize matrix information, called by switch
	displayMatrixInformation(1);	
}

//! routine when we're leaving menu
void menuCancel(void)
{
	// set default system status
	// - xxxx.0xxxb setting menu is active
	systemConfig.status &= ~0x08;
	// set new menu status
	systemConfig.menuStatus = 0;
					
	// test
	switchOffStatusRed();
	// actualize matrix information
	displayMatrixInformation(0);
}
