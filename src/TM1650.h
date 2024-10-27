/** ============================================
 * 7 segment display driver for JY-MCU module based on TM1650 chip
 * Copyright (c) 2015 Anatoli Arkhipenko
 * 
 * 
 * Changelog:
 *	v1.0.0:
 *		2015-02-24 - Initial release 
 *
 *	v1.0.1:  
 *		2015-04-27 - Added support of program memory (PROGMEM) to store the ASCII to Segment Code table
 *
 *	v1.0.2:
 *		2015-08-08 - Added check if panel is connected during init. All calls will be disabled if panel was not connected during init.
 *
 *  v1.1.0:
 *      2015-12-20 - Code cleanup. Moved to a single header file. Added Gradual brightness method.
 *
 *  v2.0.0:
 *      2024-10-27 - Converted TM1650 class to a template to support both TwoWire (hardware I2C) 
 *                   and SoftwareWire (software I2C) seamlessly.
 *                 - Replaced all `byte`, `int`, etc., types with fixed-width types (`uint8_t`, `int16_t`, etc.) 
 *                   for increased portability and clarity.
 * ===============================================*/
#ifndef _TM1650_H_
#define _TM1650_H_

#include <Arduino.h>

// #define TM1650_USE_PROGMEM

#ifdef TM1650_USE_PROGMEM
#if (defined(__AVR__))
#include <avr\pgmspace.h>
#else
#include <pgmspace.h>
#endif
#endif

#define TM1650_DISPLAY_BASE 0x34 // Address of the left-most digit
#define TM1650_DCTRL_BASE 0x24	 // Address of the control register of the left-most digit
#define TM1650_NUM_DIGITS 16	 // max number of digits
#define TM1650_MAX_STRING 128	 // number of digits

#define TM1650_BIT_ONOFF 0b00000001
#define TM1650_MSK_ONOFF 0b11111110
#define TM1650_BIT_DOT 0b00000001
#define TM1650_MSK_DOT 0b11110111
#define TM1650_BRIGHT_SHIFT 4
#define TM1650_MSK_BRIGHT 0b10001111
#define TM1650_MIN_BRIGHT 0
#define TM1650_MAX_BRIGHT 7

#ifndef TM1650_USE_PROGMEM
const uint8_t TM1650_CDigits[128]
{
#else
const PROGMEM uint8_t TM1650_CDigits[128]{
#endif
	// 0x00  0x01  0x02  0x03  0x04  0x05  0x06  0x07  0x08  0x09  0x0A  0x0B  0x0C  0x0D  0x0E  0x0F
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,		// 0x00
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 0x10
		0x00, 0x82, 0x21, 0x00, 0x00, 0x00, 0x00, 0x02, 0x39, 0x0F, 0x00, 0x00, 0x00, 0x40, 0x80, 0x00, // 0x20
		0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7f, 0x6f, 0x00, 0x00, 0x00, 0x48, 0x00, 0x53, // 0x30
		0x00, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71, 0x6F, 0x76, 0x06, 0x1E, 0x00, 0x38, 0x00, 0x54, 0x3F, // 0x40
		0x73, 0x67, 0x50, 0x6D, 0x78, 0x3E, 0x00, 0x00, 0x00, 0x6E, 0x00, 0x39, 0x00, 0x0F, 0x00, 0x08, // 0x50
		0x63, 0x5F, 0x7C, 0x58, 0x5E, 0x7B, 0x71, 0x6F, 0x74, 0x02, 0x1E, 0x00, 0x06, 0x00, 0x54, 0x5C, // 0x60
		0x73, 0x67, 0x50, 0x6D, 0x78, 0x1C, 0x00, 0x00, 0x00, 0x6E, 0x00, 0x39, 0x30, 0x0F, 0x00, 0x00	// 0x70
};

// TM1650 template class that supports both TwoWire (hardware I2C) and SoftwareWire (software I2C).
template <class WireType>
class TM1650
{
public:
	TM1650(WireType *wireInstance, uint8_t aNumDigits = 4);

	void init();
	void clear();
	void displayOn();
	void displayOff();
	void displayState(bool aState);
	void displayString(char *aString);
	int displayRunning(char *aString);
	int displayRunningShift();
	void setBrightness(uint8_t aValue = TM1650_MAX_BRIGHT);
	void setBrightnessGradually(uint8_t aValue = TM1650_MAX_BRIGHT);
	inline uint8_t getBrightness() { return iBrightness; };

	void controlPosition(uint8_t aPos, uint8_t aValue);
	void setPosition(uint8_t aPos, uint8_t aValue);
	void setDot(uint8_t aPos, bool aState);
	uint8_t getPosition(uint8_t aPos) { return iBuffer[aPos]; };
	inline uint8_t getNumPositions() { return iNumDigits; };

private:
	WireType *GenericWire;
	char *iPosition;
	bool iActive;
	uint8_t iNumDigits;
	uint8_t iBrightness;
	char iString[TM1650_MAX_STRING + 1];
	uint8_t iBuffer[TM1650_NUM_DIGITS + 1];
	uint8_t iCtrl[TM1650_NUM_DIGITS];
};

//  ----  Implementation ----

/** Constructor, uses default values for the parameters
 * so could be called with no parameters.
 * aNumDigits - number of display digits (default = 4)
 */
template <class WireType>
TM1650<WireType>::TM1650(WireType *wireInstance, uint8_t aNumDigits) : GenericWire(wireInstance)
{
	iNumDigits = (aNumDigits > TM1650_NUM_DIGITS) ? TM1650_NUM_DIGITS : aNumDigits;
}

/** Initialization
 * initializes the driver. Turns display on, but clears all digits.
 */
template <class WireType>
void TM1650<WireType>::init()
{
	iPosition = NULL;
	for (int i = 0; i < iNumDigits; i++)
	{
		iBuffer[i] = 0;
		iCtrl[i] = 0;
	}
	GenericWire->beginTransmission(TM1650_DISPLAY_BASE);
	iActive = (GenericWire->endTransmission() == 0);
	clear();
	displayOn();
}

/** Set brightness of all digits equally
 * aValue - brightness value with 1 being the lowest, and 7 being the brightest
 */
template <class WireType>
void TM1650<WireType>::setBrightness(uint8_t aValue)
{
	if (!iActive)
		return;

	iBrightness = (aValue > TM1650_MAX_BRIGHT) ? TM1650_MAX_BRIGHT : aValue;

	for (int i = 0; i < iNumDigits; i++)
	{
		GenericWire->beginTransmission(TM1650_DCTRL_BASE + i);
		iCtrl[i] = (iCtrl[i] & TM1650_MSK_BRIGHT) | (iBrightness << TM1650_BRIGHT_SHIFT);
		GenericWire->write((uint8_t)iCtrl[i]);
		GenericWire->endTransmission();
	}
}

/** Set brightness of all digits equally
 * aValue - brightness value with 1 being the lowest, and 7 being the brightest
 */
template <class WireType>
void TM1650<WireType>::setBrightnessGradually(uint8_t aValue)
{
	if (!iActive || aValue == iBrightness)
		return;

	if (aValue > TM1650_MAX_BRIGHT)
		aValue = TM1650_MAX_BRIGHT;
	int step = (aValue < iBrightness) ? -1 : 1;
	uint8_t i = iBrightness;
	do
	{
		setBrightness(i);
		delay(50);
		i += step;
	} while (i != aValue);
}

/** Turns display on or off according to aState
 */
template <class WireType>
void TM1650<WireType>::displayState(bool aState)
{
	if (aState)
		displayOn();
	else
		displayOff();
}

/** Turns the display on
 */
template <class WireType>
void TM1650<WireType>::displayOn()
// turn all digits on
{
	if (!iActive)
		return;
	for (int i = 0; i < iNumDigits; i++)
	{
		GenericWire->beginTransmission(TM1650_DCTRL_BASE + i);
		iCtrl[i] = (iCtrl[i] & TM1650_MSK_ONOFF) | TM1650_BIT_DOT;
		GenericWire->write((uint8_t)iCtrl[i]);
		GenericWire->endTransmission();
	}
}
/** Turns the display off
 */
template <class WireType>
void TM1650<WireType>::displayOff()
// turn all digits off
{
	if (!iActive)
		return;
	for (int i = 0; i < iNumDigits; i++)
	{
		GenericWire->beginTransmission(TM1650_DCTRL_BASE + i);
		iCtrl[i] = (iCtrl[i] & TM1650_MSK_ONOFF);
		GenericWire->write((uint8_t)iCtrl[i]);
		GenericWire->endTransmission();
	}
}

/** Directly write to the CONTROL register of the digital position
 * aPos = position to set the control register for
 * aValue = value to write to the position
 *
 * Internal control buffer is updated as well
 */
template <class WireType>
void TM1650<WireType>::controlPosition(uint8_t aPos, uint8_t aValue)
{
	if (!iActive)
		return;
	if (aPos < iNumDigits)
	{
		GenericWire->beginTransmission(TM1650_DCTRL_BASE + (int)aPos);
		iCtrl[aPos] = aValue;
		GenericWire->write(aValue);
		GenericWire->endTransmission();
	}
}

/** Directly write to the digit register of the digital position
 * aPos = position to set the digit register for
 * aValue = value to write to the position
 *
 * Internal position buffer is updated as well
 */
template <class WireType>
void TM1650<WireType>::setPosition(uint8_t aPos, uint8_t aValue)
{
	if (!iActive)
		return;
	if (aPos < iNumDigits)
	{
		GenericWire->beginTransmission(TM1650_DISPLAY_BASE + (int)aPos);
		iBuffer[aPos] = aValue;
		GenericWire->write(aValue);
		GenericWire->endTransmission();
	}
}

/** Directly set/clear a 'dot' next to a specific position
 * aPos = position to set/clear the dot for
 * aState = display the dot if true, clear if false
 *
 * Internal buffer is updated as well
 */
template <class WireType>
void TM1650<WireType>::setDot(uint8_t aPos, bool aState)
{
	iBuffer[aPos] = iBuffer[aPos] & 0x7F | (aState ? 0b10000000 : 0);
	setPosition(aPos, iBuffer[aPos]);
}

/** Clear all digits. Keep the display on.
 */
template <class WireType>
void TM1650<WireType>::clear()
// clears all digits
{
	if (!iActive)
		return;
	for (int i = 0; i < iNumDigits; i++)
	{
		GenericWire->beginTransmission(TM1650_DISPLAY_BASE + i);
		iBuffer[i] = 0;
		GenericWire->write((uint8_t)0);
		GenericWire->endTransmission();
	}
}

/** Display string on the display
 * aString = character array to be displayed
 *
 * Internal buffer is updated as well
 * Only first N positions of the string are displayed if
 *  the string is longer than the number of digits
 */
template <class WireType>
void TM1650<WireType>::displayString(char *aString)
{
	if (!iActive)
		return;
	for (int i = 0; i < iNumDigits; i++)
	{
		uint8_t a = ((uint8_t)aString[i]) & 0b01111111;
		uint8_t dot = ((uint8_t)aString[i]) & 0b10000000;
#ifndef TM1650_USE_PROGMEM
		iBuffer[i] = TM1650_CDigits[a];
#else
		iBuffer[i] = pgm_read_byte_near(TM1650_CDigits + a);
#endif
		if (a)
		{
			GenericWire->beginTransmission(TM1650_DISPLAY_BASE + i);
			GenericWire->write(iBuffer[i] | dot);
			GenericWire->endTransmission();
		}
		else
			break;
	}
}

/** Display string on the display in a running fashion
 * aString = character array to be displayed
 *
 * Starts with first N positions of the string.
 * Subsequent characters are displayed with 1 char shift each time displayRunningShift() is called
 *
 * returns: number of iterations remaining to display the whole string
 */
template <class WireType>
int TM1650<WireType>::displayRunning(char *aString)
{

	strncpy(iString, aString, TM1650_MAX_STRING + 1);
	iPosition = iString;
	iString[TM1650_MAX_STRING] = '\0'; // just in case.
	displayString(iPosition);

	int l = strlen(iPosition);
	if (l <= iNumDigits)
		return 0;
	return (l - iNumDigits);
}

/** Display next segment (shifting to the left) of the string set by displayRunning()
 * Starts with first N positions of the string.
 * Subsequent characters are displayed with 1 char shift each time displayRunningShift is called
 *
 * returns: number of iterations remaining to display the whole string
 */
template <class WireType>
int TM1650<WireType>::displayRunningShift()
{
	if (strlen(iPosition) <= iNumDigits)
		return 0;
	displayString(++iPosition);
	return (strlen(iPosition) - iNumDigits);
}

#endif /* _TM1650_H_ */
