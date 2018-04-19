
#include <jm_LiquidCrystal_I2C.h>

// -----------------------------------------------------------------------------

int jm_LiquidCrystal_I2C::buf_u32( uint32_t u32 )
{
	len = snprintf( buf, 12, "%lu", u32 );
	return len;
}

int jm_LiquidCrystal_I2C::buf_s32( int32_t s32 )
{
	if (s32)
		len = snprintf( buf, 12, "%+li", s32 );
	else
		len = snprintf( buf, 12, "0" );
	return len;
}

int jm_LiquidCrystal_I2C::buf_u32f( uint32_t u32, int nfrac )
{
	buf_u32(u32);
	if (len > nfrac) {
		memmove( &buf[len-nfrac+1], &buf[len-nfrac], nfrac );
		len++;
	} else {
		memmove( &buf[nfrac+1+1-len], &buf[0], len );
		memset( &buf[0], '0', nfrac+1+1-len );
		len = nfrac+1+1;
	}
	buf[len-nfrac-1] = '.';
	return len;
}

int jm_LiquidCrystal_I2C::buf_s32f( int32_t value, int nfrac )
{
	buf_u32f( (value<0?-value:+value), nfrac );
	if (value) {
		memmove( &buf[1], &buf[0], len );
		buf[0] = sign(value)?'+':'-';
		len++;
	}
	return len;
}

int jm_LiquidCrystal_I2C::buf_rjprint( int num ) // right justification print
{
	return ( print_spaces( (num-len) >= 0 ? (num-len) : 0 ) + write( buf, len ) );
}

// -----------------------------------------------------------------------------

jm_LiquidCrystal_I2C::jm_LiquidCrystal_I2C() : fm_LiquidCrystal_I2C::fm_LiquidCrystal_I2C(0x3F, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE)
{
}

// -----------------------------------------------------------------------------

void jm_LiquidCrystal_I2C::begin()
{
	fm_LiquidCrystal_I2C::begin(20, 4, LCD_5x8DOTS);
}

void jm_LiquidCrystal_I2C::clr_screen()
{
	clear();
}

void jm_LiquidCrystal_I2C::set_cursor( int col, int row )
{
	setCursor(col, row);
}

// -----------------------------------------------------------------------------

int jm_LiquidCrystal_I2C::print_space()
{
	return print(' ');
}

int jm_LiquidCrystal_I2C::print_spaces( int num )
{
	for (int i=0; i<num; i++) print_space();
	return num;
}

// -----------------------------------------------------------------------------

int jm_LiquidCrystal_I2C::print_sgn( int sgn )
{
	return print( (sgn==+1) ? '+' : (sgn==-1) ? '-' : ' ' );
}

int jm_LiquidCrystal_I2C::print_sign( bool sign )
{
	return print( sign ? '+' : '-' );
}

// -----------------------------------------------------------------------------

int jm_LiquidCrystal_I2C::print_u8( uint8_t value )
{
	return print_u32( (uint32_t)value, 3 );
}

int jm_LiquidCrystal_I2C::print_u8( uint8_t value, int nprint )
{
	return print_u32( (uint32_t)value, nprint );
}

// -----------------------------------------------------------------------------

int jm_LiquidCrystal_I2C::print_s8( int8_t value )
{
	return print_s32( (int32_t)value, 3+1 );
}

int jm_LiquidCrystal_I2C::print_s8( int8_t value, int nprint )
{
	return print_s32( (int32_t)value, nprint );
}

// -----------------------------------------------------------------------------

int jm_LiquidCrystal_I2C::print_u16( uint16_t value )
{
	return print_u32( (uint32_t)value, 5 );
}

int jm_LiquidCrystal_I2C::print_u16( uint16_t value, int nprint )
{
	return print_u32( (uint32_t)value, nprint );
}

// -----------------------------------------------------------------------------

int jm_LiquidCrystal_I2C::print_s16( int16_t value )
{
	return print_s32( (int32_t)value, 5+1 );
}

int jm_LiquidCrystal_I2C::print_s16( int16_t value, int nprint )
{
	return print_s32( (int32_t)value, nprint );
}

// -----------------------------------------------------------------------------

int jm_LiquidCrystal_I2C::print_u32( uint32_t value )
{
	return print_u32( value, 10 );
}

int jm_LiquidCrystal_I2C::print_u32( uint32_t value, int nprint )
{
	buf_u32( value );
	return buf_rjprint( nprint );
}

// -----------------------------------------------------------------------------

int jm_LiquidCrystal_I2C::print_s32( int32_t value )
{
	return print_s32( value, 10+1 );
}

int jm_LiquidCrystal_I2C::print_s32( int32_t value, int nprint )
{
	buf_s32( value );
	return buf_rjprint( nprint );
}

// -----------------------------------------------------------------------------

int jm_LiquidCrystal_I2C::print_h4( uint8_t value )
{
	value &= 0x0F;
	return print( value>=10 ? (char)('A'+value-10) : (char)('0'+value) );
}

int jm_LiquidCrystal_I2C::print_h8( uint8_t value )
{
	return print_h4( value>>4 ) + print_h4( value>>0 );
}

int jm_LiquidCrystal_I2C::print_h16( uint16_t value )
{
	return print_h8( (uint8_t)(value>>8) ) + print_h8( (uint8_t)(value>>0) );
}

int jm_LiquidCrystal_I2C::print_h24( uint32_t value )
{
	return print_h8( (uint8_t)(value>>16) ) + print_h16( (uint16_t)(value>>0) );
}

int jm_LiquidCrystal_I2C::print_h32( uint32_t value )
{
	return print_h16( (uint16_t)(value>>16) ) + print_h16( (uint16_t)(value>>0) );
}

// -----------------------------------------------------------------------------

int jm_LiquidCrystal_I2C::print_u32f( uint32_t value, int nfrac )
{
	return print_u32f( value, nfrac, 10+1 );
}

int jm_LiquidCrystal_I2C::print_u32f( uint32_t value, int nfrac, int nprint )
{
	buf_u32f( value, nfrac );
	return buf_rjprint( nprint );
}

// -----------------------------------------------------------------------------

int jm_LiquidCrystal_I2C::print_s32f( int32_t value, int nfrac )
{
	return print_s32f( value, nfrac, 10+1+1 );
}

int jm_LiquidCrystal_I2C::print_s32f( int32_t value, int nfrac, int nprint )
{
	buf_s32f( value, nfrac );
	return buf_rjprint( nprint );
}

// -----------------------------------------------------------------------------

//	END.