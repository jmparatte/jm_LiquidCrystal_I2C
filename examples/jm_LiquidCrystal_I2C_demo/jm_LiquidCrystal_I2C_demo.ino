
#define __PROG__ "jm_LiquidCrystal_I2C_demo"

#include <jm_LiquidCrystal_I2C.h>

jm_LiquidCrystal_I2C lcd;

const uint8_t charBitmap[8][8] = {
   { 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
   { 0x00, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
   { 0x00, 0x00, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00 },
   { 0x00, 0x00, 0x00, 0x1F, 0x00, 0x00, 0x00, 0x00 },
   { 0x00, 0x00, 0x00, 0x00, 0x1F, 0x00, 0x00, 0x00 },
   { 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x00, 0x00 },
   { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x00 },
   { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F }

};

void setup()
{
	Serial.begin(115200);
	while (!Serial && millis()<3000); // wait for USB Serial ready
	Serial.print(F(__PROG__));
	Serial.print(F("..."));
	Serial.println();

	lcd.begin();

	for( int i=0; i<8; i++ ) lcd.createChar( i, (uint8_t *)charBitmap[i] );
}

void loop()
{
	int32_t rand32 = -2147483648+random()+random();
	uint32_t time32 = millis();
	static char c = 0x00;

	lcd.set_cursor( 0, 0 );
	lcd.print_s32( rand32 ); // 11 chars
	lcd.print_space();
	lcd.print_h32( (uint32_t)rand32 ); // 8 chars

	lcd.set_cursor( 0, 1 );
	lcd.print_u32( time32 ); // 10 chars
	lcd.print_space();
	lcd.print_h32( time32 ); // 8 chars

	lcd.set_cursor( 0, 2 );
	lcd.print_h8( c );
	lcd.print_space();
	for (int i=0; i<0x10; i++) lcd.print( (char)(c + i) );
	c += 0x10;

	lcd.set_cursor( 0, 3 );
	lcd.print_h8( c );
	lcd.print_space();
	for (int i=0; i<0x10; i++) lcd.print( (char)(c + i) );
	c += 0x10;

	delay(1000);
}
