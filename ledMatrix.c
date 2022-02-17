/*******************************************************************************
*
*	Author:			Georg Bauer (geo_bauer@gmx.net)
*	Date:			06.01.2014
*
*	Project-Title:	ClockWise
*	Description:	µC controlled clock with DCF77, RTC and a led-matrix
*
*	File-Title:		LED Matrix 12x12 LEDs
*
*******************************************************************************
*
* Matrix is divided into 12 rows from 0 (first - top) to 11 (last - bottom) and
* into 12 columns from 0 (first - left) to 11 (last - right).
* Every row information is dividet into two bytes
*	uint8_t high;	// information from 1st (left) to 8th led
*	uint8_t low;	// information from 9th to 12th led last (right)
* The last 4 bits of low byte are empty. If these bits are not empty, the led
* matrix will display a fail function!
*
*******************************************************************************
*
* Pin Declaration:
*	Pin						| Description
*	------------------------|-------------------------------------------------
*	PD1 (Pin 15) as output	| Reset signal (RSTREG)
*	PD2 (Pin 16) as input	| RXD USART 1 for LED Matrix
*	PD3 (Pin 17) as output	| TXD USART 1 for LED Matrix
*	PD4 (Pin 18) as output	| XCK USART 1 for LED Matrix
*	PD5 (Pin 19) as output	| inverted LED enable (!LEDEN)
*	PD6 (Pin 20) as output	| Load signal (LOAD)
*	------------------------|-------------------------------------------------
*
*******************************************************************************
*
* Timer:
*	Timer 2 used for display information on led matrix (fast pwm mode)
*
* Interrupts:
*	Timer 2 interrupt service routine is every second active
*
*******************************************************************************
*/

//! Libraries
#include "ledMatrix.h"
#include "usart.h"
#include "system.h"
#include "gpios.h"
#include "settings.h"
#include <util/delay.h>

//! Own global variables
volatile struct row actualMatrix[12];
volatile uint8_t actualRow;
volatile uint8_t acutalDot;
//! Other global variables
extern volatile struct systemParameter systemConfig;
extern volatile struct time systemTime;

//! Initialize matrix
void initMatrix(void)
{
	uint8_t i = 0;

	// Initialize uart as spi
	initUsart();
	
	// set standard values
	actualRow = 12;
	acutalDot = 0;
	
	//! timer for regulate information in display rows
	// 8 bit timer/counter 2
	// Set OC2A on Compare Match, set OC2A at match
	// counting from BOTTOM = 0x00 to TOP = 0xFF
	TCCR2A = (1 << COM2A1) | (1 << COM2A0) | (1 << WGM21) | (1 << WGM20);
	// clock select: 64 prescale -> 128µs (7,8125kHz)
	TCCR2B = (1 << CS22);
	// set compare value for pwm
	OCR2A = systemConfig.displayBrightness;
	
	// enable timer/counter 2 interrupt overflow
	// enable output compare match a interrupt
	// !LEDEN will toggle automatic
	TIMSK2 |= (1 << OCIE2A) | (1 << TOIE2);
	
	//! output: Port D
	// activate output !LEDEN (PD5), LOAD (PD6) and LEDRESET (PD1)
	DDRD |= (1 << PD1) |(1 << PD6) | (1 << PD5);
	// switch led matrix off (PD5 = !LEDEN = 1)
	PORTD |= (1 << PD5);
	// clear load signal
	PORTD &= ~(1 << PD6);
	// set reset port (logical one is NO reset)
	PORTD |= (1 << PD1);
		
	// initialize actual output
	for(i = 0; i<12; i++)
	{
		actualMatrix[i].high = 0b10000000;
		actualMatrix[i].low	 = 0b00010000;
	}

	actualMatrix[0].high	= 0b11111111;
	actualMatrix[0].low		= 0b11110000;
	actualMatrix[11].high	= 0b11111111;
	actualMatrix[11].low	= 0b11110000;

	// default values
	sendMatrixToShiftRegister(0);
}

//! send signal of row to matrix
void sendMatrixToShiftRegister(uint8_t row)
{
	// variable for masking
	uint8_t rowMaskHigh = 0;
	uint8_t rowMaskLow	= 0;
	
	// define masks
	switch(actualRow)
	{
		case 0:
			rowMaskHigh = 0b00000111;
			rowMaskLow  = 0b11111111;
			break;
		case 1:
			rowMaskHigh = 0b00001011;
			rowMaskLow  = 0b11111111;
			break;
		case 2:
			rowMaskHigh = 0b00001101;
			rowMaskLow  = 0b11111111;
			break;
		case 3:
			rowMaskHigh = 0b00001110;
			rowMaskLow  = 0b11111111;
			break;
		case 4:
			rowMaskHigh = 0b00001111;
			rowMaskLow  = 0b01111111;
			break;
		case 5:
			rowMaskHigh = 0b00001111;
			rowMaskLow  = 0b10111111;
			break;
		case 6:
			rowMaskHigh = 0b00001111;
			rowMaskLow  = 0b11011111;
			break;
		case 7:
			rowMaskHigh = 0b00001111;
			rowMaskLow  = 0b11101111;
			break;
		case 8:
			rowMaskHigh = 0b00001111;
			rowMaskLow  = 0b11110111;
			break;
		case 9:
			rowMaskHigh = 0b00001111;
			rowMaskLow  = 0b11111011;
			break;
		case 10:
			rowMaskHigh = 0b00001111;
			rowMaskLow  = 0b11111101;
			break;
		case 11:
			rowMaskHigh = 0b00001111;
			rowMaskLow  = 0b11111110;
			break;
		default:
			break;
	}

	// send new values
	usartReceiveTransmit(actualMatrix[row].high);
	//_delay_us(DELAYSPI);
	usartReceiveTransmit(actualMatrix[row].low | rowMaskHigh); // only an OR operation
	//_delay_us(DELAYSPI);
	usartReceiveTransmit(rowMaskLow);
}

//! take send values to register
void loadMatrixShiftRegister(void)
{
	// Load signal rises to logical one
	PORTD |= (1 << PD6);
	
	// wait a little bit
	_delay_us(DELAYLOAD);
	
	// Load signal fall back to down
	PORTD &= ~(1 << PD6);
}


//! enable led matrix (switch on)
void enableMatrix(void)
{
	// switch led matrix on (PD5 = !LEDEN = 0)
	PORTD &= ~(1 << PD5);
}

// disable led matrix (switch off)
void disableMatrix(void)
{
	// switch led matrix off (PD5 = !LEDEN = 1)
	PORTD |= (1 << PD5);
}

// reset shift register of led matrix
void resetMatrixShiftRegister(void)
{
	// Reset signal fall down to logical zero
	PORTD &= ~(1 << PD1);
		
	// wait a litte bit
	_delay_us(DELAYRSTREG);
		
	// Reset signal comes back to logical one
	PORTD |= (1 << PD1);
}

//! Interrupt Service Routine when Timer/Counter 2 has an overflow
// this routine will called every 128µs (7,8125kHz)
// calculated by: 16MHz /(2^8 [8bit counter] * 8 [main clock divider] * 8 [timer 2 clock divider] = 7,8125kHz
// calculated by: 16MHz /(2^8 [8bit counter] * 64 [timer 2 clock divider] = 7,8125kHz
ISR(TIMER2_OVF_vect)
{
	// enable led matrix (switch on)
	enableMatrix();
	
	// actualize led matrix
	loadMatrixShiftRegister();
	
	// increment and reset row counter
	actualRow++;
	if (actualRow == 12)
	{
		actualRow = 0;
	}
					
	// switch dot and char leds on
	switch(actualRow)
	{
		case 0:
			if (acutalDot & 0x02)
				switchOnDot1();
			break;
		case 1:
			if (acutalDot & 0x04)
				switchOnDot2();
			break;
		case 2:
			if (acutalDot & 0x08)
				switchOnDot3();
			break;
		case 3:
			if (acutalDot & 0x10)
				switchOnDot4();
			break;
		case 4:
			switchOnChar();
			break;
		default:
			break;
	}
	
	// reset register
	resetMatrixShiftRegister();
	
	// send new value
	sendMatrixToShiftRegister(actualRow);
}

//! Interrupt Service Routine when Timer/Counter 2 has an correct compare
ISR(TIMER2_COMPA_vect)
{
	// disable led matrix (switch off)
	disableMatrix();
	
	// switch dot and char leds off
	switch(actualRow)
	{
		case 0:
			switchOffDot1();
			break;
		case 1:
			switchOffDot2();
			break;
		case 2:
			switchOffDot3();
			break;
		case 3:
			switchOffDot4();
			break;
		case 4:
			switchOffChar();
			break;
		default:
			break;
	}
	
	// set new compare value for pwm
	OCR2A = systemConfig.displayBrightness;
}

// set matrix to total darkness
void setMatrixDark()
{
	uint8_t i = 0;
	// initialize all values with zero
	for(i = 0; i<12; i++)
	{
		actualMatrix[i].high = 0b00000000;
		actualMatrix[i].low	 = 0b00000000;
	}
}

// set matrix to total brightness
void setMatrixBright()
{
	uint8_t i = 0;
	// initialize all values with zero
	for(i = 0; i<12; i++)
	{
		actualMatrix[i].high = 0b11111111;
		actualMatrix[i].low	 = 0b11110000;
	}
}


// actualize 'actualMatrix' Register with system time
void actualizeMatrixWithSystemTime()
{
		uint8_t actualHour	= 0;
		int8_t add			= 0;

		// check straight pie (0) or shift pie (1)
		if(0x01 & systemConfig.displaySetting)
		{
			add = -2;
		}
		
		// it is exactly full hour, half and quarter past, quarter to
		if ((systemTime.minute >= ((60 + add) % 60) && systemTime.minute < (5 + add))  ||
			(systemTime.minute >= (15 + add) && systemTime.minute < (20 + add)) ||
			(systemTime.minute >= (30 + add) && systemTime.minute < (35 + add)) ||
			(systemTime.minute >= (45 + add) && systemTime.minute < (50 + add)))
		{
			actualMatrix[0].high	= WORD_ROW00_S_H | WORD_ROW00_IS_H | WORD_ROW00_GRAD_H;
			actualMatrix[0].low		= WORD_ROW00_S_L | WORD_ROW00_IS_L | WORD_ROW00_GRAD_L;
		}
		else
		{
			actualMatrix[0].high	= WORD_ROW00_S_H | WORD_ROW00_IS_H;
			actualMatrix[0].low		= WORD_ROW00_S_L | WORD_ROW00_IS_L;
		}
				
		// past
		if ((systemTime.minute >= (5 + add) && systemTime.minute < (10 + add))   ||
			(systemTime.minute >= (20 + add) && systemTime.minute < (25 + add)) ||
			(systemTime.minute >= (35 + add) && systemTime.minute < (40 + add)) ||
			(systemTime.minute >= (50 + add) && systemTime.minute < (55 + add)))
		{
			actualMatrix[1].high	= WORD_ROW01_KORZ_H | WORD_ROW01_NOCH_H;
			actualMatrix[1].low		= WORD_ROW01_KORZ_L | WORD_ROW01_NOCH_L;
		}
		else
		{
			actualMatrix[1].high	= 0b00000000;
			actualMatrix[1].low		= 0b00000000;
		}
		
		// to
		if ((systemTime.minute >= (10 + add) && systemTime.minute < (15 + add))  ||
			(systemTime.minute >= (40 + add) && systemTime.minute < (45 + add)) ||
			(systemTime.minute >= (55 + add) && systemTime.minute < (60 + add)))
		{
			actualMatrix[2].high	= WORD_ROW02_GLEI_H;
			actualMatrix[2].low		= WORD_ROW02_GLEI_L;
		}
		else
		{
			actualMatrix[2].high	= 0b00000000;
			actualMatrix[2].low		= 0b00000000;
		}
		
		// to and half
		if (systemTime.minute >= (25 + add) && systemTime.minute < (30 + add))
		{
			actualMatrix[2].high	= WORD_ROW02_GLEI_H | WORD_ROW02_HALWA_H;
			actualMatrix[2].low		= WORD_ROW02_GLEI_L | WORD_ROW02_HALWA_L;
		}
		
		// half
		if (systemTime.minute >= (30 + add) && systemTime.minute < (40 + add))
		{
			actualMatrix[2].high	= WORD_ROW02_HALWA_H;
			actualMatrix[2].low		= WORD_ROW02_HALWA_L;
		}
				
		// quarter past
		if (systemTime.minute >= (10 + add) && systemTime.minute < (25 + add))
		{
			actualMatrix[3].high	= WORD_ROW03_VAEDDL_H;
			actualMatrix[3].low		= WORD_ROW03_VAEDDL_L;
		}
		else
		{
			actualMatrix[3].high	= 0b00000000;
			actualMatrix[3].low		= 0b00000000;
		}
		
		// quarter to
		if (systemTime.minute >= (40 + add) && systemTime.minute < (55 + add))
		{
			actualMatrix[3].high	= WORD_ROW03_DREI_H | WORD_ROW03_VAEDDL_H;
			actualMatrix[3].low		= WORD_ROW03_DREI_L | WORD_ROW03_VAEDDL_L;
		}
		
		// no birthday
			actualMatrix[4].high	= 0;
			actualMatrix[4].low		= 0;
		
		// calculate display hour
		if (systemTime.minute >= (10 + add))
		{
			actualHour = systemTime.hour + 1;
		}
		else
		{
			actualHour = systemTime.hour;
		}
		
		// hour
		switch(actualHour)
		{
			case 0: case 12: case 24:
				// midnight, twelve
				actualMatrix[5].high	= 0b00000000;
				actualMatrix[5].low		= 0b00000000;
				actualMatrix[6].high	= WORD_ROW06_ZWOELFE_H;
				actualMatrix[6].low		= WORD_ROW06_ZWOELFE_L;
				actualMatrix[7].high	= 0b00000000;
				actualMatrix[7].low		= 0b00000000;
				actualMatrix[8].high	= 0b00000000;
				actualMatrix[8].low		= 0b00000000;
				actualMatrix[9].high	= 0b00000000;
				actualMatrix[9].low		= 0b00000000;
				actualMatrix[10].high	= 0b00000000;
				actualMatrix[10].low	= 0b00000000;
				actualMatrix[11].high	= 0b00000000;
				actualMatrix[11].low	= 0b00000000;
				break;
			case 1: case 13:
				// one, thirteen
				actualMatrix[5].high	= WORD_ROW05_OHNSE_H;
				actualMatrix[5].low		= WORD_ROW05_OHNSE_L;
				actualMatrix[6].high	= 0b00000000;
				actualMatrix[6].low		= 0b00000000;
				actualMatrix[7].high	= 0b00000000;
				actualMatrix[7].low		= 0b00000000;
				actualMatrix[8].high	= 0b00000000;
				actualMatrix[8].low		= 0b00000000;
				actualMatrix[9].high	= 0b00000000;
				actualMatrix[9].low		= 0b00000000;
				actualMatrix[10].high	= 0b00000000;
				actualMatrix[10].low	= 0b00000000;
				actualMatrix[11].high	= 0b00000000;
				actualMatrix[11].low	= 0b00000000;
				break;
			case 2: case 14:
				// two, fourteen
				actualMatrix[5].high	= 0b00000000;
				actualMatrix[5].low		= 0b00000000;
				actualMatrix[6].high	= 0b00000000;
				actualMatrix[6].low		= 0b00000000;
				actualMatrix[7].high	= 0b00000000;
				actualMatrix[7].low		= 0b00000000;
				actualMatrix[8].high	= 0b00000000;
				actualMatrix[8].low		= 0b00000000;
				actualMatrix[9].high	= 0b00000000;
				actualMatrix[9].low		= 0b00000000;
				actualMatrix[10].high	= 0b00000000;
				actualMatrix[10].low	= 0b00000000;
				actualMatrix[11].high	= WORD_ROW11_ZWEH_H;
				actualMatrix[11].low	= WORD_ROW11_ZWEH_L;
				break;
			case 3: case 15:
				// three, fifteen
				actualMatrix[5].high	= 0b00000000;
				actualMatrix[5].low		= 0b00000000;
				actualMatrix[6].high	= 0b00000000;
				actualMatrix[6].low		= 0b00000000;
				actualMatrix[7].high	= 0b00000000;
				actualMatrix[7].low		= 0b00000000;
				actualMatrix[8].high	= 0b00000000;
				actualMatrix[8].low		= 0b00000000;
				actualMatrix[9].high	= WORD_ROW09_DREI_H;
				actualMatrix[9].low		= WORD_ROW09_DREI_L;
				actualMatrix[10].high	= 0b00000000;
				actualMatrix[10].low	= 0b00000000;
				actualMatrix[11].high	= 0b00000000;
				actualMatrix[11].low	= 0b00000000;
				break;
			case 4: case 16:
				// four, sixteen
				actualMatrix[5].high	= 0b00000000;
				actualMatrix[5].low		= 0b00000000;
				actualMatrix[6].high	= 0b00000000;
				actualMatrix[6].low		= 0b00000000;
				actualMatrix[7].high	= WORD_ROW07_VIERE_H;
				actualMatrix[7].low		= WORD_ROW07_VIERE_L;
				actualMatrix[8].high	= 0b00000000;
				actualMatrix[8].low		= 0b00000000;
				actualMatrix[9].high	= 0b00000000;
				actualMatrix[9].low		= 0b00000000;
				actualMatrix[10].high	= 0b00000000;
				actualMatrix[10].low	= 0b00000000;
				actualMatrix[11].high	= 0b00000000;
				actualMatrix[11].low	= 0b00000000;
				break;
			case 5: case 17:
				// fife, seventeen
				actualMatrix[5].high	= WORD_ROW05_FUENFE_H;
				actualMatrix[5].low		= WORD_ROW05_FUENFE_L;
				actualMatrix[6].high	= 0b00000000;
				actualMatrix[6].low		= 0b00000000;
				actualMatrix[7].high	= 0b00000000;
				actualMatrix[7].low		= 0b00000000;
				actualMatrix[8].high	= 0b00000000;
				actualMatrix[8].low		= 0b00000000;
				actualMatrix[9].high	= 0b00000000;
				actualMatrix[9].low		= 0b00000000;
				actualMatrix[10].high	= 0b00000000;
				actualMatrix[10].low	= 0b00000000;
				actualMatrix[11].high	= 0b00000000;
				actualMatrix[11].low	= 0b00000000;
				break;
			case 6: case 18:
				// six, eighteen
				actualMatrix[5].high	= 0b00000000;
				actualMatrix[5].low		= 0b00000000;
				actualMatrix[6].high	= 0b00000000;
				actualMatrix[6].low		= 0b00000000;
				actualMatrix[7].high	= 0b00000000;
				actualMatrix[7].low		= 0b00000000;
				actualMatrix[8].high	= WORD_ROW08_SECHSE_H;
				actualMatrix[8].low		= WORD_ROW08_SECHSE_L;
				actualMatrix[9].high	= 0b00000000;
				actualMatrix[9].low		= 0b00000000;
				actualMatrix[10].high	= 0b00000000;
				actualMatrix[10].low	= 0b00000000;
				actualMatrix[11].high	= 0b00000000;
				actualMatrix[11].low	= 0b00000000;
				break;
			case 7: case 19:
				// seven, nineteen
				actualMatrix[5].high	= 0b00000000;
				actualMatrix[5].low		= 0b00000000;
				actualMatrix[6].high	= 0b00000000;
				actualMatrix[6].low		= 0b00000000;
				actualMatrix[7].high	= 0b00000000;
				actualMatrix[7].low		= 0b00000000;
				actualMatrix[8].high	= 0b00000000;
				actualMatrix[8].low		= 0b00000000;
				actualMatrix[9].high	= WORD_ROW09_SIWWENE_H;
				actualMatrix[9].low		= WORD_ROW09_SIWWENE_L;
				actualMatrix[10].high	= 0b00000000;
				actualMatrix[10].low	= 0b00000000;
				actualMatrix[11].high	= 0b00000000;
				actualMatrix[11].low	= 0b00000000;
				break;
			case 8: case 20:
				// eight, twenty
				actualMatrix[5].high	= 0b00000000;
				actualMatrix[5].low		= 0b00000000;
				actualMatrix[6].high	= 0b00000000;
				actualMatrix[6].low		= 0b00000000;
				actualMatrix[7].high	= 0b00000000;
				actualMatrix[7].low		= 0b00000000;
				actualMatrix[8].high	= 0b00000000;
				actualMatrix[8].low		= 0b00000000;
				actualMatrix[9].high	= 0b00000000;
				actualMatrix[9].low		= 0b00000000;
				actualMatrix[10].high	= WORD_ROW10_ACHDE_H;
				actualMatrix[10].low	= WORD_ROW10_ACHDE_L;
				actualMatrix[11].high	= 0b00000000;
				actualMatrix[11].low	= 0b00000000;
				break;
			case 9: case 21:
				// nine, twenty-one
				actualMatrix[5].high	= 0b00000000;
				actualMatrix[5].low		= 0b00000000;
				actualMatrix[6].high	= 0b00000000;
				actualMatrix[6].low		= 0b00000000;
				actualMatrix[7].high	= 0b00000000;
				actualMatrix[7].low		= 0b00000000;
				actualMatrix[8].high	= 0b00000000;
				actualMatrix[8].low		= 0b00000000;
				actualMatrix[9].high	= 0b00000000;
				actualMatrix[9].low		= 0b00000000;
				actualMatrix[10].high	= 0b00000000;
				actualMatrix[10].low	= 0b00000000;
				actualMatrix[11].high	= WORD_ROW11_NEUNE_H;
				actualMatrix[11].low	= WORD_ROW11_NEUNE_L;
				break;
			case 10: case 22:
				// ten, twenty-two
				actualMatrix[5].high	= 0b00000000;
				actualMatrix[5].low		= 0b00000000;
				actualMatrix[6].high	= 0b00000000;
				actualMatrix[6].low		= 0b00000000;
				actualMatrix[7].high	= 0b00000000;
				actualMatrix[7].low		= 0b00000000;
				actualMatrix[8].high	= 0b00000000;
				actualMatrix[8].low		= 0b00000000;
				actualMatrix[9].high	= 0b00000000;
				actualMatrix[9].low		= 0b00000000;
				actualMatrix[10].high	= 0b00000000;
				actualMatrix[10].low	= 0b00000000;
				actualMatrix[11].high	= WORD_ROW11_ZEHNE_H;
				actualMatrix[11].low	= WORD_ROW11_ZEHNE_L;
				break;
			case 11: case 23:
				// eleven, twenty-three
				actualMatrix[5].high	= 0b00000000;
				actualMatrix[5].low		= 0b00000000;
				actualMatrix[6].high	= 0b00000000;
				actualMatrix[6].low		= 0b00000000;
				actualMatrix[7].high	= 0b00000000;
				actualMatrix[7].low		= 0b00000000;
				actualMatrix[8].high	= WORD_ROW08_ELFE_H;
				actualMatrix[8].low		= WORD_ROW08_ELFE_L;
				actualMatrix[9].high	= 0b00000000;
				actualMatrix[9].low		= 0b00000000;
				actualMatrix[10].high	= 0b00000000;
				actualMatrix[10].low	= 0b00000000;
				actualMatrix[11].high	= 0b00000000;
				actualMatrix[11].low	= 0b00000000;
				break;
			default:
				actualMatrix[5].high	= 0b00000000;
				actualMatrix[5].low		= 0b00000000;
				actualMatrix[6].high	= 0b00000000;
				actualMatrix[6].low		= 0b00000000;
				actualMatrix[7].high	= 0b00000000;
				actualMatrix[7].low		= 0b00000000;
				actualMatrix[8].high	= 0b00000000;
				actualMatrix[8].low		= 0b00000000;
				actualMatrix[9].high	= 0b00000000;
				actualMatrix[9].low		= 0b00000000;
				actualMatrix[10].high	= 0b00000000;
				actualMatrix[10].low	= 0b00000000;
				actualMatrix[11].high	= 0b00000000;
				actualMatrix[11].low	= 0b00000000;
				break;
		}
		
		//! special cases
		// time to feed horses
		if (((actualHour ==  18) && (systemTime.minute >= 53) && (systemTime.minute < 8)) ||
			((actualHour ==  8) && (systemTime.minute >= 53) && (systemTime.minute < 8)))
		{
				actualMatrix[0].high	= 0b00000000;
				actualMatrix[0].low		= 0b00000000;
				actualMatrix[1].high	= WORD_ROW01_ZEID_H;
				actualMatrix[1].low		= WORD_ROW01_ZEID_L;
				actualMatrix[2].high	= WORD_ROW02_ZEID_H;
				actualMatrix[2].low		= WORD_ROW02_ZEID_L;
				actualMatrix[3].high	= WORD_ROW03_ZEID_H;
				actualMatrix[3].low		= WORD_ROW03_ZEID_L;
				actualMatrix[4].high	= WORD_ROW04_ZEID_H | WORD_ROW04_ZUM_H;
				actualMatrix[4].low		= WORD_ROW04_ZEID_L | WORD_ROW04_ZUM_L;	
				actualMatrix[5].high	= 0b00000000;
				actualMatrix[5].low		= 0b00000000;
				actualMatrix[6].high	= WORD_ROW06_FIEDAN_H;
				actualMatrix[6].low		= WORD_ROW06_FIEDAN_L;
				actualMatrix[7].high	= WORD_ROW07_FIEDAN_H;
				actualMatrix[7].low		= WORD_ROW07_FIEDAN_L;
				actualMatrix[8].high	= WORD_ROW08_FIEDAN_H;
				actualMatrix[8].low		= WORD_ROW08_FIEDAN_L;
				actualMatrix[9].high	= WORD_ROW09_FIEDAN_H;
				actualMatrix[9].low		= WORD_ROW09_FIEDAN_L;
				actualMatrix[10].high	= WORD_ROW10_FIEDAN_H;
				actualMatrix[10].low	= WORD_ROW10_FIEDAN_L;
				actualMatrix[11].high	= WORD_ROW11_FIEDAN_H;
				actualMatrix[11].low	= WORD_ROW11_FIEDAN_L;
		}
		
		// birthday time
		if ((systemTime.day == 1 && systemTime.month == 1) ||
			(systemTime.day == 16 && systemTime.month == 1))
		{
			actualMatrix[0].high	= WORD_ROW00_GEBODSDAG_H;
			actualMatrix[0].low		= WORD_ROW00_GEBODSDAG_L;
			actualMatrix[1].high	= WORD_ROW01_GEBODSDAG_H;
			actualMatrix[1].low		= WORD_ROW01_GEBODSDAG_L;
			actualMatrix[2].high	= WORD_ROW02_GEBODSDAG_H;
			actualMatrix[2].low		= WORD_ROW02_GEBODSDAG_L;
			actualMatrix[3].high	= WORD_ROW03_GEBODSDAG_H;
			actualMatrix[3].low		= WORD_ROW03_GEBODSDAG_L;
			actualMatrix[4].high	= WORD_ROW04_GEBODSDAG_H | WORD_ROW04_GUDE_H | WORD_ROW04_ZUM_H;
			actualMatrix[4].low		= WORD_ROW04_GEBODSDAG_L | WORD_ROW04_GUDE_L | WORD_ROW04_ZUM_L;
			actualMatrix[5].high	= WORD_ROW05_GEBODSDAG_H | WORD_ROW05_ALLES_H;
			actualMatrix[5].low		= WORD_ROW05_GEBODSDAG_L | WORD_ROW05_ALLES_L;
			actualMatrix[6].high	= WORD_ROW06_GEBODSDAG_H | WORD_ROW06_ALLES_H;
			actualMatrix[6].low		= WORD_ROW06_GEBODSDAG_L | WORD_ROW06_ALLES_L;
			actualMatrix[7].high	= WORD_ROW07_GEBODSDAG_H | WORD_ROW07_ALLES_H;
			actualMatrix[7].low		= WORD_ROW07_GEBODSDAG_L | WORD_ROW07_ALLES_L;
			actualMatrix[8].high	= WORD_ROW08_GEBODSDAG_H | WORD_ROW08_ALLES_H;
			actualMatrix[8].low		= WORD_ROW08_GEBODSDAG_L | WORD_ROW08_ALLES_L;
			actualMatrix[9].high	= WORD_ROW09_ALLES_H;
			actualMatrix[9].low		= WORD_ROW09_ALLES_L;
			actualMatrix[10].high	= 0b00000000;
			actualMatrix[10].low	= 0b00000000;
			actualMatrix[11].high	= 0b00000000;
			actualMatrix[11].low	= 0b00000000;
		}
/*		
		// blink blink blink -- only for testing!!!! binary date time
		actualMatrix[5].high	= systemTime.hour;
		actualMatrix[5].low		= 0;
		actualMatrix[6].high	= systemTime.minute;
		actualMatrix[6].low		= 0;
		actualMatrix[7].high	= systemTime.second;
		actualMatrix[7].low		= 0;
		actualMatrix[8].high	= systemTime.year;
		actualMatrix[8].low		= 0;
		actualMatrix[9].high	= systemTime.month;
		actualMatrix[9].low		= 0;
		actualMatrix[10].high	= systemTime.day;
		actualMatrix[10].low	= 0;
		actualMatrix[11].high	= systemTime.weekday;
		actualMatrix[11].low	= 0;
		*/
}
