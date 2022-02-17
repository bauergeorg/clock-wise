/*******************************************************************************
*
*	Author:			Georg Bauer (geo_bauer@gmx.net)
*	Date:			10.04.2014
*
*	Project-Title:	ClockWise
*	Description:	Variable description of basic's like time, status settings 
*
*	File-Title:		System File - Header File
*
*******************************************************************************
* Status description: variable 'status' uint8
*   ---x.xxxxb structure of variable
*
*	---x.xxx0b no time information in system available
*			  - the searching sequence is displayed
*	---x.xxx1b correct time information in system available
*			  - a time signal is displayed (if no menu is selected)
*
*	---x.xx0xb searching for dcf77-signal is inactive
*	---x.xx1xb searching for dcf77-signal is active
*
*	---x.x0xxb rtc time is not available
*	---x.x1xxb rtc time is available
*
*	---x.0xxxb setting menu is inactive
*	---x.1xxxb setting menu is active
*
*	---0.xxxxb automatic time mode is active
*	---1.xxxxb manual time mode is active
*
*******************************************************************************
* Status description: variable 'displayStatus' uint8 - see defines below
*
*	Default:
*	000d	total display is dark
*	001d	total display is bright - all leds are on
*	002d	show seaching mode
*
*	Time:
*	010d	actual system time in text mode
*	011d	actual system time in 00 lbr 00 mode
*	012d	actual system date in 00 lbr 00 mode
*	013d	actual system time/date in binary mode
*
*	Special Text messages:
*	020d	time to feed horses
*	021d	birthday time
*
*	Menu:
*	098d	wait to get into the menu
*	099d	- show version
*	100d	- time mode
*	110d		- automatic mode
*	111d			- search automatic dcf77 time - cancel
*	112D			- search automatic dcf77 time - ok
*	120d		- manual mode
*	121d			- set time - cancel
*	122d			- set time - ok
*	123d				- set hour manual
*	124d				- set minutes manual
*	125d				- set seconds manual
*	126d				- set year manual
*	127d				- set month manual
*	128d				- set day manual
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
*	232d			- brihtday and horse mode
*	240d		- searching sequence mode
*	241d			- square
*	242d			- no sequence
*	250d	- debug Mode
*	251d		- debug Mode 1
*	252d		- debug Mode 2
*	253d		- debug Mode 3
*
*******************************************************************************
* Display Settings: variable "displaySetting" unint8
*	vqxxyyyzb structure of variable
*
*		  zb (bit 0): shows pie variant
*		  0b original straight pie
*		  1b shifted pie (for 2 minutes)
*
*		yyyb (bit 3 to bit 1): shows character variant 
*		000b standard without birthday and horse information
*		001b birthday (01.01., 16.01., 07.08., 22.09., 08.12.) and horses@6pm
*		...b open for other variants
* 	
*	  xxb (bit 5 to bit 4):	open for other variants
*	  ..b open for other variants
*
*	 qb (bit 6): shows automatic display brightness variant
*	 0b automatic display brightness regulation is inactive 
*	 1b automatic display brightness regulation is active
* 
*	vb (bit 7): set's the display sequence while time searching mode
*	0b square & dot sequence when searching signal
*	1b no sequence when searching signal
*
*******************************************************************************
*/

//! libraries
#include <avr/io.h>
#include <stdint.h>
//#include <stdlib.h>

//! System Time Structure
struct time
{
	uint8_t  second;	// second
	uint8_t  minute;	// minute
	uint8_t  hour;		// hour
	uint8_t  day;		// day
	uint8_t  month;		// month	
	uint8_t  year;		// year
	uint8_t  weekday;	// weekday
};

//! System Parameter
struct systemParameter
{
	uint8_t status;					// See status description on top
	uint8_t lightIntensity;			// light intensity of measured value (range is 0 dark to 255 bright)
	uint8_t	potentiometerValue;		// set value of potentiometer (range is 0 low to 255 high)
	uint8_t displaySetting;			// see display settings description on top
	uint8_t displayBrightness;		// display brightness (range 0 dark to 255 bright)
	uint8_t displayStatus;			// actual display status
	uint8_t version;				// software system version
};

//! Functional prototypes
void initSystem(void);
uint8_t calcuateBrightness(uint8_t lighIntensity, uint8_t potentiometerValue);
uint8_t calculateIntensity(uint8_t intensity);
uint8_t calculatePotiValue(uint8_t potiValue);

//! Display State - horizontal (in rows)
// Default:
#define DISPLAY_STATE_DARK					0	// total display is dark
#define DISPLAY_STATE_BRIGHT				1	// total display is bright - all leds are on
#define DISPLAY_STATE_SEARCH				2	// show searching mode
// Time:
#define DISPLAY_STATE_TIME_TEXT				10	// actual system time in text mode
#define DISPLAY_STATE_TIME_NUMBER			11	// actual system time in 00 lbr 00 mode
#define DISPLAY_STATE_DATE_NUMBER			12	// actual system date in 00 lbr 00 mode
#define DISPLAY_STATE_TIME_BINARY			13	// actual system time/date in binary mode
// Special Text messages:
#define DISPLAY_STATE_SPECIAL_HORSES		20	// time to feed horses
#define DISPLAY_STATE_SPECIAL_BIRTHDAY		21	// birthday time
// Menu:
#define DISPLAY_STATE_MENU_WAIT				98	// wait to get into the menu
#define DISPLAY_STATE_MENU_VERSION			99	// - show version
#define DISPLAY_STATE_MENU_TIME_MODE		100	// - time mode
#define DISPLAY_STATE_MENU_AUTO_MODE		110	// 	- automatic mode
#define DISPLAY_STATE_MENU_AUTO_CANCEL		111	// 		- search automatic dcf77 time - cancel blink
#define DISPLAY_STATE_MENU_AUTO_OK			112	// 		- search automatic dcf77 time - ok blink
#define DISPLAY_STATE_MENU_MANUAL_MODE		120	// 	- manual mode
#define DISPLAY_STATE_MENU_MANUAL_CANCEL	121	// 		- set time - cancel blink
#define DISPLAY_STATE_MENU_MANUAL_OK		122	// 		- set time - ok blink
#define DISPLAY_STATE_MENU_SET_HOUR			123	// 			- set hour manual
#define DISPLAY_STATE_MENU_SET_MINUTE		124	// 			- set minutes manual
#define DISPLAY_STATE_MENU_SET_SECOND		125	// 			- set seconds manual
#define DISPLAY_STATE_MENU_SET_YEAR			126	// 			- set year manual
#define DISPLAY_STATE_MENU_SET_MONTH		127	// 			- set month manual
#define DISPLAY_STATE_MENU_SET_DAY			128	// 			- set day manual
#define DISPLAY_STATE_MENU_SET_WEEKDAY		129	// 			- set weekday manual
#define DISPLAY_STATE_MENU_SET_CANCEL		130	// 			- okay - take values
#define DISPLAY_STATE_MENU_SET_OK			131	// 			- cancel - reject values
#define DISPLAY_STATE_MENU_SETTINGS			200	// - display settings
#define DISPLAY_STATE_MENU_BRIGTHNESS		210	// 		- brightness
#define DISPLAY_STATE_MENU_BRIGHT_AUTO		211	// 			- automatic
#define DISPLAY_STATE_MENU_BRIGHT_MANU		212	// 			- manual
#define DISPLAY_STATE_MENU_PIE				220	// 		- pie
#define DISPLAY_STATE_MENU_PIE_STRAIGHT		221	// 			- straight
#define DISPLAY_STATE_MENU_PIE_SHIFT		222	// 			- shift
#define DISPLAY_STATE_MENU_CHAR				230	// 		- character variant
#define DISPLAY_STATE_MENU_CHAR_STANDARD	231	// 			- standard
#define DISPLAY_STATE_MENU_CHAR_BIRTHDAY	232	// 			- brithday and horse mode
#define DISPLAY_STATE_MENU_SEARCH_MODE		240	// 		- searching sequence mode
#define DISPLAY_STATE_MENU_SEARCH_SQUARE	241	// 			- square
#define DISPLAY_STATE_MENU_SEARCH_NO		242	// 			- no sequence
#define DISPLAY_STATE_MENU_DBG				250 // - debug Mode
#define DISPLAY_STATE_MENU_DBG1				251 //		- debug Mode 1
#define DISPLAY_STATE_MENU_DBG2				252 //		- debug Mode 2
#define DISPLAY_STATE_MENU_DBG3				253 //		- debug Mode 3