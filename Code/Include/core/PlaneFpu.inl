#ifndef _PLANE_H
#error "This file can only be included from Plane.h"
#else

#include "core\Vector.h"


///////////////////////////////////////////////////////////////////////////////

void Plane::setZero()
{
   memset( m_quad.v, 0, sizeof( float ) * 4 );
}

///////////////////////////////////////////////////////////////////////////////

void Plane::set( const Vector& vec )
{
   memcpy( m_quad.v, vec.m_quad.v, sizeof( float ) * 4 );
}

///////////////////////////////////////////////////////////////////////////////

void Plane::set( const Vector& normal, const FastFloat& distance )
{
   memcpy( m_quad.v, normal.m_quad.v, sizeof( float ) * 3 );
   m_quad.v[3] = distance.m_val;
}

///////////////////////////////////////////////////////////////////////////////

void Plane::set( const FastFloat& _a, const FastFloat& _b, const FastFloat& _c, const FastFloat& _d )
{
   m_quad.v[0] = _a.m_val;
   m_quad.v[1] = _b.m_val;
   m_quad.v[2] = _c.m_val;
   m_quad.v[3] = _d.m_val;
}

///////////////////////////////////////////////////////////////////////////////

void Plane::operator=( const Plane& rhs )
{
   memcpy( m_quad.v, rhs.m_quad.v, sizeof( float ) * 4 );
}

///////////////////////////////////////////////////////////////////////////////

bool Plane::operator==( const Plane& rhs ) const
{
   int cmpResult = memcmp( m_quad.v, rhs.m_quad.v, sizeof( float ) * 4 );
   return cmpResult == 0;
}

///////////////////////////////////////////////////////////////////////////////

bool Plane::operator!=( const Plane& rhs ) const
{
   int cmpResult = memcmp( m_quad.v, rhs.m_quad.v, sizeof( float ) * 4 );
   return cmpResult != 0;
}

///////////////////////////////////////////////////////////////////////////////

float& Plane::operator[]( int idx )
{
   return m_quad.v[idx];
}

///////////////////////////////////////////////////////////////////////////////

float Plane::operator[]( int idx ) const
{
   return m_quad.v[idx];
}

///////////////////////////////////////////////////////////////////////////////

const FastFloat Plane::getComponent( uint idx ) const
{
   return *( const FastFloat* )&m_quad.v[idx];
}

///////////////////////////////////////////////////////////////////////////////

void Plane::normalize()
{
   float length = sqrt( m_quad.v[0]*m_quad.v[0] + m_quad.v[1]*m_quad.v[1] + m_quad.v[2]*m_quad.v[2] );
   if ( length != 0.0f )
   {
      m_quad.v[0] /= length;
      m_quad.v[1] /= length;
      m_quad.v[2] /= length;
      m_quad.v[3] /= length;
   }
}

///////////////////////////////////////////////////////////////////////////////

const FastFloat Plane::dotCoord( const Vector& coord ) const
{
   FastFloat t;
   t.m_val = coord.m_quad.v[0] * m_quad.v[0] + coord.m_quad.v[1] * m_quad.v[1] + coord.m_quad.v[2] * m_quad.v[2] + m_quad.v[3];
   return t;
}

///////////////////////////////////////////////////////////////////////////////

const FastFloat Plane::dotNormal( const Vector& normal ) const
{
   FastFloat t;
   t.m_val = normal.m_quad.v[0] * m_quad.v[0] + normal.m_quad.v[1] * m_quad.v[1] + normal.m_quad.v[2] * m_quad.v[2];
   return t;
}

///////////////////////////////////////////////////////////////////////////////

void Plane::getNormal( Vector& outPlaneNormal ) const
{
   memcpy( outPlaneNormal.m_quad.v, m_quad.v, sizeof( float ) * 3 );
}

///////////////////////////////////////////////////////////////////////////////

#endif // _PLANE_H
