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
* Status description: variable 'menuState' uint8
*	000d	default state (press okey and cancel at the same time)
*	001d	- show version
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