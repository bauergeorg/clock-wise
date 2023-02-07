/*******************************************************************************
*
*	Author:			Georg Bauer
*	Date:			06.01.2014
*
*	Project-Title:	ClockWise
*	Description:	µC controlled clock with DCF77 and a led-matrix
*
*	File-Title:		Main File
*
*******************************************************************************
*
* After a time of 3 minutes the actual time signal is saved as main time
*
* Status LED:
* - The green LED toggles every second
* -	On boot up the yellow led is switched on, the led starts blinking
*	(on time 0,1s or 0,2s every second), when signals is receiving
* - The red LED switches off, if time value was decode correctly *!!!!test!!!!!
*
*******************************************************************************
*
* Fuse settings by programming via SPI:
*	BODLEVEL = DISABLED
*	OCDEN = [X]
*	JTAGEN = [X]
*	SPIEN = [ ]
*	WDTON = [ ]
*	EESAVE = [ ]
*	BOOTSZ = 4096W_F000
*	BOOTRST = [ ]
*	CKDIV8 = [ ]
*	CKOUT = [X]
*	SUT_CKSEL = EXTXOSC_8MHZ_XX_1CK_65MS
*
*	EXTENDED = 0xFF (valid)
*	HIGH = 0xB9 (valid)
*	LOW = 0xCF (valid)
*
*******************************************************************************
*/
  
//! Libraries
//#include <stdint.h>
#include "settings.h"
#include "taskMgnt.h"
#include "system.h"
#include "dcf77.h"
#include "ledMatrix.h"
#include "i2c.h"
#include "rtc.h"
#include "gpios.h"
#include "timeMgnt.h"

//#include "usart.h"
#include "adc.h"

//#include <util/delay.h>

//! Extern global variables
extern volatile struct systemParameter systemConfig;
extern volatile struct time systemTime;
extern volatile uint8_t dcfArray[60];

// definition of the pause
const double DELAYUART = 1;  // 1µs	
	
//! Main routine
// just do nothing is the best way ;)
int main(void)
{
//	uint8_t data= 0;
//	uint8_t i = 0;
	
	// init functions
	initSystem();		// global settings
	initGpios();		// external interrupts via switches
	initTimeMgnt();		// timer 1 for time management
	I2C_Init();			// i2c communication
	initRtc();			// rtc communication
	initAdc();			// adcs for brightness and
	initMatrix();		// matrix management
	initDcf77();		// dcf77 management
	initTasks();		// task management

	// read light intensity value of adc
	systemConfig.lightIntensity = calculateIntensity(adcRead(0));
	// read potentiometer value of adc
	systemConfig.potentiometerValue = calculatePotiValue(adcRead(1));
	// calculate display brightness value
	systemConfig.displayBrightness = calcuateBrightness(systemConfig.lightIntensity, systemConfig.potentiometerValue);

	// enable global interrupt
	sei();
	
	// switsch off test leds
	switchOffStatusYellow();
	switchOffStatusRed();
	
	// nur übergangsweise für version v0.0.2
	// nehme default time und suche nicht automatisch im boot-up
	
	// set default system status
	// - xxxx.xxx0b time value not available
	// - xxxx.xx0xb searching dcf77 signal active
	// - xxxx.x0xxb rtc time is not available
	// - xxxx.0xxxb setting menu is inactive
	// - xxx0.xxxxb automatic time mode is active
	systemConfig.status = 0x00;

	// ONLY FOR DEBUG!!!!! below!
	// set rtc time
	setTimeToRtc(3, 3, 3);
	// set system status
	// - xxxx.xxx1b time information in system available - a time signal is displayed (if no menu is selected)
	// - xxx1.xxxxb manual time mode is active
	// - xxxx.x1xxb rtc time is active
	systemConfig.status |= 0x15;
	// - xxxx.xx0xb searching for dcf77-signal is inactive
	systemConfig.status &= ~0x02;
	updateTimeWithRtcValues();
	// ONLY FOR DEBUG!!!!! above!
	
	// check if rtc device has valid values (set status)
	//checkRtcTime();
	// if device has valid values, take this values to display
	if(systemConfig.status & 0x04)
	{
		// update time values (set status)
		updateTimeWithRtcValues();
	}
	// scan dcf 77 signal
	else
	{
		// start receiving
		startDcf77Signal();					
		// set new display status: show searching mode
		systemConfig.displayStatus = DISPLAY_STATE_SEARCH;
	}
					
/*
	// check for data from rtc
	// if the rtc communication is failed 
	if(~getTimeFromRtc())
	{
		// start receiving
		startDcf77Signal();
		// set default system status
		// - xxxx.xx1xb searching dcf77 signal active
		systemConfig.status |= 0x02;
		// set display status to searching sequence
		systemConfig.displayStatus = DISPLAY_STATE_SEARCH;
		// - xxxx.xxx0b no time value available
		// - xxxx.x0xxb rtc time is not available
		systemConfig.status &= ~0x05;
	}
	else
	{
		// set default system status
		// - xxxx.xxx1b time information in system available
		// - xxxx.x1xxb rtc time is available
		systemConfig.status |= 0x05;
		// - xxxx.xx0xb searching dcf77 signal inactive
		systemConfig.status &= ~0x02;
	}
*/
	// endless loop
    while (1) 					
	{
		// when do nothing
		checkForTask();
		
		// and
			// read light intensity value of adc
			systemConfig.lightIntensity = calculateIntensity(adcRead(0));
			// read potentiometer value of adc
			systemConfig.potentiometerValue = calculatePotiValue(adcRead(1));
			// calculate display brightness value
			systemConfig.displayBrightness = calcuateBrightness(systemConfig.lightIntensity, systemConfig.potentiometerValue);
    }	
}
