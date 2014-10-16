#ifndef _PLANE_H
#error "This file can only be included from Plane.h"
#else

#include "core\Vector.h"
#include "core\SimdUtils.h"


///////////////////////////////////////////////////////////////////////////////

void Plane::setZero()
{
   m_quad = _mm_setzero_ps();
}

///////////////////////////////////////////////////////////////////////////////

void Plane::set( const Vector& vec )
{
   m_quad = vec.m_quad;
}

///////////////////////////////////////////////////////////////////////////////

void Plane::set( const Vector& normal, const FastFloat& distance )
{
   SimdUtils::setXYZ_W( &normal.m_quad, &distance.m_val, &m_quad );
}

///////////////////////////////////////////////////////////////////////////////

void Plane::set( const FastFloat& a, const FastFloat& b, const FastFloat& c, const FastFloat& d )
{
   m_quad = PACK_SIMD( a.m_val, b.m_val, c.m_val, d.m_val );
}

///////////////////////////////////////////////////////////////////////////////

void Plane::operator=( const Plane& rhs )
{
   m_quad = rhs.m_quad;
}

///////////////////////////////////////////////////////////////////////////////

bool Plane::operator==( const Plane& rhs ) const
{
   __m128 cmpResult = _mm_cmpeq_ps( m_quad, rhs.m_quad );
   int vmask = _mm_movemask_ps( cmpResult );
   return vmask == 0xf;
}

///////////////////////////////////////////////////////////////////////////////

bool Plane::operator!=( const Plane& rhs ) const
{
   __m128 cmpResult = _mm_cmpeq_ps( m_quad, rhs.m_quad );
   int vmask = _mm_movemask_ps( cmpResult );
   return vmask == 0x0;
}

///////////////////////////////////////////////////////////////////////////////

float& Plane::operator[]( int idx )
{
   return m_quad.m128_f32[idx];
}

///////////////////////////////////////////////////////////////////////////////

float Plane::operator[]( int idx ) const
{
   return m_quad.m128_f32[idx];
}

///////////////////////////////////////////////////////////////////////////////

const FastFloat Plane::getComponent( uint idx ) const
{
   __m128 comp;
   SimdUtils::getComponent( &m_quad, idx, &comp );
   return *( const FastFloat* )&comp;
}

///////////////////////////////////////////////////////////////////////////////

void Plane::normalize()
{
   __m128 dot;
   SimdUtils::dot<3>( &m_quad, &m_quad, &dot );

   // at this point we have a squared length of the plane normal - so calculate its square root
   __m128 equalsZero = _mm_cmple_ps( dot, _mm_setzero_ps() );
   __m128 sqrtDot = _mm_sqrt_ps( dot );
   __m128 length = _mm_andnot_ps( equalsZero, sqrtDot );

   m_quad = _mm_div_ps( m_quad, length );
}

///////////////////////////////////////////////////////////////////////////////

const FastFloat Plane::dotCoord( const Vector& coord ) const
{
   __m128 vecQuad = coord.m_quad;

   // plane.x * coord.x, plane.y * coord.y, plane.z * coord.z, plane.w * 1
   vecQuad = _mm_and_ps( vecQuad, _MM_DISCARD_W_MASK );
   vecQuad = _mm_or_ps( vecQuad, _MM_SET_W_MASK);

   __m128 dot;
   SimdUtils::dot<4>( &m_quad, &vecQuad, &dot );

   return ( const FastFloat& )dot;
}

///////////////////////////////////////////////////////////////////////////////

const FastFloat Plane::dotNormal( const Vector& normal ) const
{
   __m128 dot;
   SimdUtils::dot<3>( &normal.m_quad, &m_quad, &dot );

   return ( const FastFloat& )dot;
}

///////////////////////////////////////////////////////////////////////////////

void Plane::getNormal( Vector& outPlaneNormal ) const
{
   outPlaneNormal.m_quad = _mm_and_ps( m_quad, _MM_DISCARD_W_MASK );
}

///////////////////////////////////////////////////////////////////////////////

#endif // _PLANE_H
