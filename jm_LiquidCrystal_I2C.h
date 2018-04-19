
// (https://hmario.home.xs4all.nl/arduino/fm_LiquidCrystal_I2C/)
// https://arduino-info.wikispaces.com/fm_LCD-Blue-I2C
// https://bitbucket.org/fmalpartida/new-liquidcrystal/downloads
// https://bitbucket.org/fmalpartida/new-liquidcrystal/downloads/NewliquidCrystal_1.3.5.zip

#ifndef _jm_LiquidCrystal_I2C_h
#define _jm_LiquidCrystal_I2C_h

#include <fm_LiquidCrystal_I2C.h>

#ifndef sgn
template <typename T> int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}
#endif

#ifndef sign
template <typename T> int sign(T val) {
    return (val >= T(0));
}
#endif

// -----------------------------------------------------------------------------

class jm_LiquidCrystal_I2C : public fm_LiquidCrystal_I2C {

private:
protected:

	char buf[12]; // +0.123456789
	int len;

	int buf_u32( uint32_t u32 );
	int buf_s32( int32_t s32 );
	int buf_u32f( uint32_t u32, int nfrac );
	int buf_s32f( int32_t value, int nfrac );
	int buf_rjprint( int num ); // right justified print

public:

	jm_LiquidCrystal_I2C();

	void begin();

	void clr_screen();
	void set_cursor( int col, int row );

	int print_space();
	int print_spaces( int num );

	int print_sgn( int sgn );
	int print_sign( bool sign );

	int print_u8( uint8_t value );
	int print_u8( uint8_t value, int nprint );

	int print_s8( int8_t value );
	int print_s8( int8_t value, int nprint );

	int print_u16( uint16_t value );
	int print_u16( uint16_t value, int nprint );

	int print_s16( int16_t value );
	int print_s16( int16_t value, int nprint );

	int print_u32( uint32_t value );
	int print_u32( uint32_t value, int nprint );

	int print_s32( int32_t value );
	int print_s32( int32_t value, int nprint );

	int print_h4( uint8_t value );
	int print_h8( uint8_t value );
	int print_h16( uint16_t value );
	int print_h24( uint32_t value );
	int print_h32( uint32_t value );

	int print_u32f( uint32_t value, int nfrac );
	int print_u32f( uint32_t value, int nfrac, int nprint );

	int print_s32f( int32_t value, int nfrac );
	int print_s32f( int32_t value, int nfrac, int nprint );
};

// -----------------------------------------------------------------------------

#endif
