// ---------------------------------------------------------------------------
// Created by Francisco Malpartida on 20/08/11.
// Copyright 2011 - Under creative commons license 3.0:
//		  Attribution-ShareAlike CC BY-SA
//
// This software is furnished "as is", without technical support, and with no
// warranty, express or implied, as to its usefulness for any purpose.
//
// Thread Safe: No
// Extendable: Yes
//
// @file fm_LCD.cpp
// This file implements a basic liquid crystal library that comes as standard
// in the Arduino SDK.
//
// @brief
// This is a basic implementation of the HD44780 library of the
// Arduino SDK. This library is a refactored version of the one supplied
// in the Arduino SDK in such a way that it simplifies its extension
// to support other mechanism to communicate to LCDs such as I2C, Serial, SR, ...
// The original library has been reworked in such a way that this will be
// the base class implementing all generic methods to command an fm_LCD based
// on the Hitachi HD44780 and compatible chipsets.
//
// This base class is a pure abstract class and needs to be extended. As reference,
// it has been extended to drive 4 and 8 bit mode control, LCDs and I2C extension
// backpacks such as the I2CLCDextraIO using the PCF8574* I2C IO Expander ASIC.
//
//
// @version API 1.1.0
//
// 2012.03.29 bperrybap - changed comparision to use LCD_5x8DOTS rather than 0
// @author F. Malpartida - fmalpartida@gmail.com
// ---------------------------------------------------------------------------
//#include <stdio.h>
//#include <string.h>
//#include <inttypes.h>

#include <Arduino.h>

#include "fm_LCD.h"


// CLASS CONSTRUCTORS
// ---------------------------------------------------------------------------
// Constructor
fm_LCD::fm_LCD ()
{

}

// PUBLIC METHODS
// ---------------------------------------------------------------------------
// When the display powers up, it is configured as follows:
// 0. fm_LCD starts in 8 bit mode
// 1. Display clear
// 2. Function set:
//	 DL = 1; 8-bit interface data
//	 N = 0; 1-line display
//	 F = 0; 5x8 dot character font
// 3. Display on/off control:
//	 D = 0; Display off
//	 C = 0; Cursor off
//	 B = 0; Blinking off
// 4. Entry mode set:
//	 I/D = 1; Increment by 1
//	 S = 0; No shift
//
// Note, however, that resetting the Arduino doesn't reset the fm_LCD, so we
// can't assume that its in that state when a application starts (and the
// LiquidCrystal constructor is called).
// A call to begin() will reinitialize the fm_LCD.
//
void fm_LCD::begin(uint8_t cols, uint8_t lines, uint8_t dotsize)
{
	if (lines > 1)
	{
		_displayfunction |= LCD_2LINE;
	}
	_numlines = lines;
	_cols = cols;

	// for some 1 line displays you can select a 10 pixel high font
	// ------------------------------------------------------------
	if ((dotsize != LCD_5x8DOTS) && (lines == 1))
	{
		_displayfunction |= LCD_5x10DOTS;
	}

	// HITACHI HD44780U Datasheet

	// SEE PAGE 45/46 FOR INITIALIZATION SPECIFICATION!
	// ================================================

	// [Wait for more than 15 ms after VCC rises to 4.5 V]
	// (Wait for more than 40 ms after VCC rises to 2.7 V)

	// To be compatible with all kinds of Arduino voltages,
	// we need at least 40ms after power rises above 2.7V
	// before sending commands.
	wait_us(50000); // Wait for more than 40 ms after VCC rises to 2.7 V

	// Init the fm_LCD in 4-Bit or 8-Bit mode
	// -----------------------------------
	if (! (_displayfunction & LCD_8BITMODE)) // 4-Bit mode ?
	{
		// Figure 24 4-Bit Interface

		// fm_LCD starts in 8-Bit mode, try to set 4-Bit mode

		// [BF cannot be checked before this instruction.]
		send_vm( 0x3, LCD_MODE_4BIT );
		// Function set (Interface is 8 bits long.)
		wait_us(4500); // Wait for more than 4.1 ms

		// [BF cannot be checked before this instruction]
		send_vm ( 0x3, LCD_MODE_4BIT );
		// Function set (Interface is 8 bits long.)
		wait_us(150); // Wait for more than 100 us

		// [BF cannot be checked before this instruction]
		send_vm( 0x3, LCD_MODE_4BIT );
		// Function set (Interface is 8 bits long.)
		wait_us(150); // Wait for more than 100 us

		// BF can be checked after the following instructions.
		// When BF is not checked, the waiting time between
		// instructions is longer than the execution instuction
		// time. (See Table 6.)
		send_vm ( 0x2, LCD_MODE_4BIT );
		wait_us(150); // Wait for more than 100 us
		// Function set  (Set interface to be 4 bits long.)
		// Interface is 8 bits in length
	}
	else
	{
		// Figure 23 8-Bit Interface

		// Send function set command sequence
		command(LCD_FUNCTIONSET | _displayfunction);
		wait_us(4500);  // wait more than 4.1ms

		// second try
		command(LCD_FUNCTIONSET | _displayfunction);
		wait_us(150);

		// third go
		command(LCD_FUNCTIONSET | _displayfunction);
		wait_us(150);

	}

	// Function set  (Interface is 4 bits long.  Specify the
	// number of display lines and character font.)
	// The number of display lines and character font
	// cannot be changed after this point.
	command(LCD_FUNCTIONSET | _displayfunction);
	wait_us ( 60 );  // wait more

	// turn the display on with no cursor or blinking default
	_displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
	display();

	// clear the fm_LCD
	clear();

	// Initialize to default text direction (for romance languages)
	_displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
	// set the entry mode
	command(LCD_ENTRYMODESET | _displaymode);

	backlight();
}

// Common fm_LCD Commands
// ---------------------------------------------------------------------------
void fm_LCD::clear()
{
	command(LCD_CLEARDISPLAY);				 // clear display, set cursor position to zero
	wait_us(HOME_CLEAR_EXEC);	 // this command is time consuming
}

void fm_LCD::home()
{
	command(LCD_RETURNHOME);				 // set cursor position to zero
	wait_us(HOME_CLEAR_EXEC);  // This command is time consuming
}

void fm_LCD::setCursor(uint8_t col, uint8_t row)
{
	const byte row_offsetsDef[]	= { 0x00, 0x40, 0x14, 0x54 }; // For regular LCDs
	const byte row_offsetsLarge[] = { 0x00, 0x40, 0x10, 0x50 }; // For 16x4 LCDs

	if ( row >= _numlines )
	{
		row = _numlines-1;	 // rows start at 0
	}

	// 16x4 LCDs have special memory map layout
	// ----------------------------------------
	if ( _cols == 16 && _numlines == 4 )
	{
		command(LCD_SETDDRAMADDR | (col + row_offsetsLarge[row]));
	}
	else
	{
		command(LCD_SETDDRAMADDR | (col + row_offsetsDef[row]));
	}

}

// Turn the display on/off
void fm_LCD::noDisplay()
{
	_displaycontrol &= ~LCD_DISPLAYON;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
}

void fm_LCD::display()
{
	_displaycontrol |= LCD_DISPLAYON;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// Turns the underline cursor on/off
void fm_LCD::noCursor()
{
	_displaycontrol &= ~LCD_CURSORON;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void fm_LCD::cursor()
{
	_displaycontrol |= LCD_CURSORON;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// Turns on/off the blinking cursor
void fm_LCD::noBlink()
{
	_displaycontrol &= ~LCD_BLINKON;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
}

void fm_LCD::blink()
{
	_displaycontrol |= LCD_BLINKON;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// These commands scroll the display without changing the RAM
void fm_LCD::scrollDisplayLeft(void)
{
	command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}

void fm_LCD::scrollDisplayRight(void)
{
	command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}

// This is for text that flows Left to Right
void fm_LCD::leftToRight(void)
{
	_displaymode |= LCD_ENTRYLEFT;
	command(LCD_ENTRYMODESET | _displaymode);
}

// This is for text that flows Right to Left
void fm_LCD::rightToLeft(void)
{
	_displaymode &= ~LCD_ENTRYLEFT;
	command(LCD_ENTRYMODESET | _displaymode);
}

// This method moves the cursor one space to the right
void fm_LCD::moveCursorRight(void)
{
	command(LCD_CURSORSHIFT | LCD_CURSORMOVE | LCD_MOVERIGHT);
}

// This method moves the cursor one space to the left
void fm_LCD::moveCursorLeft(void)
{
	command(LCD_CURSORSHIFT | LCD_CURSORMOVE | LCD_MOVELEFT);
}


// This will 'right justify' text from the cursor
void fm_LCD::autoscroll(void)
{
	_displaymode |= LCD_ENTRYSHIFTINCREMENT;
	command(LCD_ENTRYMODESET | _displaymode);
}

// This will 'left justify' text from the cursor
void fm_LCD::noAutoscroll(void)
{
	_displaymode &= ~LCD_ENTRYSHIFTINCREMENT;
	command(LCD_ENTRYMODESET | _displaymode);
}

// Write to CGRAM of new characters
void fm_LCD::createChar(uint8_t location, uint8_t charmap[])
{
	location &= 0x7;				// we only have 8 locations 0-7

	command(LCD_SETCGRAMADDR | (location << 3));
	wait_us(30);

	for (uint8_t i = 0; i < 8; i++)
	{
		write(charmap[i]);		// call the virtual write method
		wait_us(40);
	}
}

#ifdef __AVR__
void fm_LCD::createChar(uint8_t location, const char *charmap)
{
	location &= 0x7;	// we only have 8 memory locations 0-7

	command(LCD_SETCGRAMADDR | (location << 3));
	wait_us(30);

	for (uint8_t i = 0; i < 8; i++)
	{
		write(pgm_read_byte_near(charmap++));
		wait_us(40);
	}
}
#endif // __AVR__

//
// Switch on the backlight
void fm_LCD::backlight ( void )
{
	setBacklight(255);
}

//
// Switch off the backlight
void fm_LCD::noBacklight ( void )
{
	setBacklight(0);
}

//
// Switch fully on the fm_LCD (backlight and fm_LCD)
void fm_LCD::on ( void )
{
	display();
	backlight();
}

//
// Switch fully off the fm_LCD (backlight and fm_LCD)
void fm_LCD::off ( void )
{
	noBacklight();
	noDisplay();
}

// General fm_LCD commands - generic methods used by the rest of the commands
// ---------------------------------------------------------------------------
void fm_LCD::command(uint8_t value)
{
	send_vm(value, LCD_SEND_CMD);
}

//#if (ARDUINO <  100)
//void fm_LCD::write(uint8_t value)
//{
//	send_vm(value, LCD_SEND_DATA);
//}
//#else
size_t fm_LCD::write(uint8_t value)
{
	send_vm(value, LCD_SEND_DATA);
	return 1;				 // 1 byte sent
}
//#endif

void fm_LCD::send_vm(uint8_t value, uint8_t mode)
{
//	Serial.print(' ');
//	Serial.print('S');
//	Serial.print(value);
//	Serial.print(',');
//	Serial.print(mode);

	send(value, mode);
}

void fm_LCD::wait_us(uint16_t us)
{
//	Serial.print(' ');
//	Serial.print('W');
//	Serial.print(us);

//	delayMicroseconds( us );
	wait( us );
}

