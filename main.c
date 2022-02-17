/*******************************************************************************
*
*	Author:			Georg Bauer (geo_bauer@gmx.net)
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
#include "rtc.h"
#include "gpios.h"
#include "timeMgnt.h"

//#include "usart.h"
#include "adc.h"

#include <util/delay.h>

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
	initSystem();
	initGpios();
	initTimeMgnt();
//	initRtc();
	initAdc();
	initMatrix();	
	initDcf77();
	initTasks();

	// read light intensity value of adc
	systemConfig.lightIntensity = calculateIntensity(adcRead(0));
	// real potentiometer value of adc
	systemConfig.potentiometerValue = calculatePotiValue(adcRead(1));
	// calculate display brightness value
	systemConfig.displayBrightness = calcuateBrightness(systemConfig.lightIntensity, systemConfig.potentiometerValue);

	// enable global interrupt
	sei();
	
	// start receiving
	startDcf77Signal();
		
	// endless loop
    while (1) 					
   {
		// when do nothing
		checkForTask();
    }	
}
