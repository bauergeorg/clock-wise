/*******************************************************************************
*
*	Author:			Georg Bauer
*	Date:			27.04.2014
*
*	Project-Title:	ClockWise
*	Description:	Analog-Digital-Conversion
*
*	File-Title:		ADC
*
*******************************************************************************
*
* Pin Declaration:
*	Pin						| Description
*	------------------------|-------------------------------------------------
*	PA0 (Pin 40) as input	| ADC0 for brightness VDR with 500R pullup resistor
*	PA1 (Pin 39) as input	| ADC1 for analog regulation 10kR trimmer
*	------------------------|-------------------------------------------------
*
*******************************************************************************
*/

//! Libraries
#include "adc.h"

//! Initialize ADC
void initAdc(void)
{
	// use AVCC with exterenal capacitor at ARER pin (REFS0 = 1)
	// result is reft adjust (ADLAR = 1)
	ADMUX = (1 << REFS0) | (1 << ADLAR);
		
	// CLock prescaler of 128 (16MHz/8/128) = 15,625kHz
	ADCSRA = (1 << ADPS2)  | (1 << ADPS1) | (1 << ADPS0);
	
	// ADC enable (ADEN = 1)
	ADCSRA |= (1 << ADEN);
	
	// after active the adc, we need a dummy-readout
	// to "warmup" the adc
	(void) adcRead(0);
}

//! Read ADC value
// input value is channel number
// output value is sampled value
uint8_t adcRead(uint8_t channelSelection)
{
	// select the corresponding channel 0~7
	// ANDing with '7' will always keep the value
	// of 'channelSelection' between 0 and 7
	channelSelection &= 0b00000111;  // AND operation with 7
	// clears the bottom 3 bits before ORing
	ADMUX = (ADMUX & 0xF8) | channelSelection;
		
	// start single conversion
	// write '1' to ADSC
	ADCSRA |= (1 << ADSC);
		
	// wait for conversion to complete
	// ADSC becomes '0' again
	// till then, run loop continuously
	while(ADCSRA & (1 << ADSC));
	{
		// wait for value...
	}

	// return only 8 upper bits of 10 sampled bits
	return ADCH;
}