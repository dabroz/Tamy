#ifndef _EULER_ANGLES_H
#error "This file can be included only in EulerAngles.h"
#else

#include "core\Vector.h"
#include "core\Matrix.h"
#include "core\Quaternion.h"
#include "core\MathDefs.h"


///////////////////////////////////////////////////////////////////////////////

void EulerAngles::setZero()
{
   memset( m_quad.v, 0, sizeof( float ) * 4 );
}

///////////////////////////////////////////////////////////////////////////////

void EulerAngles::set( const FastFloat& yaw, const FastFloat& pitch, const FastFloat& roll )
{
   m_quad.v[0] = yaw.m_val;
   m_quad.v[1] = pitch.m_val;
   m_quad.v[2] = roll.m_val;
   normalize();
}

///////////////////////////////////////////////////////////////////////////////

void EulerAngles::set( float yaw, float pitch, float roll )
{
   m_quad.v[0] = yaw;
   m_quad.v[1] = pitch;
   m_quad.v[2] = roll;
   normalize();   
}

///////////////////////////////////////////////////////////////////////////////

bool EulerAngles::operator==( const EulerAngles& rhs ) const
{
   return ( m_quad.v[0] == rhs.m_quad.v[0] ) && ( m_quad.v[1] == rhs.m_quad.v[1] ) && ( m_quad.v[2] == rhs.m_quad.v[2] );
}

///////////////////////////////////////////////////////////////////////////////

bool EulerAngles::operator!=( const EulerAngles& rhs ) const
{
   return !( *this == rhs );
}

///////////////////////////////////////////////////////////////////////////////

void EulerAngles::operator=( const EulerAngles& rhs )
{
   m_quad.v[0] = rhs.m_quad.v[0];
   m_quad.v[1] = rhs.m_quad.v[1];
   m_quad.v[2] = rhs.m_quad.v[2];
}

///////////////////////////////////////////////////////////////////////////////

float& EulerAngles::operator[]( int idx )
{
   return m_quad.v[idx];
}

///////////////////////////////////////////////////////////////////////////////

float EulerAngles::operator[]( int idx ) const
{
   return m_quad.v[idx];
}

///////////////////////////////////////////////////////////////////////////////

float EulerAngles::getYaw() const
{
   return m_quad.v[0];
}

///////////////////////////////////////////////////////////////////////////////

float EulerAngles::getPitch() const
{
   return m_quad.v[1];
}

///////////////////////////////////////////////////////////////////////////////

float EulerAngles::getRoll() const
{
   return m_quad.v[2];
}

///////////////////////////////////////////////////////////////////////////////

void EulerAngles::setAdd( const EulerAngles& a, const EulerAngles& b )
{
   m_quad.v[0] = a.m_quad.v[0] + b.m_quad.v[0];
   m_quad.v[1] = a.m_quad.v[1] + b.m_quad.v[1];
   m_quad.v[2] = a.m_quad.v[2] + b.m_quad.v[2];
}

///////////////////////////////////////////////////////////////////////////////

void EulerAngles::setSub( const EulerAngles& a, const EulerAngles& b )
{
   m_quad.v[0] = a.m_quad.v[0] - b.m_quad.v[0];
   m_quad.v[1] = a.m_quad.v[1] - b.m_quad.v[1];
   m_quad.v[2] = a.m_quad.v[2] - b.m_quad.v[2];
}

///////////////////////////////////////////////////////////////////////////////

void EulerAngles::add( const EulerAngles& a )
{
   m_quad.v[0] += a.m_quad.v[0];
   m_quad.v[1] += a.m_quad.v[1];
   m_quad.v[2] += a.m_quad.v[2];
}

///////////////////////////////////////////////////////////////////////////////

void EulerAngles::sub( const EulerAngles& a )
{
   m_quad.v[0] -= a.m_quad.v[0];
   m_quad.v[1] -= a.m_quad.v[1];
   m_quad.v[2] -= a.m_quad.v[2];
}

///////////////////////////////////////////////////////////////////////////////

void EulerAngles::normalized( EulerAngles& outNormalized ) const
{
   memcpy( outNormalized.m_quad.v, m_quad.v, sizeof( QuadStorage ) );
   outNormalized.normalize();
}

///////////////////////////////////////////////////////////////////////////////

void EulerAngles::normalize()
{
   m_quad.v[0] -= (int)(m_quad.v[0] / 360.0f) * 360;
   m_quad.v[1] -= (int)(m_quad.v[1] / 360.0f) * 360;
   m_quad.v[2] -= (int)(m_quad.v[2] / 360.0f) * 360;
}

///////////////////////////////////////////////////////////////////////////////

#endif // _EULER_ANGLES_H
