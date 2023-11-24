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

#include "usart0.h"

//! Own global variables
// Flag for receiving dcf77 signal
volatile uint8_t dcfActive = 0;
volatile uint8_t breakActive = 0;
volatile uint8_t estimateReceivingType = 0;		// 0 = don't know
												// 1 = signal
												// 2 = break

// Array for saving receiving dcf77 signal
volatile uint8_t dcfArray[60];
// counting at signal pause (break counter)
volatile uint16_t breakCount = 0; 


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
uint8_t plausibilityCheck(uint8_t hourNew, uint8_t minuteNew, uint8_t hourOld, uint8_t minuteOld)
{

	// compare new time values with old time values
	// if two received decoded time values equal, return a correct return flag 
	if ((minuteNew == (minuteOld-1)) || (minuteNew == 0 && (minuteOld == 59)))
	{
		if (hourNew == hourOld)
		{
			
			return 1;
		}
	}
		
	return 0;
}

void updateDcf77_backupTime(uint8_t *hourOld, uint8_t *minuteOld)
{
	// calculate old time value + 1 minute
	// with overflow handling
	*minuteOld = *minuteOld + 1;
	if (*minuteOld == 60)
	{
		*minuteOld = 0;
		*hourOld = *hourOld +1;
		if (*hourOld == 24)
		{
			*hourOld = 0;
		}
	}
}

//! Decode dcf77 received bits
void decodeDcf77(void)
{
	// static variables for time values for next decode session
	static uint8_t minuteOld = 0;
	static uint8_t hourOld = 0;

	uint8_t check = 0;

	// variables for actual received values
	uint8_t minute = 0;
	uint8_t hour = 0;
	uint8_t day = 0;
	uint8_t month = 0;
	uint8_t year = 0;
	uint8_t weekday = 0;
	uint8_t parity = 0;
	
	updateDcf77_backupTime(&hourOld, &minuteOld);
	
	// decode minute information
	// minutes
	// 21 22 23 24 25 26 27 28
	// 1m 2m 4m 8m 10 20 50 pm
	if (dcfArray[21] == 1)
	{
		minute += 1;	// add 1 minute
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
		hour += 1;		// add 1 hour
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
		day += 1;	// add 1 day
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
		weekday += 1;	// add 1 day
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
		month += 1;	// add 1 month
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
		year += 1;	// add 1 year
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
		// minute parity bit okay?
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
		// minute parity bit okay?
		if (!(parity % 2))
		{
			parity = 0;
		}
		else
		{
			return;
		}
	}
	
	// stop signal		
	stopDcf77Signal();

	usart0ReceiveTransmit(hour);
	usart0ReceiveTransmit(minute);
	usart0ReceiveTransmit(0);
	usart0ReceiveTransmit(day);
	usart0ReceiveTransmit(month);
	usart0ReceiveTransmit(year);
	usart0ReceiveTransmit(weekday);
	usart0ReceiveTransmit(0x0d); // CR
	usart0ReceiveTransmit(0x0a); // LF

	// check received time
	check = plausibilityCheck(hour, minute, hourOld, minuteOld);

	// check for plausibility
	// if plausibility check okay, set global time values
	if (check == 1)
	{
		systemTime.hour = hour;	
		systemTime.minute = minute;
		systemTime.second = 0;
		systemTime.day = day;
		systemTime.month = month;
		systemTime.year = year;
		systemTime.weekday = weekday;

		// write into rtc
		setTimeToRtc(systemTime.hour, systemTime.minute, systemTime.second, systemTime.weekday, systemTime.day, systemTime.month, systemTime.year);
		
		// disable red led
		switchOffStatusRed();
		
	}
	// if plausibility check NOT okay
	else
	{
		// restart dcf signal detection
		startDcf77Signal();
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
	
	// current receiving is a dont-know 
	estimateReceivingType = 0;

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

	// enable timer/counter 0 interrupt overflow 
	TIMSK0 |= (1 << TOIE0);
	
	// reset break counter
	breakCount = 0;

	usart0ReceiveTransmit(0x44); // D
	usart0ReceiveTransmit(0x43); // C
	usart0ReceiveTransmit(0x46); // F
	usart0ReceiveTransmit(0x20); // (space)
	usart0ReceiveTransmit(0x73); // s
	usart0ReceiveTransmit(0x74); // t
	usart0ReceiveTransmit(0x61); // a
	usart0ReceiveTransmit(0x72); // r
	usart0ReceiveTransmit(0x74); // t
	usart0ReceiveTransmit(0x0d); // CR
	usart0ReceiveTransmit(0x0a); // LF

}

//! deactivate dcf77 signal
void stopDcf77Signal(void)
{

	// disable timer/counter 0 interrupt overflow 
	TIMSK0 &= ~(1 << TOIE0);
	
	// set default system status
	// - xxxx.xxx1b time information in system available
	systemConfig.status |= 0x01;
	// - xxxx.xx0xb searching dcf77 signal inactive
	systemConfig.status &= ~0x02;

	//! external interrupt for signal of dcf 77 receiver
	// disabled external pin change interrupts PCINT23:16
	PCICR &= ~(1 << PCIE2);
	// deactivate PC2 (PCINT22) as external interrupt
	PCMSK2 &= ~(1 << PCINT22);
	// delete flag for interrupts PCINT23:16
	PCIFR |= 1 << PCIF2;
	
	// test
	//switchOffStatusRed();
	
	// disable when pc7 high
	// switch PC7 on
	PORTC |= (1 << PC7);

	// reset break counter
	breakCount = 0;

	usart0ReceiveTransmit(0x44); // D
	usart0ReceiveTransmit(0x43); // C
	usart0ReceiveTransmit(0x46); // F
	usart0ReceiveTransmit(0x20); // (space)
	usart0ReceiveTransmit(0x73); // s
	usart0ReceiveTransmit(0x74); // t
	usart0ReceiveTransmit(0x6f); // o
	usart0ReceiveTransmit(0x70); // p
	usart0ReceiveTransmit(0x0d); // CR
	usart0ReceiveTransmit(0x0a); // LF

	// uint8_t i = 0; 
	// for (i=0; i<59; i++)
	// {
	// 	usart0ReceiveTransmit(dcfArray[i]);
	// }
	// usart0ReceiveTransmit(0x0d); // CR
	// usart0ReceiveTransmit(0x0a); // LF

	// usart0ReceiveTransmit(systemTime.hour);
	// usart0ReceiveTransmit(systemTime.minute);
	// usart0ReceiveTransmit(systemTime.second);
	// usart0ReceiveTransmit(systemTime.day);
	// usart0ReceiveTransmit(systemTime.month);
	// usart0ReceiveTransmit(systemTime.year);
	// usart0ReceiveTransmit(systemTime.weekday);
	// usart0ReceiveTransmit(0x0d); // CR
	// usart0ReceiveTransmit(0x0a); // LF

}

//! Interrupt Service Routine for when DCF77 signal changes
ISR(PCINT2_vect)			// start signal 0,1s or 0,2s 
{
	// reset of timer 0
	TCNT0 = 0;						
	// set dcf77 receive flag (signal is low)
	dcfActive = 1;
	// set status led yellow
	switchOnStatusYellow();
	
	// deactivate external interrupt, if signal is complete, interrupt will be activated
	PCMSK2 &= ~(1 << PCINT22);
}

//! Interrupt Service Routine for when Timer/Counter 0 has an overflow
// if variable "dcf_active" is set, this routine will called every 16,384ms (61,03515625Hz)
// calculated by: 16MHz / 1024 [timer 0 clock divider] / 2^8 [8bit counter] = 61,03515625Hz
ISR(TIMER0_OVF_vect)
{
	// local static variables
	static uint8_t signalCount = 0; 	// counting time of signal high level
	static uint8_t arrayCount = 0; 	// index pointer to save value in dcfArray array
	
	uint8_t breakFinishedFlag  = 0;
	uint8_t signalFinishedFlag = 0;

	uint8_t current_distortion = 0;
	uint8_t abort 			   = 0;

	// * Time counter section *
	// signal is active (signal is low)
	if (dcfActive)
	{
		// if low level, count the time
		if (!(PINC & (1 << PC6)))
		{	
			// edge detected: break is finished -> set break finished flag
			if (breakActive == 1)
			{
				breakFinishedFlag = 1;
				// debug: 0x80 = break is finished
				usart0ReceiveTransmit(0x80); 
			}
			// reset break active flag
			breakActive = 0;

			// count signal time
			signalCount++;
		}
		// if high level, signal is finished
		else 
		{					
			// edge detected: signal is finished -> set signal finished flag
			signalFinishedFlag = 1;
			// debug: 0x82 = signal is finished
			usart0ReceiveTransmit(0x82);	

			// counting at signal pause (break counter)
			breakCount++;
			// set break active flag
			breakActive = 1;

			// reset dcf77 receive flag
			dcfActive = 0;
			// switch off status led yellow
			switchOffStatusYellow();
			// activate PC6 (PCINT22) as external interrupt
			PCMSK2 |= 1 << PCINT22;
		}
	}
	else
	{
		// no signal, counting at signal pause (break counter)
		breakCount++;
		// set break active flag
		breakActive = 1;
	}

	// * Evaluate counted times *

	// in case of new signal is finished: TODO Error handling!!!!
	if(signalFinishedFlag && breakFinishedFlag)
	{
		usart0ReceiveTransmit(0x91);		// debug: 0x92 = ERROR
	}

	// in case of new break is finished
	if(breakFinishedFlag)
	{
		// is signal pause smaller than 0,73728s
		// a invalid pause signal is measured
		if (breakCount < 45)
		{
			// short distortion
			current_distortion = 0x01;
			// do net reset break counter
			// switch on status led red
			switchOnStatusRed();
			// debug: 0x21 = invalid pause detected
			usart0ReceiveTransmit(0x21);	

			// receiving a signal
			if (estimateReceivingType == 1)
			{
				// break count to signal
				signalCount = signalCount + breakCount;
				// reset break counter
				breakCount = 0;
			}
			// receiving a break
			else if (estimateReceivingType == 2)
			{
				// reset time counter
				signalCount = 0;
			}
			// receiving a dont-know
			else
			{
				// reset time counter
				signalCount = 0;
				// reset break counter
				breakCount = 0;
			}
		}	
		// is signal pause bewteen 0,73728s and 0,98304s
		// a valid pause signal is measured
		else if (breakCount >= 45 && breakCount <= 60)
		{
			// error signal: receiving a signal
			if (estimateReceivingType == 1)
			{
				// short distortion
				current_distortion = 0x01;
				// do net reset break counter
				// switch on status led red
				switchOnStatusRed();
				// debug: 0x23 = invalid pause detected
				usart0ReceiveTransmit(0x23);

				// break count to signal
				signalCount = signalCount + breakCount;
				// reset break counter
				breakCount = 0;
			}
			// valid signal: receiving a break or a dont-know
			else
			{
				// next receiving is a signal 
				estimateReceivingType = 1;
				
				// reset break counter
				breakCount = 0;
				// reset time counter
				signalCount = 0;
				// debug: 0x20 = valid pause detected
				usart0ReceiveTransmit(0x20);
				usart0ReceiveTransmit(estimateReceivingType);
			}
		}
		// is signal pause more than 1,5s (dcf77 char 58)
		// reset all counters and activate the external interrupt
		else if (breakCount > 91 && breakCount <= 125)
		{
			
			// error signal: receiving a signal
			if (estimateReceivingType == 1)
			{
				// short distortion
				current_distortion = 0x01;
				// do net reset break counter
				// switch on status led red
				switchOnStatusRed();
				// debug: 0x25 = invalid pause detected
				usart0ReceiveTransmit(0x25);

				// break count to signal
				signalCount = signalCount + breakCount;
				// reset break counter
				breakCount = 0;
			}
			// valid signal: receiving a break or a dont-know
			else
			{
				// next receiving is a signal 
				estimateReceivingType = 1;

				// debug: 0x22 = long time break (>1,5 seconds)
				usart0ReceiveTransmit(0x22);
				usart0ReceiveTransmit(estimateReceivingType);

				// if 58th characters received (array counter is out of range) 
				// run the execution function 
				if (arrayCount >= 58)
				{
					// debug: 0x24 = char 58 detected
					usart0ReceiveTransmit(0x24);		
					decodeDcf77();
				}
				else
				{
					// new try >> reset all <<
					
					// reset break counter
					breakCount = 0;
					// reset signal counter
					signalCount = 0;
					// reset signal char counter
					arrayCount = 0;
					// activate PC6 (PCINT22) as external interrupt
					PCMSK2 |= (1 << PCINT22);
				}

				return;
			}
		}
		// is signal pause more than 2s: abort
		// reset all counters and activate the external interrupt
		else if (breakCount > 125)
		{

			// short distortion
			current_distortion = 0x01;
			// do net reset break counter
			// switch on status led red
			switchOnStatusRed();
			// debug: 0x2B = invalid pause detected
			usart0ReceiveTransmit(0x2B);

			// break count to signal
			signalCount = signalCount + breakCount;
			// reset break counter
			breakCount = 0;

			// next receiving is a dont-know 
			estimateReceivingType = 0;

			// reset break counter
			breakCount = 0;
			// reset signal char counter
			arrayCount = 0;
			// activate PC6 (PCINT22) as external interrupt
			PCMSK2 |= (1 << PCINT22);

			return;
		}

		else
		// a invalid pause signal is measured
		{		
			// short distortion
			current_distortion = 0x01;
			// do net reset break counter
			// switch on status led red
			switchOnStatusRed();
			// debug: 0x29 = invalid pause detected
			usart0ReceiveTransmit(0x29);	

			// receiving a signal
			if (estimateReceivingType == 1)
			{
				// break count to signal
				signalCount = signalCount + breakCount;
				// reset break counter
				breakCount = 0;
			}

			// receiving a break 
			else if (estimateReceivingType == 2)
			{
				// reset time counter
				signalCount = 0;
			}
			// receiving a dont-know
			else
			{
				// reset time counter
				signalCount = 0;
				// reset break counter
				breakCount = 0;
			}
		}
	}

	// in case of new signal is finished
	if(signalFinishedFlag)
	{
		// signal distortion - failure on receiving signal: do nothing
		// decide if last char was short 0,1s (zero: 0,1s/16,384ms=6,1) or long 0,2s (one: 0,2s/16,384ms=12,2)
		// distortion: shorter than 4 (0,049152s)
		if (signalCount < 3)
		{
			// short distortion
			current_distortion = 0x01;
			// switch on status led red
			switchOnStatusRed();
			// debug: 0x11 = invalid signal detected
			usart0ReceiveTransmit(0x11);

			// receiving a break
			if (estimateReceivingType == 2)
			{
				// singal count to break
				breakCount = breakCount + signalCount;

				// reset dcf77 receive flag
				dcfActive = 0;
				// switch off status led yellow
				switchOffStatusYellow();
				// reset time counter
				signalCount = 0;
				// clear flag for next time
				//last_signal_was_distorted = 0x00;
				// activate PC6 (PCINT22) as external interrupt
				PCMSK2 |= 1 << PCINT22;
			}
			// receiving a signal
			else if (estimateReceivingType == 1)
			{
				// // singal count to break
				// breakCount = breakCount + signalCount
				// reset dcf77 receive flag
				dcfActive = 0;
				// switch off status led yellow
				switchOffStatusYellow();
				// // reset time counter
				// signalCount = 0;
				// clear flag for next time
				//last_signal_was_distorted = 0x00;
				// activate PC6 (PCINT22) as external interrupt
				PCMSK2 |= 1 << PCINT22;
			}
			// receiving a dont-know
			else
			{
				// // singal count to break
				// breakCount = breakCount + signalCount
				// reset dcf77 receive flag
				dcfActive = 0;
				// switch off status led yellow
				switchOffStatusYellow();
				// // reset time counter
				signalCount = 0;
				// clear flag for next time
				//last_signal_was_distorted = 0x00;
				// activate PC6 (PCINT22) as external interrupt
				PCMSK2 |= 1 << PCINT22;
			}
		}
		// decide if last char was short 0,1s (zero: 0,1s/16,384ms=6,1) or long 0,2s (one: 0,2s/16,384ms=12,2)
		// low (zero) detected : 3 (0,049152s) ... 8 (0,131072s)
		else if (signalCount >= 3 && signalCount <= 8)
		{			
			// invalid signal: receiving a break
			if (estimateReceivingType == 2)
			{
				// short distortion
				current_distortion = 0x01;
				// do net reset break counter
				// switch on status led red
				switchOnStatusRed();
				// debug: 0x13 = invalid pause detected
				usart0ReceiveTransmit(0x13);

				// signal count to break
				breakCount = signalCount + breakCount;
				// reset signal counter
				signalCount = 0;

				// // singal count to break
				// breakCount = breakCount + signalCount
				// reset dcf77 receive flag
				dcfActive = 0;
				// switch off status led yellow
				switchOffStatusYellow();
				// // reset time counter
				// signalCount = 0;
				// clear flag for next time
				//last_signal_was_distorted = 0x00;
				// activate PC6 (PCINT22) as external interrupt
				PCMSK2 |= 1 << PCINT22;
			}
			// valid signal: receiving a signal or a dont-know
			else
			{
				// save detected value to array
				dcfArray[arrayCount] = 0;
				// debug: 0x00 = valid zero value detected
				usart0ReceiveTransmit(arrayCount);
				usart0ReceiveTransmit(0x00);
				// // reset break counter
				// breakCount = 0;
				// increment signal char counter
				arrayCount++;

				// next receiving is a break 
				estimateReceivingType = 2;
				usart0ReceiveTransmit(estimateReceivingType);

				// reset dcf77 receive flag
				dcfActive = 0;
				// switch off status led yellow
				switchOffStatusYellow();
				// reset time counter
				signalCount = 0;
				// clear flag for next time
				//last_signal_was_distorted = 0x00;
				// activate PC6 (PCINT22) as external interrupt
				PCMSK2 |= 1 << PCINT22;

			}
		}
		// distortion: 8 (0,131072s) ... 9 (0,147456s)
		else if (signalCount > 8 && signalCount < 9)
		{
			// short distortion
			current_distortion = 0x01;
			// switch on status led red
			switchOnStatusRed();
			// debug: 0x15 = invalid signal detected
			usart0ReceiveTransmit(0x15);

			// receiving a break
			if (estimateReceivingType == 2)
			{
				// singal count to break
				breakCount = breakCount + signalCount;

				// reset dcf77 receive flag
				dcfActive = 0;
				// switch off status led yellow
				switchOffStatusYellow();
				// reset time counter
				signalCount = 0;
				// clear flag for next time
				//last_signal_was_distorted = 0x00;
				// activate PC6 (PCINT22) as external interrupt
				PCMSK2 |= 1 << PCINT22;
			}
			// receiving a signal 
			else if (estimateReceivingType == 1)
			{
				// // singal count to break
				// breakCount = breakCount + signalCount
				// reset dcf77 receive flag
				dcfActive = 0;
				// switch off status led yellow
				switchOffStatusYellow();
				// // reset time counter
				// signalCount = 0;
				// clear flag for next time
				//last_signal_was_distorted = 0x00;
				// activate PC6 (PCINT22) as external interrupt
				PCMSK2 |= 1 << PCINT22;
			}
			// receiveing a dont-know
			else
			{
				// // singal count to break
				// breakCount = breakCount + signalCount
				// reset dcf77 receive flag
				dcfActive = 0;
				// switch off status led yellow
				switchOffStatusYellow();
				// // reset time counter
				signalCount = 0;
				// clear flag for next time
				//last_signal_was_distorted = 0x00;
				// activate PC6 (PCINT22) as external interrupt
				PCMSK2 |= 1 << PCINT22;
			}
		}
		// high (one): 9 (0,147456s) ... 14 (0,229376s)
		else if (signalCount >= 9 && signalCount <= 14)
		{
			
			// invalid signal: receiving a break
			if (estimateReceivingType == 2)
			{
				// short distortion
				current_distortion = 0x01;
				// do net reset break counter
				// switch on status led red
				switchOnStatusRed();
				// debug: 0x19 = invalid pause detected
				usart0ReceiveTransmit(0x19);
				
				// signal count to break
				breakCount = signalCount + breakCount;
				// reset signal counter
				signalCount = 0;

				// reset dcf77 receive flag
				dcfActive = 0;
				// switch off status led yellow
				switchOffStatusYellow();
				// reset time counter
				//signalCount = 0;
				// clear flag for next time
				//last_signal_was_distorted = 0x00;
				// activate PC6 (PCINT22) as external interrupt
				PCMSK2 |= 1 << PCINT22;
			}
			// valid signal: receiving a signal or a dont-know
			else
			{
				// save detected value to array
				dcfArray[arrayCount] = 1;
				// debug: 0x01 = valid one value detected
				usart0ReceiveTransmit(arrayCount);
				usart0ReceiveTransmit(0x01);
				// // reset break counter
				// breakCount = 0;
				// increment signal char counter
				arrayCount++;

				// next receiving is a break 
				estimateReceivingType = 2;
				usart0ReceiveTransmit(estimateReceivingType);

				// reset dcf77 receive flag
				dcfActive = 0;
				// switch off status led yellow
				switchOffStatusYellow();
				// reset time counter
				signalCount = 0;
				// clear flag for next time
				//last_signal_was_distorted = 0x00;
				// activate PC6 (PCINT22) as external interrupt
				PCMSK2 |= 1 << PCINT22;

			}
		}
		// long dingal > 2 secs: abort
		else if (signalCount >= 125)
		{
			// long distortion
			current_distortion = 0x01;
			// switch on status led red
			switchOnStatusRed();
			// debug: 0x1F = invalid signal detected
			usart0ReceiveTransmit(0x1F);
			//usart0ReceiveTransmit(arrayCount);

			// next receiving is a dont-know 
			estimateReceivingType = 0;

			// switch off status led yellow
			switchOffStatusYellow();

			// reset break counter
			breakCount = 0;
			// reset dcf77 receive flag
			dcfActive = 0;
			// reset time counter
			signalCount = 0;
			// clear flag for next time
			//last_signal_was_distorted = 0x00;
			// activate PC6 (PCINT22) as external interrupt
			PCMSK2 |= 1 << PCINT22;

			return;
		}
		// signal distortion - failure on receiving signal
		else
		{
			// long distortion
			current_distortion = 0x01;
			// switch on status led red
			switchOnStatusRed();
			// debug: 0x1B = invalid signal detected
			usart0ReceiveTransmit(0x1B);
			//usart0ReceiveTransmit(arrayCount);

			// receiving a break
			if (estimateReceivingType == 2)
			{
				// singal count to break
				breakCount = breakCount + signalCount;

				// reset dcf77 receive flag
				dcfActive = 0;
				// switch off status led yellow
				switchOffStatusYellow();
				// reset time counter
				signalCount = 0;
				// clear flag for next time
				//last_signal_was_distorted = 0x00;
				// activate PC6 (PCINT22) as external interrupt
				PCMSK2 |= 1 << PCINT22;
			}
			// receiving a signal 
			else if(estimateReceivingType == 1)
			{
				// // singal count to break
				// breakCount = breakCount + signalCount
				// reset dcf77 receive flag
				dcfActive = 0;
				// switch off status led yellow
				switchOffStatusYellow();
				// // reset time counter
				// signalCount = 0;
				// clear flag for next time
				//last_signal_was_distorted = 0x00;
				// activate PC6 (PCINT22) as external interrupt
				PCMSK2 |= 1 << PCINT22;
			}
			// a dont-know
			else
			{
				// // singal count to break
				// breakCount = breakCount + signalCount
				// reset dcf77 receive flag
				dcfActive = 0;
				// switch off status led yellow
				switchOffStatusYellow();
				// // reset time counter
				signalCount = 0;
				// clear flag for next time
				//last_signal_was_distorted = 0x00;
				// activate PC6 (PCINT22) as external interrupt
				PCMSK2 |= 1 << PCINT22;
			}
		}
	}
}