/*******************************************************************************
*
*	Author:			Georg Bauer
*	Date:			06.01.2014
*
*	Project-Title:	ClockWise
*	Description:	Status LEDs, Switches and other LED outputs (char and dots)
*
*	File-Title:		General Purpose Input/Outputs
*
*******************************************************************************
*
* Pin Declaration:
*	Pin						| Description
*	------------------------|-------------------------------------------------
*	PA2 (Pin 38) as input	| Switch 1 - external interrupt PCINT2
*	PA3 (Pin 37) as input	| Switch 2 - external interrupt PCINT3
*	PA4 (Pin 36) as input	| Switch 3 - external interrupt PCINT4
*	PA5 (Pin 35) as input	| Switch 4 - external interrupt PCINT5
*	------------------------|-------------------------------------------------
*	PB0 (Pin 1) as output	| Dot 1 LED
*	PB1 (Pin 2) as output	| Dot 2 LED
*	PB2 (Pin 3)	as output	| Dot 3 LED
*	PB3 (Pin 4) as output	| Dot 4 LED
*	PB4 (Pin 5) as output	| Character LED's
*	------------------------|-------------------------------------------------
*	PB5 (Pin 6) as output	| Status 1 LED - green
*	PB6 (Pin 7) as output	| Status 2 LED - yellow
*	PB7 (Pin 8)	as output	| Status 3 LED - red
*	------------------------|-------------------------------------------------
*
*******************************************************************************
*/

//! Libraries
#include "gpios.h"
#include "system.h"
#include "menu.h"

/*
// Own global variables
volatile uint8_t i = 0;
volatile uint8_t i_max = 85;
volatile uint8_t pwm = 1;
volatile uint8_t updown = 1; // up = 1, down = 0
// definition of the pause
const double DELAY = 100;  // 10µs
*/

//! Extern global variables
extern volatile struct systemParameter systemConfig;

//! Initialize input and output ports
void initGpios(void)
{
	//! Output: Port B
	// activate complete port b as output
	DDRB |= (1 << PB7) | (1 << PB6) | (1 << PB5) | (1 << PB4) | (1 << PB3) | (1 << PB2) | (1 << PB1) | (1 << PB0);
	// switch status leds on
	PORTB |= (1 << PB7) | (1 << PB6) |(1 << PB5);
	// switch matrix led for dot's and character off
	PORTB &= ~((1 << PB4) |(1 << PB3) | (1 << PB2) | (1 << PB1) | (1 << PB0));

	//! Input: Port A
	// activate PA2 to PA5 as input 
	DDRA &= ~((1 << PA5) | (1 << PA4) | (1 << PA3) | (1 << PA2));
	// disable pull-up resistor on input switches
	PORTA &= ~((1 << PA5) | (1 << PA4) | (1 << PA3) | (1 << PA2));

	//! external interrupt for switch 1, 2, 3 and 4
	// enabled external pin change interrupts PCINT0:7
	PCICR |= 1 << PCIE0;
	// activate PA2 (PCINT2), PA3 (PCINT3), PA4 (PCINT4), PA5 (PCINT5) as external interrupt
	PCMSK0 |= (1 << PCINT2) | (1 << PCINT3) | (1 << PCINT4) | (1 << PCINT5);
	// delete flag for interrupts PCINT0:7
	PCIFR |= 1 << PCIF0;
}

//! Interrupt Service Routine for switch 1, 2, 3 and 4
// is called when a switch is pressed in and pressed out
ISR(PCINT0_vect)
{
	// actual values for switches
	uint8_t switches;

	// get value of switch 1, 2, 3 and 4 - masking with 0011.1100b and shift to right
	// alternative text: (PINA & ((1<<PINA2) | (1<<PINA3) | (1<<PINA4) | (1<<PINA5)) >> 2; 
	switches = (PINA & 0x3C) >> 2;

	// any thing else is pressed in the menu mode
	if(systemConfig.displayStatus >= DISPLAY_STATE_MENU_WAIT)
	{
		// call menu management function
		menuMgnt(switches);
	}

	// cancel and ok is pressed at the same time in standard mode
	if((switches & 0x08) && (switches & 0x01) && (systemConfig.displayStatus <= DISPLAY_STATE_MENU_WAIT))
	{
		// set new display status: show version
		systemConfig.displayStatus = DISPLAY_STATE_MENU_WAIT;
	}

	// delete flag for interrupts PCINT0:7
	PCIFR |= 1 << PCIF0;
}


//! toggle status led 1 - green
void toggleStatusGreen(void)
{
	// toggle
	PORTB ^= (1 << PB5);	
}

//! toggle status led 2 - yellow
void toggleStatusYellow(void)
{
	// toggle
	PORTB ^= (1 << PB6);
}

//! toggle status led 3 - red
void toggleStatusRed(void)
{
	// toggle
	PORTB ^= (1 << PB7);
}

//! switch on status led 1 - green
void switchOnStatusGreen(void)
{
	// switch on
	PORTB |= (1 << PB5);
}

//! switch on  status led 2 - yellow
void switchOnStatusYellow(void)
{
	// switch on
	PORTB |= (1 << PB6);
}

//! switch on  status led 3 - red
void switchOnStatusRed(void)
{
	// switch on
	PORTB |= (1 << PB7);
}

//! switch off status led 1 - green
void switchOffStatusGreen(void)
{
	// switch off
	PORTB &= ~(1 << PB5);
}

//! switch off status led 2 - yellow
void switchOffStatusYellow(void)
{
	// switch off
	PORTB &= ~(1 << PB6);
}

//! switch off status led 3 - red
void switchOffStatusRed(void)
{
	// switch off
	PORTB &= ~(1 << PB7);
}

//! switch on dot 1
void switchOnDot1(void)
{
	// switch on
	PORTB |= (1 << PB0);
}

//! switch on dot 2
void switchOnDot2(void)
{
	// switch on
	PORTB |= (1 << PB1);
}

//! switch on dot 3
void switchOnDot3(void)
{
	// switch on
	PORTB |= (1 << PB2);
}

//! switch on dot 4
void switchOnDot4(void)
{
	// switch on
	PORTB |= (1 << PB3);
}

//! switch off dot 1
void switchOffDot1(void)
{
	// switch off
	PORTB &= ~(1 << PB0);
}

//! switch off dot 2
void switchOffDot2(void)
{
	// switch off
	PORTB &= ~(1 << PB1);
}

//! switch off dot 3
void switchOffDot3(void)
{
	// switch off
	PORTB &= ~(1 << PB2);
}

//! switch off dot 4
void switchOffDot4(void)
{
	// switch off
	PORTB &= ~(1 << PB3);
}

//! switch on char
void switchOnChar(void)
{
	// switch on
	PORTB |= (1 << PB4);
}

//! switch off char
void switchOffChar(void)
{
	// switch off
	PORTB &= ~(1 << PB4);
}

/*
//! heartbeat on led as an soft-pwm
void heartbeat(void)
{
	_delay_us(DELAY);
	i++;
		
	if (i == pwm)
	{
		// switch led at PB0 off
		PORTB &= ~(1 << PB0);
	}

	if (i == i_max)
	{
		// start new pwm phase			
		i = 0;
		// switch led at PB0 on
		PORTB |= (1 << PB0);

		// control direction
		if (updown >= 1)
		{
			pwm++;
		}
		else
		{
			pwm--;
		}

		// control pwm borders
		if (pwm >= i_max)
		{
			updown = 0;
		}
		if (pwm <= 1)
		{
			updown = 1;
		}
	}
}
*/