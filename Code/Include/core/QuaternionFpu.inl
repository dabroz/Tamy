#ifndef _QUATERNION_H
#error "This file can be included only in Quaternion.h"
#else

#include "core\MathDefs.h"
#include "core\Vector.h"
#include "core\Assert.h"


///////////////////////////////////////////////////////////////////////////////

Quaternion::Quaternion()
{
   memcpy( m_quad.v, Quad_0001.v, sizeof( QuadStorage ) );
}

///////////////////////////////////////////////////////////////////////////////

Quaternion::Quaternion( float x, float y, float z, float w )
{
   m_quad.v[0] = x;
   m_quad.v[1] = y;
   m_quad.v[2] = z;
   m_quad.v[3] = w;
}

///////////////////////////////////////////////////////////////////////////////

Quaternion::Quaternion( const QuadStorage& quad )
{
   memcpy( m_quad.v, quad.v, sizeof( QuadStorage ) );
}

///////////////////////////////////////////////////////////////////////////////

void Quaternion::setIdentity()
{
   memcpy( m_quad.v, Quad_0001.v, sizeof( QuadStorage ) );
}

///////////////////////////////////////////////////////////////////////////////

void Quaternion::set( const QuadStorage& quad )
{
   memcpy( m_quad.v, quad.v, sizeof( QuadStorage ) );
}

///////////////////////////////////////////////////////////////////////////////

void Quaternion::set( float x, float y, float z, float w )
{
   m_quad.v[0] = x;
   m_quad.v[1] = y;
   m_quad.v[2] = z;
   m_quad.v[3] = w;
}

///////////////////////////////////////////////////////////////////////////////

void Quaternion::setAxisAngle( const Vector& axis, const FastFloat& angle )
{
   ASSERT_MSG( axis.isNormalized(), "Axis vector is not unit length" );

   float s = sin( angle.m_val/2 );
   float x = axis.m_quad.v[0] * s;
   float y = axis.m_quad.v[1] * s; 
   float z = axis.m_quad.v[2] * s;
   float w = cos( angle.m_val/2 );
   set( x, y, z, w );
}

///////////////////////////////////////////////////////////////////////////////

void Quaternion::operator=( const Quaternion& rhs )
{
   memcpy( m_quad.v, rhs.m_quad.v, sizeof( QuadStorage ) );
}

///////////////////////////////////////////////////////////////////////////////

bool Quaternion::operator==( const Quaternion& rhs ) const
{
   int result = memcmp( m_quad.v, rhs.m_quad.v, sizeof( QuadStorage ) );
   return result == 0;
}

///////////////////////////////////////////////////////////////////////////////

bool Quaternion::operator!=( const Quaternion& rhs ) const
{
   int result = memcmp( m_quad.v, rhs.m_quad.v, sizeof( QuadStorage ) );
   return result != 0;
}

///////////////////////////////////////////////////////////////////////////////

float& Quaternion::operator[]( int idx )
{
   return m_quad.v[idx];
}

///////////////////////////////////////////////////////////////////////////////

float Quaternion::operator[]( int idx ) const
{
   return m_quad.v[idx];
}

///////////////////////////////////////////////////////////////////////////////

void Quaternion::setAdd( const Quaternion& q1, const Quaternion& q2 )
{
   m_quad.v[0] = q1.m_quad.v[0] + q2.m_quad.v[0];
   m_quad.v[1] = q1.m_quad.v[1] + q2.m_quad.v[1];
   m_quad.v[2] = q1.m_quad.v[2] + q2.m_quad.v[2];
   m_quad.v[3] = q1.m_quad.v[3] + q2.m_quad.v[3];
}

///////////////////////////////////////////////////////////////////////////////

void Quaternion::setSub( const Quaternion& q1, const Quaternion& q2 )
{
   m_quad.v[0] = q1.m_quad.v[0] - q2.m_quad.v[0];
   m_quad.v[1] = q1.m_quad.v[1] - q2.m_quad.v[1];
   m_quad.v[2] = q1.m_quad.v[2] - q2.m_quad.v[2];
   m_quad.v[3] = q1.m_quad.v[3] - q2.m_quad.v[3];
}

///////////////////////////////////////////////////////////////////////////////

void Quaternion::setMul( const Quaternion& q1, const Quaternion& q2 )
{
   // x  =  x2 * w1  +  w2 * x1  +  y2 * z1  -  z2 * y1
   // y  =  w2 * y1  +  y2 * w1  -  x2 * z1  +  z2 * x1
   // z  =  w2 * z1  +  z2 * w1  +  x2 * y1  -  y2 * x1
   // w  =  w2 * w1  - 
   m_quad.v[0] = q2.m_quad.v[0]*q1.m_quad.v[3] + q2.m_quad.v[3]*q1.m_quad.v[0] + q2.m_quad.v[1]*q1.m_quad.v[2] - q2.m_quad.v[2]*q1.m_quad.v[1];
   m_quad.v[1] = q2.m_quad.v[3]*q1.m_quad.v[1] - q2.m_quad.v[0]*q1.m_quad.v[2] + q2.m_quad.v[1]*q1.m_quad.v[3] + q2.m_quad.v[2]*q1.m_quad.v[0];
   m_quad.v[2] = q2.m_quad.v[3]*q1.m_quad.v[2] + q2.m_quad.v[0]*q1.m_quad.v[1] - q2.m_quad.v[1]*q1.m_quad.v[0] + q2.m_quad.v[2]*q1.m_quad.v[3];
   m_quad.v[3] = q2.m_quad.v[3]*q1.m_quad.v[3] - q2.m_quad.v[0]*q1.m_quad.v[0] - q2.m_quad.v[1]*q1.m_quad.v[1] - q2.m_quad.v[2]*q1.m_quad.v[2];
}

///////////////////////////////////////////////////////////////////////////////

void Quaternion::add( const Quaternion& rhs )
{
   m_quad.v[0] += rhs.m_quad.v[0];
   m_quad.v[1] += rhs.m_quad.v[1];
   m_quad.v[2] += rhs.m_quad.v[2];
   m_quad.v[3] += rhs.m_quad.v[3];
}

///////////////////////////////////////////////////////////////////////////////

void Quaternion::sub( const Quaternion& rhs )
{
   m_quad.v[0] -= rhs.m_quad.v[0];
   m_quad.v[1] -= rhs.m_quad.v[1];
   m_quad.v[2] -= rhs.m_quad.v[2];
   m_quad.v[3] -= rhs.m_quad.v[3];
}

///////////////////////////////////////////////////////////////////////////////

void Quaternion::mul( const Quaternion& rhs )
{
   float x = rhs.m_quad.v[0]*m_quad.v[3] + rhs.m_quad.v[3]*m_quad.v[0] + rhs.m_quad.v[1]*m_quad.v[2] - rhs.m_quad.v[2]*m_quad.v[1];
   float y = rhs.m_quad.v[3]*m_quad.v[1] - rhs.m_quad.v[0]*m_quad.v[2] + rhs.m_quad.v[1]*m_quad.v[3] + rhs.m_quad.v[2]*m_quad.v[0];
   float z = rhs.m_quad.v[3]*m_quad.v[2] + rhs.m_quad.v[0]*m_quad.v[1] - rhs.m_quad.v[1]*m_quad.v[0] + rhs.m_quad.v[2]*m_quad.v[3];
   float w = rhs.m_quad.v[3]*m_quad.v[3] - rhs.m_quad.v[0]*m_quad.v[0] - rhs.m_quad.v[1]*m_quad.v[1] - rhs.m_quad.v[2]*m_quad.v[2];

   m_quad.v[0] = x;
   m_quad.v[1] = y;
   m_quad.v[2] = z;
   m_quad.v[3] = w;
}

///////////////////////////////////////////////////////////////////////////////

void Quaternion::neg()
{
   m_quad.v[0] = -m_quad.v[0];
   m_quad.v[1] = -m_quad.v[1];
   m_quad.v[2] = -m_quad.v[2];
   m_quad.v[3] = -m_quad.v[3];
}

///////////////////////////////////////////////////////////////////////////////

void Quaternion::setConjugated( const Quaternion& rhs )
{
   m_quad.v[0] = -rhs.m_quad.v[0];
   m_quad.v[1] = -rhs.m_quad.v[1];
   m_quad.v[2] = -rhs.m_quad.v[2];
   m_quad.v[3] =  rhs.m_quad.v[3];
}

///////////////////////////////////////////////////////////////////////////////

void Quaternion::conjugate()
{
   m_quad.v[0] = -m_quad.v[0];
   m_quad.v[1] = -m_quad.v[1];
   m_quad.v[2] = -m_quad.v[2];
}

///////////////////////////////////////////////////////////////////////////////

void Quaternion::normalize()
{
   float len = sqrt( m_quad.v[0]*m_quad.v[0] + m_quad.v[1]*m_quad.v[1] + m_quad.v[2]*m_quad.v[2] + m_quad.v[3]*m_quad.v[3] );
   if ( len > 0.0f )
   {
      m_quad.v[0] /= len;
      m_quad.v[1] /= len;
      m_quad.v[2] /= len;
      m_quad.v[3] /= len;
   }
}

///////////////////////////////////////////////////////////////////////////////

void Quaternion::setSlerp( const Quaternion& a, const Quaternion& b, const FastFloat& t )
{
   // Calculate angle between them.
   double cosHalfTheta = a.m_quad.v[3] * b.m_quad.v[3] + a.m_quad.v[0] * b.m_quad.v[0] + a.m_quad.v[1] * b.m_quad.v[1] + a.m_quad.v[2] * b.m_quad.v[2];

   // if a=b or a=-b then theta = 0 and we can return a
   if ( abs( cosHalfTheta ) >= 1.0 )
   {
      m_quad.v[3] = a.m_quad.v[3];
      m_quad.v[0] = a.m_quad.v[0];
      m_quad.v[1] = a.m_quad.v[1];
      m_quad.v[2] = a.m_quad.v[2];
      return;
   }

   // Calculate temporary values.
   double halfTheta = acos( cosHalfTheta );
   double sinHalfTheta = sqrt( 1.0 - cosHalfTheta*cosHalfTheta );

   // if theta = 180 degrees then result is not fully defined
   // we could rotate around any axis normal to a or b
   if ( fabs( sinHalfTheta ) < 0.001 )
   { 
      // fabs is floating point absolute
      m_quad.v[3] = (float)( a.m_quad.v[3] * 0.5 + b.m_quad.v[3] * 0.5 );
      m_quad.v[0] = (float)( a.m_quad.v[0] * 0.5 + b.m_quad.v[0] * 0.5 );
      m_quad.v[1] = (float)( a.m_quad.v[1] * 0.5 + b.m_quad.v[1] * 0.5 );
      m_quad.v[2] = (float)( a.m_quad.v[2] * 0.5 + b.m_quad.v[2] * 0.5 );

      return;
   }

   double ratioA = sin( ( 1 - t.m_val ) * halfTheta ) / sinHalfTheta;
   double ratioB = sin( t.m_val * halfTheta ) / sinHalfTheta; 

   m_quad.v[3] = (float)( a.m_quad.v[3] * ratioA + b.m_quad.v[3] * ratioB );
   m_quad.v[0] = (float)( a.m_quad.v[0] * ratioA + b.m_quad.v[0] * ratioB );
   m_quad.v[1] = (float)( a.m_quad.v[1] * ratioA + b.m_quad.v[1] * ratioB );
   m_quad.v[2] = (float)( a.m_quad.v[2] * ratioA + b.m_quad.v[2] * ratioB );
}

///////////////////////////////////////////////////////////////////////////////

void Quaternion::setNlerp( const Quaternion& a, const Quaternion& b, const FastFloat& t )
{
   m_quad.v[0] = a.m_quad.v[0] + ( b.m_quad.v[0] - a.m_quad.v[0] ) * t.m_val;
   m_quad.v[1] = a.m_quad.v[1] + ( b.m_quad.v[1] - a.m_quad.v[1] ) * t.m_val;
   m_quad.v[2] = a.m_quad.v[2] + ( b.m_quad.v[2] - a.m_quad.v[2] ) * t.m_val;
   m_quad.v[3] = a.m_quad.v[3] + ( b.m_quad.v[3] - a.m_quad.v[3] ) * t.m_val;
}

///////////////////////////////////////////////////////////////////////////////

void Quaternion::setAddScaledVector( const Quaternion& a, const Vector& v, const FastFloat& t )
{
   Quaternion q;
   q.set( v.m_quad.v[0] * t.m_val, v.m_quad.v[1] * t.m_val, v.m_quad.v[2] * t.m_val, 0.0f );
   q.mul( a );

   memcpy( m_quad.v, a.m_quad.v, sizeof( m_quad.v ) );
   m_quad.v[0] += q.m_quad.v[0] * 0.5f;
   m_quad.v[1] += q.m_quad.v[1] * 0.5f;
   m_quad.v[2] += q.m_quad.v[2] * 0.5f;
   m_quad.v[3] += q.m_quad.v[3] * 0.5f;

   normalize();
}

///////////////////////////////////////////////////////////////////////////////

void Quaternion::addScaledVector( const Vector& v, const FastFloat& t )
{
   Quaternion q;
   q.set( v.m_quad.v[0] * t.m_val, v.m_quad.v[1] * t.m_val, v.m_quad.v[2] * t.m_val, 0.0f );
   q.mul( *this );

   m_quad.v[0] += q.m_quad.v[0] * 0.5f;
   m_quad.v[1] += q.m_quad.v[1] * 0.5f;
   m_quad.v[2] += q.m_quad.v[2] * 0.5f;
   m_quad.v[3] += q.m_quad.v[3] * 0.5f;

   normalize();
}

///////////////////////////////////////////////////////////////////////////////

void Quaternion::getAxis( Vector& outAxis ) const
{
   float sq = (float)sqrt( 1.0f - m_quad.v[3]*m_quad.v[3] );
   if ( sq != 0.0f )
   {
      outAxis.m_quad.v[0] = m_quad.v[0] / sq;
      outAxis.m_quad.v[1] = m_quad.v[1] / sq;
      outAxis.m_quad.v[2] = m_quad.v[2] / sq;
      outAxis.normalize();
   }
}

///////////////////////////////////////////////////////////////////////////////

void Quaternion::getAngle( FastFloat& outAngle ) const
{
   outAngle.m_val = 2.0f * (float)acos( m_quad.v[3] );
}

///////////////////////////////////////////////////////////////////////////////

void Quaternion::transform( const Vector& inVec, Vector& outVec ) const
{
   const float x = m_quad.v[0];
   const float y = m_quad.v[1];
   const float z = m_quad.v[2];
   const float w = m_quad.v[3];
   const float vX = inVec.m_quad.v[0];
   const float vY = inVec.m_quad.v[1];
   const float vZ = inVec.m_quad.v[2];
   const float vW = inVec.m_quad.v[3];

   const float x2 = x*x;
   const float y2 = y*y;
   const float z2 = z*z;
   const float w2 = w*w;

   outVec.m_quad.v[0] =  w2*vX    + 2*y*w*vZ  - 2*z*w*vY  + x2*vX     + 2*y*x*vY  + 2*z*x*vZ  - z2*vX     - y2*vX;
   outVec.m_quad.v[1] = 2*x*y*vX  + y2*vY     + 2*z*y*vZ  + 2*w*z*vX  - z2*vY     + w2*vY     - 2*x*w*vZ  - x2*vY;
   outVec.m_quad.v[2] = 2*x*z*vX  + 2*y*z*vY  + z2*vZ     - 2*w*y*vX  - y2*vZ     + 2*w*x*vY  - x2*vZ     + w2*vZ;
}

///////////////////////////////////////////////////////////////////////////////

void Quaternion::store( float* arrFloats ) const
{
   memcpy( arrFloats, m_quad.v, sizeof( QuadStorage ) );
}

///////////////////////////////////////////////////////////////////////////////

void Quaternion::load( const float* arrFloats )
{
   memcpy( m_quad.v, arrFloats, sizeof( QuadStorage ) );
}

///////////////////////////////////////////////////////////////////////////////

#endif // _QUATERNION_H
