/*******************************************************************************
*
*	Author:			Georg Bauer
*	Date:			06.01.2014
*
*	Project-Title:	ClockWise
*	Description:	Receiving and decoding DCF77 Signal
*
*	File-Title:		DCF 77 (Reichelt Receiver)
*
*******************************************************************************
*
* Status LED:
* - On boot up the yellow led is switched on the led starts blinking
*	(on time 0,1s or 0,2s every second), when signals is receiving
* - The red LED switches off, if time value was decode correctly
*
*******************************************************************************
*
* Pin Declaration:
*	Pin						| Description
*	------------------------|-------------------------------------------------
*	PC7 (Pin 29) as output	| activation signal for dcf77 signal (low active)
*	PC6 (Pin 28) as input	| external interrupt isr will activated if
*							| dcf signal changes (PCINT22)
*							| signal is normally high, the signal is 0,1s
*							| (zero) or 0,2s (one) is low.
*	------------------------|-------------------------------------------------
*
*******************************************************************************
*
* Interrupt:
*	External Pin Change Interrupt PC6 (PCINT22) used for external dcf signal
*	- interrupt routine is active when dcf77 signal changes
*	Timer 0 interrupt service routine is every 16,384ms active
*
* Timer:
*	Timer 0 is used for sampling the dcf 77 signal at 16,384ms (61,51757813Hz)
*
*******************************************************************************
*/

//! Libraries
#include "dcf77.h"
#include "system.h"
#include "gpios.h"
#include "rtc.h"

//! Own global variables
// Flag for receiving dcf77 signal
volatile uint8_t dcfActive = 0;
// Array for saving receiving dcf77 signal
volatile uint8_t dcfArray [60];

//! Extern globals variables
extern volatile struct time systemTime;
extern volatile struct systemParameter systemConfig;

//! Initialize dcf77
void initDcf77(void)
{
	/*
	//! external interrupt for signal of dcf 77 receiver
	// enabled external pin change interrupts PCINT23:16 
	PCICR |= 1 << PCIE2;
	// activate PC2 (PCINT22) as external interrupt
	PCMSK2 |= 1 << PCINT22;
	// delete flag for interrupts PCINT23:16 
	PCIFR |= 1 << PCIF2;
	*/
	
	//! timer for decoding dcf77 signal
	// 8 bit timer/counter 0
	// clock select: prescale 1024 -> 16,384ms (61,51757813Hz)
	TCCR0B = (1 << CS02) | (1 << CS00);	
	// enable timer/counter 0 interrupt overflow 
	TIMSK0 |= (1 << TOIE0);
		
	//! port settings for activation signal of dcf77 receiver
	// enable when pc7 low
	// disable when pc7 high
	// activate PC7 as output
	DDRC |= (1 << PC7);
	// switch PC7 high
	PORTC |= (1 << PC7);
			
	//! port settings for input signal of dcf77 receiver
	// activate PC6 as input
	DDRC &= ~(1 << PC6);
	// disable PC6 pull-up resistor (tristate)
	PORTC &= ~(1 << PC6);
}

//! Check for plausibility
// return value is '1', means a correct detection
// return value is '0', means a failure 
uint8_t plausibilityCheck (	uint8_t hourNew, uint8_t minuteNew, uint8_t hourOld, uint8_t minuteOld)
{
	// calculate old time value + 1 minute
	minuteOld++;
	if (minuteOld == 60)
	{
		minuteOld = 0;
		hourOld++;
		if (hourOld == 24)
		{
			hourOld = 0;			
		}
	}
	
	// compare new time values with old time values
	// if two received decoded time values equal, return a correct return flag 
	if (minuteNew == minuteOld)
	{
		if (hourNew == hourOld)
		{
			return 1;
		}
	}
	return 0;
}

//! Decode dcf77 received bits
void decodeDcf77(void)
{
	// static variables for time values for next decode session
	static uint8_t minuteOld = 0;
	static uint8_t hourOld = 0;
	
	// variables for actual received values
	uint8_t minute = 0;
	uint8_t hour = 0;
	uint8_t day = 0;
	uint8_t month = 0;
	uint8_t year = 0;
	uint8_t weekday = 0;
	uint8_t parity = 0;
	
	// decode minute information
	// minutes
	// 21 22 23 24 25 26 27 28
	// 1m 2m 4m 8m 10 20 50 pm
	if (dcfArray[21] == 1)
	{
		minute = 1;	// add 1 minute
		parity++;
	}
	if (dcfArray[22] == 1)
	{
		minute += 2;	// add 2 minutes
		parity++;
	}
	if (dcfArray[23] == 1)
	{
		minute += 4;	// add 4 minutes
		parity++;
	}
	if (dcfArray[24] == 1)
	{
		minute += 8;	// add 8 minutes
		parity++;
	}
	if (dcfArray[25] == 1)
	{
		minute += 10;	// add 10 minutes
		parity++;
	}
	if (dcfArray[26] == 1)
	{
		minute += 20;	// add 20 minutes
		parity++;
	}
	if (dcfArray[27] == 1)
	{
		minute += 40;	// add 40 minutes
		parity++;
	}
	
	if (dcfArray[28] == 1)
	{
		// minute parity bit okay?
		if (parity % 2)//parity == 1 || parity == 3 || parity == 5 || parity == 7)
		{
			parity = 0;
		}
		else
		{
			return;
		}
	}
	else {
		// minute parity bit okay?
		if (!(parity % 2))//parity == 0 || parity == 2 || parity == 4 || parity == 6)
		{
			parity = 0;
		}
		else
		{
			return;
		}
	}	
	
	// decode hour information
	// hours
	// 29 30 31 32 33 34 35
	// 1h 2h 4h 8h 10 20 ph
	if (dcfArray[29] == 1)
	{	
		hour = 1;		// add 1 hour
		parity++;
	}
	if (dcfArray[30] == 1)
	{	
		hour += 2;		// add 2 hours
		parity++;
	}
	if (dcfArray[31] == 1)
	{	
		hour += 4;		// add 4 hours
		parity++;
	}
	if (dcfArray[32] == 1)
	{	
		hour += 8;		// add 8 hours
		parity++;
	}
	if (dcfArray[33] == 1)
	{	
		hour += 10;	// add 10 hours
		parity++;
	}
	if (dcfArray[34] == 1)
	{	
		hour += 20;	// add 20 hours
		parity++;
	}
	
	if (dcfArray[35] == 1)
	{	
		// hour parity bit okay?
		if (parity % 2)//parity == 1 || parity == 3 || parity == 5)
		{
			parity = 0;
		}
		else
		{
			return;
		}
	}
	else
	{						
		// hour parity bit okay?
		if (!(parity % 2))//parity == 0 || parity == 2 || parity == 4 || parity == 6)
		{
			parity = 0;
		}
		else
		{
			return;
		}
	}
	
	// decode date information
	// day
	// 36 37 38 39 40 41
	// 1d 2d 4d 8d 10 20
	
	if (dcfArray[36] == 1)
	{
		day = 1;	// add 1 day
		parity++;
	}
	if (dcfArray[37] == 1)
	{
		day += 2;	// add 2 days
		parity++;
	}
	if (dcfArray[38] == 1)
	{
		day += 4;	// add 4 days
		parity++;
	}
	if (dcfArray[39] == 1)
	{
		day += 8;	// add 8 days
		parity++;
	}
	if (dcfArray[40] == 1)
	{
		day += 10;	// add 10 days
		parity++;
	}
	if (dcfArray[41] == 1)
	{
		day += 20;	// add 20 days
		parity++;
	}
	
	// day
	// 42 43 44
	// 1d 2d 4d
	
	if (dcfArray[42] == 1)
	{
		weekday = 1;	// add 1 day
		parity++;
	}	
	if (dcfArray[43] == 1)
	{
		weekday += 2;	// add 2 days
		parity++;
	}	
	if (dcfArray[44] == 1)
	{
		weekday += 4;	// add 4 days
		parity++;
	}
	
	// month
	// 45 46 47 48 49
	// 1m 2m 4m 8m 10
	
	if (dcfArray[45] == 1)
	{
		month = 1;	// add 1 month
		parity++;
	}	
	if (dcfArray[46] == 1)
	{
		month += 2;	// add 2 months
		parity++;
	}	
	if (dcfArray[47] == 1)
	{
		month += 4;	// add 4 months
		parity++;
	}	
	if (dcfArray[48] == 1)
	{
		month += 8;	// add 8 months
		parity++;
	}	
	if (dcfArray[49] == 1)
	{
		month += 10;// add 10 months
		parity++;
	}
	
	// year
	// 50 51 52 53 54 55 56 57
	// 1y 2y 4y 8y 10 20 40 80
	
	if (dcfArray[50] == 1)
	{
		year = 1;	// add 1 year
		parity++;
	}
	if (dcfArray[51] == 1)
	{
		year += 2;	// add 2 years
		parity++;
	}
	if (dcfArray[52] == 1)
	{
		year += 4;	// add 4 years
		parity++;
	}
	if (dcfArray[53] == 1)
	{
		year += 8;	// add 8 years
		parity++;
	}
	if (dcfArray[54] == 1)
	{
		year += 10;// add 10 years
		parity++;
	}
	if (dcfArray[55] == 1)
	{
		year += 20;// add 20 years
		parity++;
	}
	if (dcfArray[56] == 1)
	{
		year += 40;// add 40 years
		parity++;
	}
	if (dcfArray[57] == 1)
	{
		year += 80;// add 80 years
		parity++;
	}
		
	if (dcfArray[58] == 1)
	{
		// minute parity bit okey?
		if (parity % 2)
		{
			parity = 0;
		}
		else
		{
			return;
		}
	}
	else {
		// minute parity bit okey?
		if (!(parity % 2))
		{
			parity = 0;
		}
		else
		{
			return;
		}
	}
	
	// check for plausibility
	if (plausibilityCheck (hour, minute, hourOld, minuteOld))
	{
		// if plausibility check okay, set global time values
		systemTime.hour = hour;	
		systemTime.minute = minute;
		systemTime.second = 0;
		systemTime.day = day;
		systemTime.month = month;
		systemTime.year = year;
		systemTime.weekday = weekday;
				
		stopDcf77Signal();
		
		setTimeToRtc(systemTime.hour, systemTime.minute, systemTime.second, systemTime.weekday, systemTime.month, systemTime.year, systemTime.weekday);
	}
	
	// save actual time values for next decode session
	minuteOld = minute;
	hourOld = hour;
}

//! activate dcf77 signal
void startDcf77Signal(void)
{

	// set default system status
	// - xxxx.xx1xb searching dcf77 signal active
	systemConfig.status |= 0x02;
	
	//! external interrupt for signal of dcf 77 receiver
	// enabled external pin change interrupts PCINT23:16
	PCICR |= 1 << PCIE2;
	// activate PC2 (PCINT22) as external interrupt
	PCMSK2 |= 1 << PCINT22;
	// delete flag for interrupts PCINT23:16
	PCIFR |= 1 << PCIF2;
	
	// enable when pc7 low
	// switch PC7 low
	PORTC &= ~(1 << PC7);
	
	// test
	switchOnStatusRed();
}

//! deactivate dcf77 signal
void stopDcf77Signal(void)
{
	// set default system status
	// - xxxx.xxx1b time information in system available
	systemConfig.status |= 0x01;
	// - xxxx.xx0xb searching dcf77 signal inactive
	systemConfig.status &= ~0x02;

	//! external interrupt for signal of dcf 77 receiver
	// disabled external pin change interrupts PCINT23:16
	PCICR &= ~(1 << PCIE2);
	// inactivate PC2 (PCINT22) as external interrupt
	PCMSK2 &= ~(1 << PCINT22);
	// delete flag for interrupts PCINT23:16
	PCIFR |= 1 << PCIF2;
	
	// test
	switchOffStatusRed();
	
	// disable when pc7 high
	// switch PC7 on
	PORTC |= (1 << PC7);
}

//! Interrupt Service Routine for when DCF77 signal changes
ISR(PCINT2_vect)			// start signal 0,1s or 0,2s 
{
	// reset of timer 0
	TCNT0 = 0;						
	// set dcf77 receive flag 
	dcfActive = 1;
	
	// set status led yellow
	switchOnStatusYellow();
	
	// deactivate external interrupt, if signal is complete, interrupt will be activated
	PCMSK2 &= ~(1 << PCINT22);
}

//! Interrupt Service Routine for when Timer/Counter 0 has an overflow
// if variable "dcf_active" is set, this routine will called every 16,384ms (61,51757813Hz)
// calculated by: 16MHz / 1024 [timer 0 clock divider] / 2^8 [8bit counter] = 61,51757813Hz
ISR(TIMER0_OVF_vect)
{
	// local static variables
	static uint16_t breakCount = 0;
	static uint8_t timeCount = 0;
	static uint8_t arrayCount = 0;
	
	// signal is active
	if (dcfActive)
	{
	
		// is signal pause more than 1,5s (dcf77 char 58)
		// reset all counters and activate the external interrupt
		if (breakCount > 91)
		{
			// if 58th characters received (array counter is out of range) 
			// run the execution function 
			if (arrayCount >= 58)
			{
				decodeDcf77();
			}

			// reset break counter
			breakCount = 0;
			// reset signal char counter
			arrayCount = 0;
			// activate PC6 (PCINT22) as external interrupt
			PCMSK2 |= (1 << PCINT22);

			return;
		}

		// reset break counter
		breakCount = 0;
		// if low level, count the time
		if (!(PINC & (1 << PC6)))
		{	
			timeCount++;	
		}
		// if high level, signal is 
		else 
		{					
			// decide if last char was short 0,1s (zero) or long 0,2s (one)
			// limit is set at 0,163s is equal with timecount = 9
			if (timeCount <= 9)
			{
				dcfArray[arrayCount] = 0;
			}
			else
			{
				dcfArray[arrayCount] = 1;
			}
					
			// increment signal char counter
			arrayCount++;

			// reset dcf77 receive flag
			dcfActive = 0;
			
			// switch off status led yellow
			switchOffStatusYellow();

			// reset time counter
			timeCount = 0;
			// activate PC6 (PCINT22) as external interrupt
			PCMSK2 |= 1 << PCINT22;
		}
	}
	else
	{
		// no signal, counting at signal pause (break counter)
		breakCount++;
	}
}