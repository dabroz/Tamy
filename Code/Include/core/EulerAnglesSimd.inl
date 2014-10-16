#ifndef _EULER_ANGLES_H
#error "This file can be included only in EulerAngles.h"
#else

#include "core\Vector.h"
#include "core\Matrix.h"
#include "core\Quaternion.h"
#include "core\MathDefs.h"
#include "core\SimdUtils.h"
#include "core\FastFloat.h"


///////////////////////////////////////////////////////////////////////////////

void EulerAngles::setZero()
{
   m_quad = _mm_setzero_ps();
}

///////////////////////////////////////////////////////////////////////////////

void EulerAngles::set( const FastFloat& yaw, const FastFloat& pitch, const FastFloat& roll )
{
   m_quad = PACK_SIMD( yaw.m_val, pitch.m_val, roll.m_val, Float_0.m_val );
   normalize();
}

///////////////////////////////////////////////////////////////////////////////

void EulerAngles::set( float yaw, float pitch, float roll )
{
   SimdUtils::toSimd( yaw, pitch, roll, 0.0f, &m_quad );
   normalize();   
}

///////////////////////////////////////////////////////////////////////////////

bool EulerAngles::operator==( const EulerAngles& rhs ) const
{
   __m128 cmpResult = _mm_cmpeq_ps( m_quad, rhs.m_quad );
   int vmask = _mm_movemask_ps( cmpResult );
   return vmask == 0xf;
}

///////////////////////////////////////////////////////////////////////////////

bool EulerAngles::operator!=( const EulerAngles& rhs ) const
{
   __m128 cmpResult = _mm_cmpeq_ps( m_quad, rhs.m_quad );
   int vmask = _mm_movemask_ps( cmpResult );
   return vmask == 0x0;
}

///////////////////////////////////////////////////////////////////////////////

void EulerAngles::operator=( const EulerAngles& rhs )
{
   m_quad = rhs.m_quad;
}

///////////////////////////////////////////////////////////////////////////////

float& EulerAngles::operator[]( int idx )
{
   return m_quad.m128_f32[idx];
}

///////////////////////////////////////////////////////////////////////////////

float EulerAngles::operator[]( int idx ) const
{
   return m_quad.m128_f32[idx];
}

///////////////////////////////////////////////////////////////////////////////

float EulerAngles::getYaw() const
{
   return m_quad.m128_f32[0];
}

///////////////////////////////////////////////////////////////////////////////

float EulerAngles::getPitch() const
{
   return m_quad.m128_f32[1];
}

///////////////////////////////////////////////////////////////////////////////

float EulerAngles::getRoll() const
{
   return m_quad.m128_f32[2];
}

///////////////////////////////////////////////////////////////////////////////

void EulerAngles::setAdd( const EulerAngles& a, const EulerAngles& b )
{
   m_quad = _mm_add_ps( a.m_quad, b.m_quad );
}

///////////////////////////////////////////////////////////////////////////////

void EulerAngles::setSub( const EulerAngles& a, const EulerAngles& b )
{
   m_quad = _mm_sub_ps( a.m_quad, b.m_quad );
}

///////////////////////////////////////////////////////////////////////////////

void EulerAngles::add( const EulerAngles& a )
{
   m_quad = _mm_add_ps( m_quad, a.m_quad );
}

///////////////////////////////////////////////////////////////////////////////

void EulerAngles::sub( const EulerAngles& a )
{
   m_quad = _mm_sub_ps( m_quad, a.m_quad );
}

///////////////////////////////////////////////////////////////////////////////

void EulerAngles::normalized( EulerAngles& outNormalized ) const
{
   outNormalized.m_quad = m_quad;
   outNormalized.normalize();
}

///////////////////////////////////////////////////////////////////////////////

void EulerAngles::normalize()
{
   static __m128 f360 = _mm_set1_ps( 360.0f );

   __m64 numCycles = _mm_cvtps_pi16( _mm_div_ps( m_quad, f360 ) );
   __m128 floatNumCycles = _mm_cvtpi16_ps( numCycles );
   _mm_empty();

   __m128 anglesToSub = _mm_mul_ps( floatNumCycles, f360 );
   m_quad = _mm_sub_ps( m_quad, anglesToSub );
}

///////////////////////////////////////////////////////////////////////////////

#endif // _EULER_ANGLES_H
