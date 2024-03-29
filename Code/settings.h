/*******************************************************************************
*
*	Author:			Georg Bauer
*	Date:			25.01.2014
*
*	Project-Title:	ClockWise
*	Description:	File only for basic defines
*
*	File-Title:		General settings for project - Header File
*
*******************************************************************************
*/

//! Definition

// Oscillator with 16MHz, no clock division
#ifndef F_CPU
#define F_CPU 16000000UL
#endif

// PWM settings (value range is 8 bit)
#define PWMVALUE_GAIN 1
#define PWMVALUE_OFFSET 0
#define PWMVALUE_MINIMUM 1
#define PWMVALUE_MAXIMUM 254

// Intensity VDR settings (value range is 8 bit)
#define INTENSITY_GAIN 3
#define INTENSITY_OFFSET 0
#define INTENSITY_MINIMUM 0
#define INTENSITY_MAXIMUM 254

// Poti settings (value range is 8 bit)
#define POTIVALUE_GAIN 1
#define POTIVALUE_OFFSET 0
#define POTIVALUE_MINIMUM 0
#define POTIVALUE_MAXIMUM 254

// delay time of load signal (pulse width) in �s
#define DELAYLOAD 1

// delay time of register reset signal (pulse width) in �s
#define DELAYRSTREG 1

// delay between spi 8 bit values in �s
#define DELAYSPI 1

// start signal of led matrix
#define MATRIXHIGH 0b11111111
#define MATRIXLOW 0b11110000

// task pre counter value
#define TASK_PRECOUNTER 15

//! Words - horizontal (in rows)
#define WORD_ROW00_S_H			0b11000000
#define WORD_ROW00_S_L			0b00000000
#define WORD_ROW00_IS_H			0b00011000
#define WORD_ROW00_IS_L			0b00000000
#define WORD_ROW00_GRAD_H		0b00000011
#define WORD_ROW00_GRAD_L		0b11000000
#define WORD_ROW01_KORZ_H		0b11110000
#define WORD_ROW01_KORZ_L		0b00000000
#define WORD_ROW01_NOCH_H		0b00000111
#define WORD_ROW01_NOCH_L		0b10000000
#define WORD_ROW01_DE_H			0b00000000
#define WORD_ROW01_DE_L			0b00110000
#define WORD_ROW02_GLEI_H		0b01111000
#define WORD_ROW02_GLEI_L		0b00000000
#define WORD_ROW02_HALWA_H		0b00000011
#define WORD_ROW02_HALWA_L		0b11100000
#define WORD_ROW03_DREI_H		0b11110000
#define WORD_ROW03_DREI_L		0b00000000
#define WORD_ROW03_VAEDDL_H		0b00001111
#define WORD_ROW03_VAEDDL_L		0b10000000
#define WORD_ROW04_GUDE_H		0b01111000
#define WORD_ROW04_GUDE_L		0b00000000
#define WORD_ROW04_ZUM_H		0b00000011
#define WORD_ROW04_ZUM_L		0b10000000
#define WORD_ROW05_OHNSE_H		0b01111100
#define WORD_ROW05_OHNSE_L		0b00000000
#define WORD_ROW05_FUENFE_H		0b00000011
#define WORD_ROW05_FUENFE_L		0b11100000
#define WORD_ROW06_ZWOELFE_H	0b00011111
#define WORD_ROW06_ZWOELFE_L	0b10000000
#define WORD_ROW07_ZEHNE_H		0b01111100
#define WORD_ROW07_ZEHNE_L		0b00000000
#define WORD_ROW07_VIERE_H		0b00000011
#define WORD_ROW07_VIERE_L		0b11100000
#define WORD_ROW08_SECHSE_H		0b00111111
#define WORD_ROW08_SECHSE_L		0b00000000
#define WORD_ROW08_ELFE_H		0b00000001
#define WORD_ROW08_ELFE_L		0b11100000
#define WORD_ROW09_SIWWENE_H	0b11111110
#define WORD_ROW09_SIWWENE_L	0b00000000
#define WORD_ROW09_DREI_H		0b00000001
#define WORD_ROW09_DREI_L		0b11100000
#define WORD_ROW10_MIDDANACHD_H	0b01111111
#define WORD_ROW10_MIDDANACHD_L	0b11100000
#define WORD_ROW10_ACHDE_H		0b00000001
#define WORD_ROW10_ACHDE_L		0b11110000
#define WORD_ROW11_ZWEH_H		0b11110000
#define WORD_ROW11_ZWEH_L		0b00000000
#define WORD_ROW11_ZEHNE_H		0b00001111
#define WORD_ROW11_ZEHNE_L		0b10000000
#define WORD_ROW11_NEUNE_H		0b00000001
#define WORD_ROW11_NEUNE_L		0b11110000

//! Words - vertical (in columns)
#define WORD_ROW00_GEBODSDAG_H	0b00000000
#define WORD_ROW00_GEBODSDAG_L	0b00010000
#define WORD_ROW01_GEBODSDAG_H	0b00000000
#define WORD_ROW01_GEBODSDAG_L	0b00010000
#define WORD_ROW02_GEBODSDAG_H	0b00000000
#define WORD_ROW02_GEBODSDAG_L	0b00010000
#define WORD_ROW03_GEBODSDAG_H	0b00000000
#define WORD_ROW03_GEBODSDAG_L	0b00010000
#define WORD_ROW04_GEBODSDAG_H	0b00000000
#define WORD_ROW04_GEBODSDAG_L	0b00010000
#define WORD_ROW05_GEBODSDAG_H	0b00000000
#define WORD_ROW05_GEBODSDAG_L	0b00010000
#define WORD_ROW06_GEBODSDAG_H	0b00000000
#define WORD_ROW06_GEBODSDAG_L	0b00010000
#define WORD_ROW07_GEBODSDAG_H	0b00000000
#define WORD_ROW07_GEBODSDAG_L	0b00010000
#define WORD_ROW08_GEBODSDAG_H	0b00000000
#define WORD_ROW08_GEBODSDAG_L	0b00010000
#define WORD_ROW01_ZEID_H		0b00010000
#define WORD_ROW01_ZEID_L		0b00000000
#define WORD_ROW02_ZEID_H		0b00010000
#define WORD_ROW02_ZEID_L		0b00000000
#define WORD_ROW03_ZEID_H		0b00010000
#define WORD_ROW03_ZEID_L		0b00000000
#define WORD_ROW04_ZEID_H		0b00010000
#define WORD_ROW04_ZEID_L		0b00000000
#define WORD_ROW05_ALLES_H		0b10000000
#define WORD_ROW05_ALLES_L		0b00000000
#define WORD_ROW06_ALLES_H		0b10000000
#define WORD_ROW06_ALLES_L		0b00000000
#define WORD_ROW07_ALLES_H		0b10000000
#define WORD_ROW07_ALLES_L		0b00000000
#define WORD_ROW08_ALLES_H		0b10000000
#define WORD_ROW08_ALLES_L		0b00000000
#define WORD_ROW09_ALLES_H		0b10000000
#define WORD_ROW09_ALLES_L		0b00000000
#define WORD_ROW06_FIEDAN_H		0b00000001
#define WORD_ROW06_FIEDAN_L		0b00000000
#define WORD_ROW07_FIEDAN_H		0b00000001
#define WORD_ROW07_FIEDAN_L		0b00000000
#define WORD_ROW08_FIEDAN_H		0b00000001
#define WORD_ROW08_FIEDAN_L		0b00000000
#define WORD_ROW09_FIEDAN_H		0b00000001
#define WORD_ROW09_FIEDAN_L		0b00000000
#define WORD_ROW10_FIEDAN_H		0b00000001
#define WORD_ROW10_FIEDAN_L		0b00000000
#define WORD_ROW11_FIEDAN_H		0b00000001
#define WORD_ROW11_FIEDAN_L		0b00000000