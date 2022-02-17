/*******************************************************************************
*
*	Author:			Georg Bauer
*	Date:			04.01.2016
*
*	Project-Title:	ClockWise
*	Description:	µC controlled clock with DCF77, RTC and a led-matrix
*
*	File-Title:		Menu Management, selectable via switches
*					(press okey and cancel at the same time)
*
*******************************************************************************
*
*
*******************************************************************************
*/

//! libraries
#include "menu.h"
#include "system.h"
#include "dcf77.h"
#include "gpios.h"
#include "displayMatrix.h"

//! Own global variables
volatile struct time setTime;

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
	
	switch(systemConfig.displayStatus)
	{
		// default status: wait to get into menu
		case DISPLAY_STATE_MENU_WAIT:
		{
			// set new display status: show version
			systemConfig.displayStatus = DISPLAY_STATE_MENU_VERSION;
			// set default system status
			// - xxxx.1xxxb setting menu is active
			systemConfig.status |= 0x08;
		
			// actualize matrix information
			displayMatrixInformation(0);
			break;
		}
		// show version
		case DISPLAY_STATE_MENU_VERSION:
		{
			// up switch is pressed
			if(upSwitch)
			{
				// set new display status: time mode
				systemConfig.displayStatus = DISPLAY_STATE_MENU_TIME_MODE;
			}
			// down switch is pressed
			if(downSwitch)
			{
				// set new display status: debug menu
				systemConfig.displayStatus = DISPLAY_STATE_MENU_DBG;
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
			case DISPLAY_STATE_MENU_TIME_MODE:
			{
				// ok switch is pressed
				if(okSwitch)
				{
					// set new display status: automatic mode
					systemConfig.displayStatus = DISPLAY_STATE_MENU_AUTO_MODE;		
				}
				// down switch is pressed
				if(downSwitch)
				{
					// set new display status: show version
					systemConfig.displayStatus = DISPLAY_STATE_MENU_VERSION;
				}
				// up switch is pressed
				if(upSwitch)
				{
					// set new display status: display settings
					systemConfig.displayStatus = DISPLAY_STATE_MENU_SETTINGS;
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
			case DISPLAY_STATE_MENU_AUTO_MODE:
			{
				// ok switch is pressed
				if(okSwitch)
				{
					// set new display status: search time cancel blinking
					systemConfig.displayStatus = DISPLAY_STATE_MENU_AUTO_CANCEL;
				}
				// up or down switch is pressed
				if(upSwitch || downSwitch)
				{
					// set new display status: manual mode
					systemConfig.displayStatus = DISPLAY_STATE_MENU_MANUAL_MODE;
				}
				// cancel switch is pressed
				if(cancelSwitch)
				{
					// set new display status: time mode
					systemConfig.displayStatus = DISPLAY_STATE_MENU_TIME_MODE;
				}
				break;
			}
			
			// automatic mode: search time cancel blinking
			case DISPLAY_STATE_MENU_AUTO_CANCEL:
			{
				// ok switch is pressed
				if(okSwitch)
				{
					// set new display status: time mode
					systemConfig.displayStatus = DISPLAY_STATE_MENU_AUTO_MODE;
				}
				// up or down switch is pressed
				if(upSwitch || downSwitch)
				{
					// set new display status: search time ok blinking
					systemConfig.displayStatus = DISPLAY_STATE_MENU_AUTO_OK;
				}
				// cancel switch is pressed
				if(cancelSwitch)
				{
					// set new display status: time mode
					systemConfig.displayStatus = DISPLAY_STATE_MENU_AUTO_MODE;
				}
				break;
			}
			
			// automatic mode: search time ok blinking
			case DISPLAY_STATE_MENU_AUTO_OK:
			{
				// ok switch is pressed
				if(okSwitch)
				{
					// start receiving
					startDcf77Signal();
					
					// set system status
					// - xxxx.xxx0b no time information in system available - the searching sequence is displayed
					// - xxx0.xxxxb automatic time mode is active
					systemConfig.status &= ~0x11;
					// - xxxx.xx1xb searching for dcf77-signal is active
					systemConfig.status |= 0x02;
					
					// set new display status: show searching mode
					systemConfig.displayStatus = DISPLAY_STATE_SEARCH;
															
					// call menu cancel routine
					menuCancel();
				}
				// up or down switch is pressed
				if(upSwitch || downSwitch)
				{
					// set new display status: search time cancel blinking
					systemConfig.displayStatus = DISPLAY_STATE_MENU_AUTO_CANCEL;			
				}
				// cancel switch is pressed
				if(cancelSwitch)
				{
					// set new display status: time mode
					systemConfig.displayStatus = DISPLAY_STATE_MENU_AUTO_MODE;
				}
				break;
			}
		
			// manual mode
			case DISPLAY_STATE_MENU_MANUAL_MODE:
			{
				// ok switch is pressed
				if(okSwitch)
				{
					// set new display status: set time cancel blinking
					systemConfig.displayStatus = DISPLAY_STATE_MENU_MANUAL_CANCEL;
				}
				// up or down switch is pressed
				if(upSwitch || downSwitch)
				{
					// set new display status: automatic mode
					systemConfig.displayStatus = DISPLAY_STATE_MENU_AUTO_MODE;
				}
				// cancel switch is pressed
				if(cancelSwitch)
				{
					// set new display status: time mode
					systemConfig.displayStatus = DISPLAY_STATE_MENU_TIME_MODE;
				}
				break;
			}
		
			// set time cancel blinking
			case DISPLAY_STATE_MENU_MANUAL_CANCEL:
			{
				// ok switch is pressed
				if(okSwitch)
				{
					// set new display status: manual mode
					systemConfig.displayStatus = DISPLAY_STATE_MENU_MANUAL_MODE;
				}
				// up or down switch is pressed
				if(upSwitch || downSwitch)
				{
					// set new display status: set time ok blinking
					systemConfig.displayStatus = DISPLAY_STATE_MENU_MANUAL_OK;
				}
				// cancel switch is pressed
				if(cancelSwitch)
				{
					// set new display status: manual mode
					systemConfig.displayStatus = DISPLAY_STATE_MENU_MANUAL_MODE;
				}
				break;
			}
			
			// set time ok blinking
			case DISPLAY_STATE_MENU_MANUAL_OK:
			{
				// ok switch is pressed
				if(okSwitch)
				{
					// get time values from system time
					setTime = systemTime;
					// set new display status: set hour
					systemConfig.displayStatus = DISPLAY_STATE_MENU_SET_HOUR;
				}
				// up or down switch is pressed
				if(upSwitch || downSwitch)
				{
					// set new display status: set time cancel blinking
					systemConfig.displayStatus = DISPLAY_STATE_MENU_MANUAL_CANCEL;
				}
				// cancel switch is pressed
				if(cancelSwitch)
				{
					// set new display status: manual mode
					systemConfig.displayStatus = DISPLAY_STATE_MENU_MANUAL_MODE;
				}
				break;
			}			
		
			// set hour manual
			case DISPLAY_STATE_MENU_SET_HOUR:
			{
				// ok switch is pressed
				if(okSwitch)
				{
					// set new display status: set minute
					systemConfig.displayStatus = DISPLAY_STATE_MENU_SET_MINUTE;
				}
				// up switch is pressed
				if(upSwitch)
				{
					// increment hour
					setTime.hour++;
					if(setTime.hour >= 24)
					{
						setTime.hour = 0;
					}
				}
				// down switch is pressed
				if(downSwitch)
				{
					// decrement hour
					setTime.hour--;
					if(setTime.hour >= 24)
					{
						setTime.hour = 23;
					}
				}
				// cancel switch is pressed
				if(cancelSwitch)
				{
					// set new display status: set cancel
					systemConfig.displayStatus = DISPLAY_STATE_MENU_SET_CANCEL;
				}
				break;
			}
		
			// set minute manual
			case DISPLAY_STATE_MENU_SET_MINUTE:
			{
				// ok switch is pressed
				if(okSwitch)
				{
					// set new display status: set second
					systemConfig.displayStatus = DISPLAY_STATE_MENU_SET_SECOND;
				}
				// up switch is pressed
				if(upSwitch)
				{
					// increment minute
					setTime.minute++;
					if(setTime.minute >= 60)
					{
						setTime.minute = 0;
						setTime.hour++;
						if(setTime.hour >= 24)
						{
							setTime.hour = 0;
						}
					}
				}
				// down switch is pressed
				if(downSwitch)
				{
					// decrement minute
					setTime.minute--;
					if(setTime.minute >= 60)
					{
						setTime.minute = 59;
						setTime.hour--;
						if(setTime.hour >= 24)
						{
							setTime.hour = 23;
						}
					}
				}
				// cancel switch is pressed
				if(cancelSwitch)
				{
					// set new display status: set hour
					systemConfig.displayStatus = DISPLAY_STATE_MENU_SET_HOUR;
				}
				break;
			}
			
			// set second manual
			case DISPLAY_STATE_MENU_SET_SECOND:
			{
				// ok switch is pressed
				if(okSwitch)
				{
					// set new display status: set day
					systemConfig.displayStatus = DISPLAY_STATE_MENU_SET_DAY;
				}
				// up switch is pressed
				if(upSwitch)
				{
					// increment second
					setTime.second++;
					if(setTime.second >= 60)
					{
						
						setTime.second = 0;
						setTime.minute++;
						if(setTime.minute >= 60)
						{
							setTime.minute = 0;
							setTime.hour++;
							if(setTime.hour >= 24)
							{
								setTime.hour = 0;
							}
						}
					}
				}
				// down switch is pressed
				if(downSwitch)
				{
					// decrement second
					setTime.second--;
					if(setTime.second >= 60)
					{
						setTime.second = 59;
						setTime.minute--;
						if(setTime.minute >= 60)
						{
							setTime.minute = 59;
							setTime.hour--;
							if(setTime.hour >= 24)
							{
								setTime.hour = 23;
							}
						}
					}
				}
				// cancel switch is pressed
				if(cancelSwitch)
				{
					// set new display status: set minute
					systemConfig.displayStatus = DISPLAY_STATE_MENU_SET_MINUTE;
				}
				break;
			}		

			// set day manual
			case DISPLAY_STATE_MENU_SET_DAY:
			{
				// ok switch is pressed
				if(okSwitch)
				{
					// set new display status: set month
					systemConfig.displayStatus = DISPLAY_STATE_MENU_SET_MONTH;
				}
				// up switch is pressed
				if(upSwitch)
				{
					// increment day
					setTime.day++;
					if(setTime.day >= 32)
					{
						
						setTime.day = 1;
						setTime.month++;
						if(setTime.month >= 13)
						{
							setTime.month = 1;
							setTime.year++;
							if(setTime.year >= 100)
							{
								setTime.year = 0;
							}
						}
					}
				}
				// down switch is pressed
				if(downSwitch)
				{
					// decrement day
					setTime.day--;
					if(setTime.day <= 0)
					{
						setTime.day = 31;
						setTime.month--;
						if(setTime.month <= 0)
						{
							setTime.month = 12;
							setTime.year--;
							if(setTime.year <= 0)
							{
								setTime.year = 99;
							}
						}
					}
				}
				// cancel switch is pressed
				if(cancelSwitch)
				{
					// set new display status: set second
					systemConfig.displayStatus = DISPLAY_STATE_MENU_SET_SECOND;
				}
				break;
			}
			

			// set month manual
			case DISPLAY_STATE_MENU_SET_MONTH:
			{
				// ok switch is pressed
				if(okSwitch)
				{
					// set new display status: set year
					systemConfig.displayStatus = DISPLAY_STATE_MENU_SET_YEAR;
				}
				// up switch is pressed
				if(upSwitch)
				{
					// increment month
					setTime.month++;
					if(setTime.month >= 13)
					{
						setTime.month = 1;
						setTime.year++;
						if(setTime.year >= 100)
						{
							setTime.year = 0;
						}
					}
				}
				// down switch is pressed
				if(downSwitch)
				{
					// decrement month
					setTime.month--;
					if(setTime.month <= 0)
					{
						setTime.month = 12;
						setTime.year--;
						if(setTime.year <= 0)
						{
							setTime.year =99;
						}
					}
				}
				// cancel switch is pressed
				if(cancelSwitch)
				{
					// set new display status: set day
					systemConfig.displayStatus = DISPLAY_STATE_MENU_SET_DAY;
				}
				break;
			}			
			
			// set year manual
			case DISPLAY_STATE_MENU_SET_YEAR:
			{
				// ok switch is pressed
				if(okSwitch)
				{
					// set new display status: set weekday
					systemConfig.displayStatus = DISPLAY_STATE_MENU_SET_WEEKDAY;
				}
				// up switch is pressed
				if(upSwitch)
				{
					// increment year
					setTime.year++;
					if(setTime.year >= 100)
					{
						setTime.year = 0;
					}
				}
				// down switch is pressed
				if(downSwitch)
				{
					// decrement year
					setTime.year--;
					if(setTime.year <= 0)
					{
						setTime.year = 99;
					}
				}
				// cancel switch is pressed
				if(cancelSwitch)
				{
					// set new display status: set month
					systemConfig.displayStatus = DISPLAY_STATE_MENU_SET_MONTH;
				}
				break;
			}
			
			// set weekday manual
			case DISPLAY_STATE_MENU_SET_WEEKDAY:
			{
				// ok switch is pressed
				if(okSwitch)
				{
					// set new display status: set ok
					systemConfig.displayStatus = DISPLAY_STATE_MENU_SET_OK;
				}
				// up switch is pressed
				if(upSwitch)
				{
					// increment weekday
					setTime.weekday++;
					if(setTime.weekday >= 8)
					{
						setTime.weekday = 1;
					}
				}
				// down switch is pressed
				if(downSwitch)
				{
					// decrement weekday
					setTime.weekday--;
					if(setTime.weekday <= 0)
					{
						setTime.weekday = 7;
					}
				}
				// cancel switch is pressed
				if(cancelSwitch)
				{
					// set new display status: set year
					systemConfig.displayStatus = DISPLAY_STATE_MENU_SET_YEAR;
				}
				break;
			}			
						
			// set time cancel blinking
			case DISPLAY_STATE_MENU_SET_CANCEL:
			{
				// ok switch is pressed
				if(okSwitch)
				{			
					// set new display status: manual mode
					systemConfig.displayStatus = DISPLAY_STATE_MENU_MANUAL_MODE;
				}
				// up or down switch is pressed
				if(upSwitch || downSwitch)
				{
					// set new display status: set time ok blinking
					systemConfig.displayStatus = DISPLAY_STATE_MENU_SET_OK;
				}
				// cancel switch is pressed
				if(cancelSwitch)
				{
					// set new display status: set hour
					systemConfig.displayStatus = DISPLAY_STATE_MENU_SET_HOUR;
				}
				break;
			}
			
			// set time ok blinking
			case DISPLAY_STATE_MENU_SET_OK:
			{
				// ok switch is pressed
				if(okSwitch)
				{
					// set actual manual time to system time
					systemTime = setTime;
					// set system status
					// - xxxx.xxx1b time information in system available - a time signal is displayed (if no menu is selected)
					// - xxx1.xxxb manual time mode is active
					systemConfig.status |= 0x11;
					// - xxxx.xx0xb searching for dcf77-signal is inactive
					systemConfig.status &= ~0x02;
					
					// stop dcf77 signal
					stopDcf77Signal();

					// call menu cancel routine
					menuCancel();
				}
				// up or down switch is pressed
				if(upSwitch || downSwitch)
				{
					// set new display status: set time cancel blinking
					systemConfig.displayStatus = DISPLAY_STATE_MENU_SET_CANCEL;
				}
				// cancel switch is pressed
				if(cancelSwitch)
				{
					// set new display status: set hour
					systemConfig.displayStatus = DISPLAY_STATE_MENU_SET_HOUR;
				}
				break;
			}
		
		// display settings		
		case DISPLAY_STATE_MENU_SETTINGS:
		{
			// ok switch is pressed
			if(okSwitch)
			{
				// set new display status: brightness
				systemConfig.displayStatus = DISPLAY_STATE_MENU_BRIGTHNESS;
			}
			// up switch is pressed
			if(upSwitch)
			{
				// set new display status: debug mode
				systemConfig.displayStatus = DISPLAY_STATE_MENU_DBG;
			}
			// down switch is pressed
			if(downSwitch)
			{
				// set new display status: time mode
				systemConfig.displayStatus = DISPLAY_STATE_MENU_TIME_MODE;
			}
			// cancel switch is pressed
			if(cancelSwitch)
			{
				// call menu cancel routine
				menuCancel();	
			}
			break;
		}
		
			// display settings: brightness
			case DISPLAY_STATE_MENU_BRIGTHNESS:
			{
				// ok switch is pressed
				if(okSwitch)
				{
					// set new display status: 
					systemConfig.displayStatus = DISPLAY_STATE_MENU_BRIGHT_AUTO;
				}
				// up switch is pressed
				if(upSwitch)
				{
					// set new display status: pie
					systemConfig.displayStatus = DISPLAY_STATE_MENU_PIE;
				}
				// down switch is pressed
				if(downSwitch)
				{
					// set new display status: searching mode
					systemConfig.displayStatus = DISPLAY_STATE_MENU_SEARCH_MODE;
				}
				// cancel switch is pressed
				if(cancelSwitch)
				{
					// set new display status: display settings
					systemConfig.displayStatus = DISPLAY_STATE_MENU_SETTINGS;
				}
				break;
			}
			
				// display settings: brightness automatic mode
				case DISPLAY_STATE_MENU_BRIGHT_AUTO:
				{
					// ok switch is pressed
					if(okSwitch)
					{
						// set system display settings
						// - x1xx.xxxxb automatic display brightness regulation is active
						systemConfig.displaySetting |= 0x40; // see above
						// set new display status: display settings brightness
						systemConfig.displayStatus = DISPLAY_STATE_MENU_BRIGTHNESS;
					}
					// up or down switch is pressed
					if(upSwitch || downSwitch)
					{
						// set new display status: manual mode
						systemConfig.displayStatus = DISPLAY_STATE_MENU_BRIGHT_MANU;
					}
					// cancel switch is pressed
					if(cancelSwitch)
					{
						// set new display status: display settings brightness
						systemConfig.displayStatus = DISPLAY_STATE_MENU_BRIGTHNESS;
					}
					break;
				}
				
				// display settings: brightness manual mode
				case DISPLAY_STATE_MENU_BRIGHT_MANU:
				{
					// ok switch is pressed
					if(okSwitch)
					{
						// set system display settings
						// - x0xx.xxxxb automatic display brightness regulation is inactive
						systemConfig.displaySetting &= ~0x40; // see above
						// set new display status: display settings brightness
						systemConfig.displayStatus = DISPLAY_STATE_MENU_BRIGTHNESS;
					}
					// up or down switch is pressed
					if(upSwitch || downSwitch)
					{
						// set new display status: manual mode
						systemConfig.displayStatus = DISPLAY_STATE_MENU_BRIGHT_AUTO;
					}
					// cancel switch is pressed
					if(cancelSwitch)
					{
						// set new display status: display settings brightness
						systemConfig.displayStatus = DISPLAY_STATE_MENU_BRIGTHNESS;
					}
					break;
				}				
			
			// display settings: pie
			case DISPLAY_STATE_MENU_PIE:
			{
				// ok switch is pressed
				if(okSwitch)
				{
					// set new display status:
					systemConfig.displayStatus = DISPLAY_STATE_MENU_PIE_STRAIGHT;
				}
				// up switch is pressed
				if(upSwitch)
				{
					// set new display status: character variant
					systemConfig.displayStatus = DISPLAY_STATE_MENU_CHAR;
				}
				// down switch is pressed
				if(downSwitch)
				{
					// set new display status: brightness
					systemConfig.displayStatus = DISPLAY_STATE_MENU_BRIGTHNESS;
				}
				// cancel switch is pressed
				if(cancelSwitch)
				{
					// set new display status: display settings
					systemConfig.displayStatus = DISPLAY_STATE_MENU_SETTINGS;
				}
				break;
			}
			
				// display settings: pie straight
				case DISPLAY_STATE_MENU_PIE_STRAIGHT:
				{
					// ok switch is pressed
					if(okSwitch)
					{
						// set system display settings
						// - xxxx.xxx0b original straight pie
						systemConfig.displaySetting &= ~0x01; // see above
						// set new display status: display settings pie
						systemConfig.displayStatus = DISPLAY_STATE_MENU_PIE;
					}
					// up or down switch is pressed
					if(upSwitch || downSwitch)
					{
						// set new display status: shift pie
						systemConfig.displayStatus = DISPLAY_STATE_MENU_PIE_SHIFT;
					}
					// cancel switch is pressed
					if(cancelSwitch)
					{
						// set new display status: display settings pie
						systemConfig.displayStatus = DISPLAY_STATE_MENU_PIE;
					}
					break;
				}
				
				// display settings: pie shift
				case DISPLAY_STATE_MENU_PIE_SHIFT:
				{
					// ok switch is pressed
					if(okSwitch)
					{
						// set system display settings
						// - xxxx.xxx1b shifted pie (for 2 minutes)
						systemConfig.displaySetting |= 0x01; // see above
						// set new display status: display settings pie
						systemConfig.displayStatus = DISPLAY_STATE_MENU_PIE;
					}
					// up or down switch is pressed
					if(upSwitch || downSwitch)
					{
						// set new display status: straight pie
						systemConfig.displayStatus = DISPLAY_STATE_MENU_PIE_STRAIGHT;
					}
					// cancel switch is pressed
					if(cancelSwitch)
					{
						// set new display status: display settings pie
						systemConfig.displayStatus = DISPLAY_STATE_MENU_PIE;
					}
					break;
				}			
			
			// display settings: character variant
			case DISPLAY_STATE_MENU_CHAR:
			{
				// ok switch is pressed
				if(okSwitch)
				{
					// set new display status: character variant standard
					systemConfig.displayStatus = DISPLAY_STATE_MENU_CHAR_STANDARD;
				}
				// up switch is pressed
				if(upSwitch)
				{
					// set new display status: searching sequence
					systemConfig.displayStatus = DISPLAY_STATE_MENU_SEARCH_MODE;
				}
				// down switch is pressed
				if(downSwitch)
				{
					// set new display status: pie
					systemConfig.displayStatus = DISPLAY_STATE_MENU_PIE;
				}
				// cancel switch is pressed
				if(cancelSwitch)
				{
					// set new display status: display settings
					systemConfig.displayStatus = DISPLAY_STATE_MENU_SETTINGS;
				}
				break;
			}
			
				// display settings: character variant standard
				case DISPLAY_STATE_MENU_CHAR_STANDARD:
				{
					// ok switch is pressed
					if(okSwitch)
					{
						// set system display settings
						// - xxxx.000xb standard without birthday and horse information
						systemConfig.displaySetting &= ~0x0E; // see above
						// set new display status: display settings character variant
						systemConfig.displayStatus = DISPLAY_STATE_MENU_CHAR;
					}
					// up or down switch is pressed
					if(upSwitch || downSwitch)
					{
						// set new display status: character variant birthday & horses
						systemConfig.displayStatus = DISPLAY_STATE_MENU_CHAR_BIRTHDAY;
					}
					// cancel switch is pressed
					if(cancelSwitch)
					{
						// set new display status: display settings character variant
						systemConfig.displayStatus = DISPLAY_STATE_MENU_CHAR;
					}
					break;
				}	
				
				// display settings: character variant birthday & horses
				case DISPLAY_STATE_MENU_CHAR_BIRTHDAY:
				{
					// ok switch is pressed
					if(okSwitch)
					{
						// set system display settings
						// - xxxx.001xb birthday (01.01., 16.01., 07.08., 22.09., 08.12.) and horses@6pm
						systemConfig.displaySetting |= 0x02; // see above
						// set new display status: display settings character variant
						systemConfig.displayStatus = DISPLAY_STATE_MENU_CHAR;
					}
					// up or down switch is pressed
					if(upSwitch || downSwitch)
					{
						// set new display status: character variant birthday & horses
						systemConfig.displayStatus = DISPLAY_STATE_MENU_CHAR_STANDARD;
					}
					// cancel switch is pressed
					if(cancelSwitch)
					{
						// set new display status: display settings character variant
						systemConfig.displayStatus = DISPLAY_STATE_MENU_CHAR;
					}
					break;
				}					
			
			// display settings: searching sequence
			case DISPLAY_STATE_MENU_SEARCH_MODE:
			{
				// ok switch is pressed
				if(okSwitch)
				{
					// set new display status: no searching sequence
					systemConfig.displayStatus = DISPLAY_STATE_MENU_SEARCH_NO;
				}
				// up switch is pressed
				if(upSwitch)
				{
					// set new display status: brightness
					systemConfig.displayStatus = DISPLAY_STATE_MENU_BRIGTHNESS;
				}
				// down switch is pressed
				if(downSwitch)
				{
					// set new display status: character variant
					systemConfig.displayStatus = DISPLAY_STATE_MENU_CHAR;
				}
				// cancel switch is pressed
				if(cancelSwitch)
				{
					// set new display status: display settings
					systemConfig.displayStatus = DISPLAY_STATE_MENU_SETTINGS;
				}
				break;
			}	
			
				// display settings: no searching sequence
				case DISPLAY_STATE_MENU_SEARCH_NO:
				{
					// ok switch is pressed
					if(okSwitch)
					{
						// set system display settings
						// - 1xxx.xxxxb set's the display sequence while time searching mode - no sequence
						systemConfig.displaySetting |= 0x80; // see above
						// set new display status: display settings searching sequence
						systemConfig.displayStatus = DISPLAY_STATE_MENU_SEARCH_MODE;
					}
					// up or down switch is pressed
					if(upSwitch || downSwitch)
					{
						// set new display status: searching sequence square
						systemConfig.displayStatus = DISPLAY_STATE_MENU_SEARCH_SQUARE;
					}
					// cancel switch is pressed
					if(cancelSwitch)
					{
						// set new display status: display settings searching sequence
						systemConfig.displayStatus = DISPLAY_STATE_MENU_SEARCH_MODE;
					}
					break;
				}								
				
				// display settings: searching sequence square
				case DISPLAY_STATE_MENU_SEARCH_SQUARE:
				{
					// ok switch is pressed
					if(okSwitch)
					{
						// set system display settings
						// - 0xxx.xxxxb set's the display sequence while time searching mode - square
						systemConfig.displaySetting &= ~0x80; // see above
						// set new display status: display settings searching sequence
						systemConfig.displayStatus = DISPLAY_STATE_MENU_SEARCH_MODE;
					}
					// up or down switch is pressed
					if(upSwitch || downSwitch)
					{
						// set new display status: no searching sequence
						systemConfig.displayStatus = DISPLAY_STATE_MENU_SEARCH_NO;
					}
					// cancel switch is pressed
					if(cancelSwitch)
					{
						// set new display status: display settings searching sequence
						systemConfig.displayStatus = DISPLAY_STATE_MENU_SEARCH_MODE;
					}
					break;
				}				
				
		// debug mode
		case DISPLAY_STATE_MENU_DBG:
		{
			// ok switch is pressed
			if(okSwitch)
			{
				// set new display status: debug mode 1
				systemConfig.displayStatus = DISPLAY_STATE_MENU_DBG1;
			}
			// up switch is pressed
			if(upSwitch)
			{
				// set new display status: show version
				systemConfig.displayStatus = DISPLAY_STATE_MENU_VERSION;
			}
			// down switch is pressed
			if(downSwitch)
			{
				// set new display status: display settings		
				systemConfig.displayStatus = DISPLAY_STATE_MENU_SETTINGS;
			}
			// cancel switch is pressed
			if(cancelSwitch)
			{
				// call menu cancel routine
				menuCancel();
			}
			break;
		}
			
			// debug mode 1
			case DISPLAY_STATE_MENU_DBG1:
			{
				// ok switch is pressed
				if(okSwitch)
				{
					// do nothing
				}
				// up switch is pressed
				if(upSwitch)
				{
					// set new display status: debug mode 2
					systemConfig.displayStatus = DISPLAY_STATE_MENU_DBG2;
				}
				// down switch is pressed
				if(downSwitch)
				{
					// set new display status: debug mode 3
					systemConfig.displayStatus = DISPLAY_STATE_MENU_DBG3;
				}				
				// cancel switch is pressed
				if(cancelSwitch)
				{
					// set new display status: debug mode 
					systemConfig.displayStatus = DISPLAY_STATE_MENU_DBG;
				}
				break;
			}
			
			// debug mode 2
			case DISPLAY_STATE_MENU_DBG2:
			{
				// ok switch is pressed
				if(okSwitch)
				{
					// do nothing
				}
				// up switch is pressed
				if(upSwitch)
				{
					// set new display status: debug mode 3
					systemConfig.displayStatus = DISPLAY_STATE_MENU_DBG3;
				}
				// down switch is pressed
				if(downSwitch)
				{
					// set new display status: debug mode 1
					systemConfig.displayStatus = DISPLAY_STATE_MENU_DBG1;
				}
				// cancel switch is pressed
				if(cancelSwitch)
				{
					// set new display status: debug mode
					systemConfig.displayStatus = DISPLAY_STATE_MENU_DBG;
				}
				break;
			}
		
			// debug mode 3
			case DISPLAY_STATE_MENU_DBG3:
			{
				// ok switch is pressed
				if(okSwitch)
				{
					// do nothing
				}
				// up switch is pressed
				if(upSwitch)
				{
					// set new display status: debug mode 1
					systemConfig.displayStatus = DISPLAY_STATE_MENU_DBG1;
				}
				// down switch is pressed
				if(downSwitch)
				{
					// set new display status: debug mode 2
					systemConfig.displayStatus = DISPLAY_STATE_MENU_DBG2;
				}
				// cancel switch is pressed
				if(cancelSwitch)
				{
					// set new display status: debug mode
					systemConfig.displayStatus = DISPLAY_STATE_MENU_DBG;
				}
				break;
			}		
		
		// out of state? return to default state
		default:
		{
			// do nothing
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
	// - xxxx.0xxxb setting menu is inactive
	systemConfig.status &= ~0x08;
	// set new display status
	systemConfig.displayStatus = DISPLAY_STATE_DARK;
					
	// actualize matrix information
	displayMatrixInformation(0);
}
