<!--

2017-05-03: Initial commit.

Licence: LGPL v3

Author: Jean-Marc Paratte
Email: jean-marc@paratte.ch

-->

# jm_LiquidCrystal_I2C Arduino Library

My enhanced fm_LiquidCrystal_I2C Arduino Library 

````
2018-05-14: v1.0.2 - Solve OUTPUT mistake in fm_I2CIO.cpp.
2018-04-19: v1.0.1 - Solve conflict with NewLiquidCrystal_1.3.5.
2017-05-03: v1.0.0 - Initial commit.
````

jm_LiquidCrystal_I2C is a fork of fm_LiquidCrystal_I2C.h written by F. Malpartida.

My version is dedicated to the common blue 4x20 lines LCD display I2C interfaced (address 0x3F).
It is driven by jm_Scheduler and jm_Wire. 
So all delay() functions have been replaced to allow real multitasking process.

See _jm_LiquidCrystal_I2C_demo_ example.

## Example

    #include <jm_Scheduler.h>
    #include <jm_Wire.h>
    extern uint16_t twi_readFrom_timeout;
    extern uint16_t twi_writeTo_timeout;
    extern bool twi_readFrom_wait;
    extern bool twi_writeTo_wait;
    #include <jm_LiquidCrystal_I2C.h>
  
    // your code...
  
    void setup(void)
    {
	Wire.begin();
	twi_readFrom_wait = false; // Suppress twi_readFrom() waiting loop
	twi_writeTo_wait = false; // Suppress twi_writeTo() waiting loop

	lcd.begin();
	while (lcd._i2cio.yield_request()) jm_Scheduler::yield();
      
    }
  
    void loop(void)
    {
	jm_Scheduler::yield();
    }

