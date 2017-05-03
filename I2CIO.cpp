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
// @file I2CIO.h
// This file implements a basic IO library using the PCF8574 I2C IO Expander
// chip.
//
// @brief
// Implement a basic IO library to drive the PCF8574* I2C IO Expander ASIC.
// The library implements basic IO general methods to configure IO pin direction
// read and write uint8_t operations and basic pin level routines to set or read
// a particular IO port.
//
//
// @version API 1.0.0
//
// @author F. Malpartida - fmalpartida@gmail.com

////////////////////////////////////////////////////////////////////////////////

#include <jm_Scheduler.h>

////////////////////////////////////////////////////////////////////////////////

void lcd_i2c_write(uint8_t data);
void lcd_i2c_wait(uint16_t us);

//------------------------------------------------------------------------------

#include <jm_LiquidCrystal_I2C.h>
//#include "jm_LiquidCrystal_I2C.h"

//------------------------------------------------------------------------------

#include <Arduino.h>

//#include <Wire.h>
#include <jm_Wire.h>

//#include <inttypes.h>

#include "I2CIO.h"

//------------------------------------------------------------------------------

I2CIO::I2CIO ( )
{
	_i2cAddr	  = 0x0;
	_dirMask	  = 0xFF;	 // mark all as INPUTs
	_shadow		= 0x0;	  // no values set
	_initialised = false;
}

// PUBLIC METHODS
// ---------------------------------------------------------------------------


//
// begin
int I2CIO::begin (  uint8_t i2cAddr )
{
	lcd_i2c_addr = i2cAddr;

	_i2cAddr = i2cAddr;

	Wire.begin ( );

	_initialised = isAvailable ( _i2cAddr );

	if (_initialised)
	{
#if (ARDUINO <  100)
		_shadow = Wire.receive ();
#else
		_shadow = Wire.read (); // Remove the byte read don't need it.
#endif
	}
	return ( _initialised );
}

//
// pinMode
void I2CIO::pinMode ( uint8_t pin, uint8_t dir )
{
	if ( _initialised )
	{
		if ( OUTPUT == dir )
		{
			_dirMask &= ~( 1 << pin );
		}
		else
		{
			_dirMask |= ( 1 << pin );
		}
	}
}

//
// portMode
void I2CIO::portMode ( uint8_t dir )
{

	if ( _initialised )
	{
		if ( dir == INPUT )
		{
			_dirMask = 0xFF;
		}
		else
		{
			_dirMask = 0x00;
		}
	}
}

//
// read
uint8_t I2CIO::read ( void )
{
	uint8_t retVal = 0;

	if ( _initialised )
	{
		Wire.requestFrom ( _i2cAddr, (uint8_t)1 );
#if (ARDUINO <  100)
		retVal = ( _dirMask & Wire.receive ( ) );
#else
		retVal = ( _dirMask & Wire.read ( ) );
#endif

	}
	return ( retVal );
}

//
// write
int I2CIO::write ( uint8_t value )
{
	int status = 0;

	if ( _initialised )
	{
		// Only write HIGH the values of the ports that have been initialised as
		// outputs updating the output shadow of the device
		_shadow = ( value & ~(_dirMask) );

#if 0
		Wire.beginTransmission ( _i2cAddr );
		Wire.write ( _shadow );
		status = Wire.endTransmission ();

		delayMicroseconds( 300 );
#else
//		if (_shadow == (uint8_t)0xFF) Serial.println('$');
		assert (_shadow != 0xFF);

		lcd_i2c_write(_shadow);
#endif
	}
	return ( (status == 0) );
}



void I2CIO::wait ( uint16_t us )
{
#if 0
	delayMicroseconds( us );
#else
	lcd_i2c_wait(us);
#endif
}



//
// digitalRead
uint8_t I2CIO::digitalRead ( uint8_t pin )
{
	uint8_t pinVal = 0;

	// Check if initialised and that the pin is within range of the device
	// -------------------------------------------------------------------
	if ( ( _initialised ) && ( pin <= 7 ) )
	{
		// Remove the values which are not inputs and get the value of the pin
		pinVal = this->read() & _dirMask;
		pinVal = ( pinVal >> pin ) & 0x01; // Get the pin value
	}
	return (pinVal);
}

//
// digitalWrite
int I2CIO::digitalWrite ( uint8_t pin, uint8_t level )
{
	uint8_t writeVal;
	int status = 0;

	// Check if initialised and that the pin is within range of the device
	// -------------------------------------------------------------------
	if ( ( _initialised ) && ( pin <= 7 ) )
	{
		// Only write to HIGH the port if the port has been configured as
		// an OUTPUT pin. Add the new state of the pin to the shadow
		writeVal = ( 1 << pin ) & ~_dirMask;
		if ( level == HIGH )
		{
			_shadow |= writeVal;

		}
		else
		{
			_shadow &= ~writeVal;
		}
		status = this->write ( _shadow );
	}
	return ( status );
}

//
// PRIVATE METHODS
// ---------------------------------------------------------------------------
bool I2CIO::isAvailable (uint8_t i2cAddr)
{
	int error;

	Wire.beginTransmission( i2cAddr );
	error = Wire.endTransmission();
	if (error==0)
	{
	  return true;
	}
	else //Some error occured
	{
	  return false;
	}
}

//------------------------------------------------------------------------------

bool I2CIO::yield_request()
{
	return (lcd_i2c_buffer_available() > 0);
}

//------------------------------------------------------------------------------

#define LCD_I2C_BUFFER_SIZE ((uint8_t) 32) //128)

uint8_t lcd_i2c_buffer[LCD_I2C_BUFFER_SIZE];
uint8_t lcd_i2c_iindex = 0;
uint8_t lcd_i2c_oindex = 0;
uint8_t lcd_i2c_count = 0;

int lcd_i2c_buffer_free()
{
	return (LCD_I2C_BUFFER_SIZE - lcd_i2c_count);
}

int lcd_i2c_buffer_write(uint8_t data)
{
	assert(lcd_i2c_buffer_free() >= 1);

	lcd_i2c_buffer[ lcd_i2c_iindex++ ] = data;
	if (lcd_i2c_iindex == LCD_I2C_BUFFER_SIZE) lcd_i2c_iindex = 0;
	lcd_i2c_count++;

	return 1;
}

int lcd_i2c_buffer_available()
{
	return (lcd_i2c_count);
}

uint8_t lcd_i2c_buffer_read()
{
	assert(lcd_i2c_buffer_available() >= 1);

	uint8_t data = lcd_i2c_buffer[ lcd_i2c_oindex++ ];
	if (lcd_i2c_oindex == LCD_I2C_BUFFER_SIZE) lcd_i2c_oindex = 0;
	lcd_i2c_count--;

	return data;
}

//------------------------------------------------------------------------------

jm_Scheduler lcd_i2c_scheduler;

uint8_t lcd_i2c_addr;

void lcd_i2c_routine()
{
	if (lcd_i2c_buffer_available() == 0)
	{
//		Serial.println('.');

		lcd_i2c_scheduler.stop();

		return;
	}

	uint8_t data = lcd_i2c_buffer_read();

	if (data == 0xFF)
	{
		data = lcd_i2c_buffer_read();

//		Serial.print('w');
//		Serial.print(20*data);
//		Serial.print(';');

		lcd_i2c_scheduler.rearm_async( 20*data );
	}
	else
	{
//		Serial.print('(');
//		Serial.print(data, HEX);
//		Serial.print(')');

		Wire.beginTransmission( lcd_i2c_addr );
		Wire.write( data );
		Wire.endTransmission( true );

		lcd_i2c_scheduler.rearm_async( 300 );
	}
}

#if 01

void lcd_i2c_write(uint8_t data)
{
//	Serial.print('<');
//	Serial.print(data, HEX);
//	Serial.print('>');

	while (lcd_i2c_buffer_free() < 1) jm_Scheduler::yield();

	lcd_i2c_buffer_write(data);

	if (!lcd_i2c_scheduler) lcd_i2c_scheduler.start( lcd_i2c_routine );
}

void lcd_i2c_wait(uint16_t us)
{
//	Serial.print('w');
//	Serial.print(us);
//	Serial.print(';');

	while (lcd_i2c_buffer_free() < 2) jm_Scheduler::yield();

	lcd_i2c_buffer_write(0xFF);
	lcd_i2c_buffer_write((us + (20 - 1))/20);

	if (!lcd_i2c_scheduler) lcd_i2c_scheduler.start( lcd_i2c_routine );
}

#else

void lcd_i2c_write(uint8_t data)
{
//	Serial.print('<');
//	Serial.print(data, HEX);
//	Serial.print('>');

	Wire.beginTransmission( _i2cAddr );
	Wire.write( data );
	Wire.endTransmission( true );

	delayMicroseconds(300);
}

void lcd_i2c_wait(uint16_t us)
{
//	Serial.print('w');
//	Serial.print(us);
//	Serial.print(';');

	delayMicroseconds(us);
}

#endif

//------------------------------------------------------------------------------

// END.