#ifndef _FAST_FLOAT_H
#error "This file can only be included from FastFloat.h"
#else

#include "core\MathDataStorage.h"
#include "core\SimdUtils.h"
#include <math.h>


///////////////////////////////////////////////////////////////////////////////

void FastFloat::operator=( const FastFloat& rhs )
{
   m_val = rhs.m_val;
}

///////////////////////////////////////////////////////////////////////////////

void FastFloat::operator=( const FastFloatStorage& rhs )
{
   m_val = rhs;
}

///////////////////////////////////////////////////////////////////////////////

void FastFloat::setFromFloat( float val )
{
   m_val = _mm_set1_ps( val );
}

///////////////////////////////////////////////////////////////////////////////

void FastFloat::setFromInt( int val )
{
   m_val = _mm_cvtepi32_ps( _mm_set1_epi32(val) );
}

///////////////////////////////////////////////////////////////////////////////

void FastFloat::setFromByte( byte val )
{
   int intVal = val;
   m_val = _mm_cvtepi32_ps( _mm_set1_epi32( intVal ) );
}

///////////////////////////////////////////////////////////////////////////////

void FastFloat::setFromWord( word val )
{
   int intVal = val;
   m_val = _mm_cvtepi32_ps( _mm_set1_epi32( intVal ) );
}

///////////////////////////////////////////////////////////////////////////////

float FastFloat::getFloat() const
{
   float s;
   _mm_store_ss( &s, m_val );
   return s;
}

///////////////////////////////////////////////////////////////////////////////

void FastFloat::setAdd( const FastFloat& a, const FastFloat& b )
{
   m_val = _mm_add_ps( a.m_val, b.m_val );
}

///////////////////////////////////////////////////////////////////////////////

void FastFloat::setSub( const FastFloat& a, const FastFloat& b )
{
   m_val = _mm_sub_ps( a.m_val, b.m_val );
}

///////////////////////////////////////////////////////////////////////////////

void FastFloat::setMul( const FastFloat& a, const FastFloat& b )
{
   m_val = _mm_mul_ps( a.m_val, b.m_val );
}

///////////////////////////////////////////////////////////////////////////////

void FastFloat::setDiv( const FastFloat& a, const FastFloat& b )
{
   m_val = _mm_div_ps( a.m_val, b.m_val );
}

///////////////////////////////////////////////////////////////////////////////

void FastFloat::add( const FastFloat& a )
{
   m_val = _mm_add_ps( m_val, a.m_val );
}

///////////////////////////////////////////////////////////////////////////////

void FastFloat::sub( const FastFloat& a )
{
   m_val = _mm_sub_ps( m_val, a.m_val );
}

///////////////////////////////////////////////////////////////////////////////

void FastFloat::mul( const FastFloat& a )
{
   m_val = _mm_mul_ps( m_val, a.m_val );
}

///////////////////////////////////////////////////////////////////////////////

void FastFloat::div( const FastFloat& a )
{
   m_val = _mm_div_ps( m_val, a.m_val );
}

///////////////////////////////////////////////////////////////////////////////

FastFloat FastFloat::operator+( const FastFloat& a ) const
{
   __m128 val = _mm_add_ps( m_val, a.m_val );
   return FastFloat( val );
}

///////////////////////////////////////////////////////////////////////////////

FastFloat FastFloat::operator-( const FastFloat& a ) const
{
   __m128 val = _mm_sub_ps( m_val, a.m_val );
   return FastFloat( val );
}

///////////////////////////////////////////////////////////////////////////////

FastFloat FastFloat::operator*( const FastFloat& a ) const
{
   __m128 val = _mm_mul_ps( m_val, a.m_val );
   return FastFloat( val );
}

///////////////////////////////////////////////////////////////////////////////

FastFloat FastFloat::operator/( const FastFloat& a ) const
{
   __m128 val = _mm_div_ps( m_val, a.m_val );
   return FastFloat( val );
}

///////////////////////////////////////////////////////////////////////////////

FastFloat& FastFloat::operator+=( const FastFloat& a )
{
   m_val = _mm_add_ps( m_val, a.m_val );
   return *this;
}

///////////////////////////////////////////////////////////////////////////////

FastFloat& FastFloat::operator-=( const FastFloat& a )
{
   m_val = _mm_sub_ps( m_val, a.m_val );
   return *this;
}

///////////////////////////////////////////////////////////////////////////////

FastFloat& FastFloat::operator*=( const FastFloat& a )
{
   m_val = _mm_mul_ps( m_val, a.m_val );
   return *this;
}

///////////////////////////////////////////////////////////////////////////////

FastFloat& FastFloat::operator/=( const FastFloat& a )
{
   m_val = _mm_div_ps( m_val, a.m_val );
   return *this;
}

///////////////////////////////////////////////////////////////////////////////

FastFloat FastFloat::operator-() const
{
   __m128 val = _mm_xor_ps( m_val, _MM_SIGN_MASK );
   return FastFloat( val );
}

///////////////////////////////////////////////////////////////////////////////

bool FastFloat::operator<( const FastFloat& rhs ) const
{
   return _mm_ucomilt_ss( m_val, rhs.m_val ) == 1;
}

///////////////////////////////////////////////////////////////////////////////

bool FastFloat::operator<=( const FastFloat& rhs ) const
{
   return _mm_ucomile_ss( m_val, rhs.m_val ) == 1;
}

///////////////////////////////////////////////////////////////////////////////

bool FastFloat::operator>( const FastFloat& rhs ) const
{
   return _mm_ucomigt_ss( m_val, rhs.m_val ) == 1;
}

///////////////////////////////////////////////////////////////////////////////

bool FastFloat::operator>=( const FastFloat& rhs ) const
{
   return _mm_ucomige_ss( m_val, rhs.m_val ) == 1;
}

///////////////////////////////////////////////////////////////////////////////

bool FastFloat::operator==( const FastFloat& rhs ) const
{
   return _mm_ucomieq_ss( m_val, rhs.m_val ) == 1;
}

///////////////////////////////////////////////////////////////////////////////

bool FastFloat::operator!=( const FastFloat& rhs ) const
{
   return _mm_ucomineq_ss( m_val, rhs.m_val ) == 1;
}

///////////////////////////////////////////////////////////////////////////////

void FastFloat::setMax( const FastFloat& a, const FastFloat& b )
{
   m_val = _mm_max_ps( a.m_val, b.m_val );
}

///////////////////////////////////////////////////////////////////////////////

void FastFloat::setMin( const FastFloat& a, const FastFloat& b )
{
   m_val = _mm_min_ps( a.m_val, b.m_val );
}

///////////////////////////////////////////////////////////////////////////////

void FastFloat::setClamped( const FastFloat& a, const FastFloat& minVal, const FastFloat& maxVal )
{
   FastFloatStorage mv = _mm_max_ps( a.m_val, minVal.m_val );
   m_val = _mm_min_ps( maxVal.m_val, mv );
}

///////////////////////////////////////////////////////////////////////////////

void FastFloat::setAbs( const FastFloat& a )
{
   m_val = _mm_and_ps( a.m_val, _MM_ABS_VAL_MASK );
}

///////////////////////////////////////////////////////////////////////////////

void FastFloat::abs()
{
   m_val = _mm_and_ps( m_val, _MM_ABS_VAL_MASK );
}

///////////////////////////////////////////////////////////////////////////////

void FastFloat::setFlipSign( const FastFloat& a, const FastFloat& sign )
{
   m_val = _mm_xor_ps( a.m_val, _mm_and_ps( sign.m_val, _MM_SIGN_MASK ) );
}

///////////////////////////////////////////////////////////////////////////////

void FastFloat::setSign( const FastFloat& a )
{
   m_val = _mm_xor_ps( Float_1.m_val, _mm_and_ps( a.m_val, _MM_SIGN_MASK ) );
}

///////////////////////////////////////////////////////////////////////////////

void FastFloat::sign()
{
   m_val = _mm_xor_ps( Float_1.m_val, _mm_and_ps( m_val, _MM_SIGN_MASK ) );
}

///////////////////////////////////////////////////////////////////////////////

bool FastFloat::approxEqual( const FastFloat& a, const FastFloat& eps ) const
{
   __m128 diff = _mm_sub_ps( m_val, a.m_val );
   diff = _mm_and_ps( diff, _MM_ABS_VAL_MASK );
   return _mm_ucomile_ss( diff, eps.m_val ) == 1;
}

///////////////////////////////////////////////////////////////////////////////

void FastFloat::setSqrt( const FastFloat& a )
{
   const __m128 equalsZero = _mm_cmple_ps( a.m_val, _mm_setzero_ps() );
   const __m128 e = _mm_sqrt_ps( a.m_val );
   m_val = _mm_andnot_ps( equalsZero, e );
}

///////////////////////////////////////////////////////////////////////////////

void FastFloat::sqrt()
{
   const __m128 equalsZero = _mm_cmple_ps( m_val, _mm_setzero_ps() );
   const __m128 e = _mm_sqrt_ps( m_val );
   m_val = _mm_andnot_ps( equalsZero, e );
}

///////////////////////////////////////////////////////////////////////////////

void FastFloat::setReciprocal( const FastFloat& a )
{
   m_val = _mm_div_ps( Float_1.m_val, a.m_val );
}

///////////////////////////////////////////////////////////////////////////////

void FastFloat::reciprocal()
{
   m_val = _mm_div_ps( Float_1.m_val, m_val );
}

///////////////////////////////////////////////////////////////////////////////

void FastFloat::setNeg( const FastFloat& a )
{
   m_val = _mm_xor_ps( a.m_val, _MM_SIGN_MASK );
}

///////////////////////////////////////////////////////////////////////////////

void FastFloat::neg()
{
   m_val = _mm_xor_ps( m_val, _MM_SIGN_MASK );
}

///////////////////////////////////////////////////////////////////////////////

void FastFloat::sin()
{
   float v = ::sin( m_val.m128_f32[0] );
   m_val = _mm_set1_ps( v );
}

///////////////////////////////////////////////////////////////////////////////

void FastFloat::cos()
{
   float v = ::cos( m_val.m128_f32[0] );
   m_val = _mm_set1_ps( v );
}

///////////////////////////////////////////////////////////////////////////////

void FastFloat::setSin( const FastFloat& val )
{
   float v = ::sin( val.m_val.m128_f32[0] );
   m_val = _mm_set1_ps( v );
}

///////////////////////////////////////////////////////////////////////////////

void FastFloat::setCos( const FastFloat& val )
{
   float v = ::cos( val.m_val.m128_f32[0] );
   m_val = _mm_set1_ps( v );
}

///////////////////////////////////////////////////////////////////////////////

void FastFloat::asin()
{
   float v = ::asin( m_val.m128_f32[0] );
   m_val = _mm_set1_ps( v );
}

///////////////////////////////////////////////////////////////////////////////

void FastFloat::acos()
{
   float v = ::acos( m_val.m128_f32[0] );
   m_val = _mm_set1_ps( v );
}

///////////////////////////////////////////////////////////////////////////////

void FastFloat::setAsin( const FastFloat& val )
{
   float v = ::asin( val.m_val.m128_f32[0] );
   m_val = _mm_set1_ps( v );
}

///////////////////////////////////////////////////////////////////////////////

void FastFloat::setAcos( const FastFloat& val )
{
   float v = ::acos( val.m_val.m128_f32[0] );
   m_val = _mm_set1_ps( v );
}

///////////////////////////////////////////////////////////////////////////////

void FastFloat::floor()
{
   SimdUtils::floor( &m_val, &m_val );
}

///////////////////////////////////////////////////////////////////////////////

void FastFloat::setFloor( const FastFloat& rhs )
{
   SimdUtils::floor( &rhs.m_val, &m_val );
}

///////////////////////////////////////////////////////////////////////////////

void FastFloat::round()
{
   SimdUtils::round( &m_val, &m_val );
}

///////////////////////////////////////////////////////////////////////////////

void FastFloat::setRound( const FastFloat& rhs )
{
   SimdUtils::round( &rhs.m_val, &m_val );
}

///////////////////////////////////////////////////////////////////////////////

void FastFloat::setFloatRemainder( const FastFloat& dividend, const FastFloat& divisor )
{
   m_val = _mm_div_ps( dividend.m_val, divisor.m_val );
   SimdUtils::round( &m_val, &m_val );
   m_val = _mm_mul_ps( divisor.m_val, m_val );
   m_val = _mm_sub_ps( dividend.m_val, m_val );
}

///////////////////////////////////////////////////////////////////////////////

void FastFloat::floatRemainder( const FastFloat& divisor )
{
   __m128 dividend = m_val;

   m_val = _mm_div_ps( dividend, divisor.m_val );
   SimdUtils::round( &m_val, &m_val );
   m_val = _mm_mul_ps( divisor.m_val, m_val );
   m_val = _mm_sub_ps( dividend, m_val );
}

///////////////////////////////////////////////////////////////////////////////

#endif // _FAST_FLOAT_H
