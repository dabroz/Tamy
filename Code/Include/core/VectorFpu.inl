#ifndef _VECTOR_H
#error "This file can only be included from Vector.h"
#else

#include "core\FastFloat.h"
#include "core\Algorithms.h"


///////////////////////////////////////////////////////////////////////////////

Vector::Vector() 
{
   memset( m_quad.v, 0, sizeof( QuadStorage ) );
}

///////////////////////////////////////////////////////////////////////////////

Vector::Vector( const QuadStorage& quad )
{
   memcpy( &m_quad, &quad, sizeof( QuadStorage ) );
}

///////////////////////////////////////////////////////////////////////////////

Vector::Vector( float x, float y, float z, float w )
{
   m_quad.v[0] = x;
   m_quad.v[1] = y;
   m_quad.v[2] = z;
   m_quad.v[3] = w;
}

///////////////////////////////////////////////////////////////////////////////

void Vector::setZero()
{
   memset( m_quad.v, 0, sizeof( QuadStorage ) );
}

///////////////////////////////////////////////////////////////////////////////

void Vector::set( const QuadStorage& quad )
{
   memcpy( &m_quad, &quad, sizeof( QuadStorage ) );
}


///////////////////////////////////////////////////////////////////////////////

void Vector::set( float x, float y, float z, float w )
{
   m_quad.v[0] = x;
   m_quad.v[1] = y;
   m_quad.v[2] = z;
   m_quad.v[3] = w;
}

///////////////////////////////////////////////////////////////////////////////

void Vector::set( float* coords )
{
   memcpy( &m_quad, coords, sizeof( float ) * 4 );
}

///////////////////////////////////////////////////////////////////////////////

void Vector::set( const FastFloat& x, const FastFloat& y, const FastFloat& z, const FastFloat& w )
{
   m_quad.v[0] = x.m_val;
   m_quad.v[1] = y.m_val;
   m_quad.v[2] = z.m_val;
   m_quad.v[3] = w.m_val;
}

///////////////////////////////////////////////////////////////////////////////

void Vector::setBroadcast( const FastFloat& val )
{
   m_quad.v[0] = val.m_val;
   m_quad.v[1] = val.m_val;
   m_quad.v[2] = val.m_val;
   m_quad.v[3] = val.m_val;
}

///////////////////////////////////////////////////////////////////////////////

bool Vector::equals4( const Vector& rhs ) const
{
   return memcmp( m_quad.v, rhs.m_quad.v, sizeof( QuadStorage ) ) == 0;
}

///////////////////////////////////////////////////////////////////////////////

bool Vector::operator==( const Vector& rhs ) const
{
   return memcmp( m_quad.v, rhs.m_quad.v, sizeof( float ) * 3 ) == 0;
}

///////////////////////////////////////////////////////////////////////////////

bool Vector::operator!=( const Vector& rhs ) const
{
   return memcmp( m_quad.v, rhs.m_quad.v, sizeof( float ) * 3 ) != 0;
}

///////////////////////////////////////////////////////////////////////////////

void Vector::operator=( const Vector& rhs )
{
   memcpy( &m_quad, &rhs.m_quad, sizeof( QuadStorage ) );
}

///////////////////////////////////////////////////////////////////////////////

void Vector::setMul( const Vector& vec, const FastFloat& t )
{
   m_quad.v[0] = vec.m_quad.v[0] * t.m_val;
   m_quad.v[1] = vec.m_quad.v[1] * t.m_val;
   m_quad.v[2] = vec.m_quad.v[2] * t.m_val;
   m_quad.v[3] = vec.m_quad.v[3] * t.m_val;
}

///////////////////////////////////////////////////////////////////////////////

void Vector::setMul( const Vector& vec1, const Vector& vec2 )
{
   m_quad.v[0] = vec1.m_quad.v[0] * vec2.m_quad.v[0];
   m_quad.v[1] = vec1.m_quad.v[1] * vec2.m_quad.v[1];
   m_quad.v[2] = vec1.m_quad.v[2] * vec2.m_quad.v[2];
   m_quad.v[3] = vec1.m_quad.v[3] * vec2.m_quad.v[3];
}

///////////////////////////////////////////////////////////////////////////////

void Vector::setMulAdd( const Vector& vec1, const FastFloat& t, const Vector& vec2 )
{
   m_quad.v[0] = vec1.m_quad.v[0] * t.m_val + vec2.m_quad.v[0];
   m_quad.v[1] = vec1.m_quad.v[1] * t.m_val + vec2.m_quad.v[1];
   m_quad.v[2] = vec1.m_quad.v[2] * t.m_val + vec2.m_quad.v[2];
   m_quad.v[3] = vec1.m_quad.v[3] * t.m_val + vec2.m_quad.v[3];
}

///////////////////////////////////////////////////////////////////////////////

void Vector::setMulAdd( const Vector& vec1, const Vector& t, const Vector& vec2 )
{
   m_quad.v[0] = vec1.m_quad.v[0] * t.m_quad.v[0] + vec2.m_quad.v[0];
   m_quad.v[1] = vec1.m_quad.v[1] * t.m_quad.v[1] + vec2.m_quad.v[1];
   m_quad.v[2] = vec1.m_quad.v[2] * t.m_quad.v[2] + vec2.m_quad.v[2];
   m_quad.v[3] = vec1.m_quad.v[3] * t.m_quad.v[3] + vec2.m_quad.v[3];
}

///////////////////////////////////////////////////////////////////////////////

void Vector::setDiv( const Vector& vec1, const Vector& vec2 )
{
   m_quad.v[0] = vec1.m_quad.v[0] / vec2.m_quad.v[0];
   m_quad.v[1] = vec1.m_quad.v[1] / vec2.m_quad.v[1];
   m_quad.v[2] = vec1.m_quad.v[2] / vec2.m_quad.v[2];
   m_quad.v[3] = vec1.m_quad.v[3] / vec2.m_quad.v[3];
}

///////////////////////////////////////////////////////////////////////////////

void Vector::setDiv( const Vector& vec1, const FastFloat& t )
{
   m_quad.v[0] = vec1.m_quad.v[0] / t.m_val;
   m_quad.v[1] = vec1.m_quad.v[1] / t.m_val;
   m_quad.v[2] = vec1.m_quad.v[2] / t.m_val;
   m_quad.v[3] = vec1.m_quad.v[3] / t.m_val;
}

///////////////////////////////////////////////////////////////////////////////

void Vector::setAdd( const Vector& vec1, const Vector& vec2 )
{
   m_quad.v[0] = vec1.m_quad.v[0] + vec2.m_quad.v[0];
   m_quad.v[1] = vec1.m_quad.v[1] + vec2.m_quad.v[1];
   m_quad.v[2] = vec1.m_quad.v[2] + vec2.m_quad.v[2];
   m_quad.v[3] = vec1.m_quad.v[3] + vec2.m_quad.v[3];
}

///////////////////////////////////////////////////////////////////////////////

void Vector::setSub( const Vector& vec1, const Vector& vec2 )
{
   m_quad.v[0] = vec1.m_quad.v[0] - vec2.m_quad.v[0];
   m_quad.v[1] = vec1.m_quad.v[1] - vec2.m_quad.v[1];
   m_quad.v[2] = vec1.m_quad.v[2] - vec2.m_quad.v[2];
   m_quad.v[3] = vec1.m_quad.v[3] - vec2.m_quad.v[3];
}

///////////////////////////////////////////////////////////////////////////////

void Vector::mul( const FastFloat& t )
{
   m_quad.v[0] *= t.m_val;
   m_quad.v[1] *= t.m_val;
   m_quad.v[2] *= t.m_val;
   m_quad.v[3] *= t.m_val;
}

///////////////////////////////////////////////////////////////////////////////

void Vector::mul( const Vector& vec )
{
   m_quad.v[0] *= vec.m_quad.v[0];
   m_quad.v[1] *= vec.m_quad.v[1];
   m_quad.v[2] *= vec.m_quad.v[2];
   m_quad.v[3] *= vec.m_quad.v[3];
}

///////////////////////////////////////////////////////////////////////////////

void Vector::div( const FastFloat& t )
{
   m_quad.v[0] /= t.m_val;
   m_quad.v[1] /= t.m_val;
   m_quad.v[2] /= t.m_val;
   m_quad.v[3] /= t.m_val;
}

///////////////////////////////////////////////////////////////////////////////

void Vector::div( const Vector& vec )
{
   m_quad.v[0] /= vec.m_quad.v[0];
   m_quad.v[1] /= vec.m_quad.v[1];
   m_quad.v[2] /= vec.m_quad.v[2];
   m_quad.v[3] /= vec.m_quad.v[3];
}

///////////////////////////////////////////////////////////////////////////////

void Vector::add( const Vector& vec )
{
   m_quad.v[0] += vec.m_quad.v[0];
   m_quad.v[1] += vec.m_quad.v[1];
   m_quad.v[2] += vec.m_quad.v[2];
   m_quad.v[3] += vec.m_quad.v[3];
}

///////////////////////////////////////////////////////////////////////////////

void Vector::sub( const Vector& vec )
{
   m_quad.v[0] -= vec.m_quad.v[0];
   m_quad.v[1] -= vec.m_quad.v[1];
   m_quad.v[2] -= vec.m_quad.v[2];
   m_quad.v[3] -= vec.m_quad.v[3];
}

///////////////////////////////////////////////////////////////////////////////

void Vector::neg()
{
   m_quad.v[0] = -m_quad.v[0];
   m_quad.v[1] = -m_quad.v[1];
   m_quad.v[2] = -m_quad.v[2];
   m_quad.v[3] = -m_quad.v[3];
}

///////////////////////////////////////////////////////////////////////////////

const FastFloat Vector::dot( const Vector& rhs ) const
{
   return m_quad.v[0] * rhs.m_quad.v[0] + m_quad.v[1] * rhs.m_quad.v[1] + m_quad.v[2] * rhs.m_quad.v[2];
}

///////////////////////////////////////////////////////////////////////////////

const FastFloat Vector::dot4( const Vector& rhs ) const
{
   return m_quad.v[0] * rhs.m_quad.v[0] + m_quad.v[1] * rhs.m_quad.v[1] + m_quad.v[2] * rhs.m_quad.v[2] + m_quad.v[3] * rhs.m_quad.v[3];
}

///////////////////////////////////////////////////////////////////////////////

void Vector::setCross( const Vector& v1, const Vector& v2 )
{
   m_quad.v[0] = v1.m_quad.v[1] * v2.m_quad.v[2] - v1.m_quad.v[2] * v2.m_quad.v[1];
   m_quad.v[1] = v1.m_quad.v[2] * v2.m_quad.v[0] - v1.m_quad.v[0] * v2.m_quad.v[2];
   m_quad.v[2] = v1.m_quad.v[0] * v2.m_quad.v[1] - v1.m_quad.v[1] * v2.m_quad.v[0];
   m_quad.v[3] = 1.0f;
}

///////////////////////////////////////////////////////////////////////////////

void Vector::preCross( const Vector& rhs )
{
   Vector tmpVec;
   tmpVec.setCross( *this, rhs );
}

///////////////////////////////////////////////////////////////////////////////

void Vector::postCross( const Vector& rhs )
{
   Vector tmpVec;
   tmpVec.setCross( rhs, *this );
}

///////////////////////////////////////////////////////////////////////////////

void Vector::setNormalized( const Vector& rhs )
{
   setNormalized( rhs.m_quad.v[0], rhs.m_quad.v[1], rhs.m_quad.v[2] );
}

///////////////////////////////////////////////////////////////////////////////

void Vector::setNormalized( float x, float y, float z )
{
   float len = sqrt( x * x + y * y + z * z );
   if (len == 0)
   {
      memset( m_quad.v, 0, sizeof( QuadStorage ) );
   }
   else
   {
      m_quad.v[0] = x / len;
      m_quad.v[1] = y / len;
      m_quad.v[2] = z / len;
      m_quad.v[3] = 0.0f;
   }
}

///////////////////////////////////////////////////////////////////////////////

void Vector::setNormalized( const FastFloat& x, const FastFloat& y, const FastFloat& z )
{
   setNormalized( x.m_val, y.m_val, z.m_val );
}

///////////////////////////////////////////////////////////////////////////////

void Vector::normalize()
{

   float len = sqrt( m_quad.v[0] * m_quad.v[0] + m_quad.v[1] * m_quad.v[1] + m_quad.v[2] * m_quad.v[2] );
   if (len == 0)
   {
      memset( m_quad.v, 0, sizeof( float ) * 3 );
   }
   else
   {
      m_quad.v[0] /= len;
      m_quad.v[1] /= len;
      m_quad.v[2] /= len;
   }
}

///////////////////////////////////////////////////////////////////////////////

void Vector::floor()
{
   m_quad.v[0] = floorf( m_quad.v[0] );
   m_quad.v[1] = floorf( m_quad.v[1] );
   m_quad.v[2] = floorf( m_quad.v[2] );
   m_quad.v[3] = floorf( m_quad.v[3] );
}

///////////////////////////////////////////////////////////////////////////////

void Vector::setFloor( const Vector& rhs )
{
   m_quad.v[0] = floorf( rhs.m_quad.v[0] );
   m_quad.v[1] = floorf( rhs.m_quad.v[1] );
   m_quad.v[2] = floorf( rhs.m_quad.v[2] );
   m_quad.v[3] = floorf( rhs.m_quad.v[3] );
}

///////////////////////////////////////////////////////////////////////////////

void Vector::round()
{
   m_quad.v[0] = roundf( m_quad.v[0] );
   m_quad.v[1] = roundf( m_quad.v[1] );
   m_quad.v[2] = roundf( m_quad.v[2] );
   m_quad.v[3] = roundf( m_quad.v[3] );
}

///////////////////////////////////////////////////////////////////////////////

void Vector::setRound( const Vector& rhs )
{
   m_quad.v[0] = roundf( rhs.m_quad.v[0] );
   m_quad.v[1] = roundf( rhs.m_quad.v[1] );
   m_quad.v[2] = roundf( rhs.m_quad.v[2] );
   m_quad.v[3] = roundf( rhs.m_quad.v[3] );
}

///////////////////////////////////////////////////////////////////////////////

void Vector::setLerp( const Vector& a, const Vector& b, const FastFloat& t )
{
   m_quad.v[0] = a.m_quad.v[0] + ( b.m_quad.v[0] - a.m_quad.v[0] ) * t.m_val;
   m_quad.v[1] = a.m_quad.v[1] + ( b.m_quad.v[1] - a.m_quad.v[1] ) * t.m_val;
   m_quad.v[2] = a.m_quad.v[2] + ( b.m_quad.v[2] - a.m_quad.v[2] ) * t.m_val;
   m_quad.v[3] = a.m_quad.v[3] + ( b.m_quad.v[3] - a.m_quad.v[3] ) * t.m_val;
}

///////////////////////////////////////////////////////////////////////////////

void Vector::setFloatRemainder( const Vector& dividend, const FastFloat& divisor )
{
   m_quad.v[0] = dividend.v[0] - ( divisor.m_val * roundf( dividend.v[0] / divisor.m_val ) );
   m_quad.v[1] = dividend.v[1] - ( divisor.m_val * roundf( dividend.v[1] / divisor.m_val ) );
   m_quad.v[2] = dividend.v[2] - ( divisor.m_val * roundf( dividend.v[2] / divisor.m_val ) );
   m_quad.v[3] = dividend.v[3] - ( divisor.m_val * roundf( dividend.v[3] / divisor.m_val ) );
}

///////////////////////////////////////////////////////////////////////////////

void Vector::setFloatRemainder( const Vector& dividend, const Vector& divisor )
{
   m_quad.v[0] = dividend.v[0] - ( divisor.v[0] * roundf( dividend.v[0] / divisor.v[0] ) );
   m_quad.v[1] = dividend.v[1] - ( divisor.v[1] * roundf( dividend.v[1] / divisor.v[1] ) );
   m_quad.v[2] = dividend.v[2] - ( divisor.v[2] * roundf( dividend.v[2] / divisor.v[2] ) );
   m_quad.v[3] = dividend.v[3] - ( divisor.v[3] * roundf( dividend.v[3] / divisor.v[3] ) );
}

///////////////////////////////////////////////////////////////////////////////

void Vector::floatRemainder( const FastFloat& divisor )
{
   m_quad.v[0] = m_quad.v[0] - ( divisor.m_val * roundf( m_quad.v[0] / divisor.m_val ) );
   m_quad.v[1] = m_quad.v[1] - ( divisor.m_val * roundf( m_quad.v[1] / divisor.m_val ) );
   m_quad.v[2] = m_quad.v[2] - ( divisor.m_val * roundf( m_quad.v[2] / divisor.m_val ) );
   m_quad.v[3] = m_quad.v[3] - ( divisor.m_val * roundf( m_quad.v[3] / divisor.m_val ) );
}

///////////////////////////////////////////////////////////////////////////////

void Vector::floatRemainder( const Vector& divisor )
{
   m_quad.v[0] = m_quad.v[0] - ( divisor.v[0] * roundf( m_quad.v[0] / divisor.v[0] ) );
   m_quad.v[1] = m_quad.v[1] - ( divisor.v[1] * roundf( m_quad.v[1] / divisor.v[1] ) );
   m_quad.v[2] = m_quad.v[2] - ( divisor.v[2] * roundf( m_quad.v[2] / divisor.v[2] ) );
   m_quad.v[3] = m_quad.v[3] - ( divisor.v[3] * roundf( m_quad.v[3] / divisor.v[3] ) );
}

///////////////////////////////////////////////////////////////////////////////

const FastFloat Vector::length() const
{
   float val = sqrt( m_quad.v[0] * m_quad.v[0] + m_quad.v[1] * m_quad.v[1] + m_quad.v[2] * m_quad.v[2] );
   return *( const FastFloat* )&val;
}

///////////////////////////////////////////////////////////////////////////////

const FastFloat Vector::lengthSq() const
{
   float val = m_quad.v[0] * m_quad.v[0] + m_quad.v[1] * m_quad.v[1] + m_quad.v[2] * m_quad.v[2];
   return *( const FastFloat* )&val;
}

///////////////////////////////////////////////////////////////////////////////

void Vector::setMax( const Vector& a, const Vector& b )
{
   m_quad.v[0] = max2( a.m_quad.v[0], b.m_quad.v[0] );
   m_quad.v[1] = max2( a.m_quad.v[1], b.m_quad.v[1] );
   m_quad.v[2] = max2( a.m_quad.v[2], b.m_quad.v[2] );
   m_quad.v[3] = max2( a.m_quad.v[3], b.m_quad.v[3] );
}

///////////////////////////////////////////////////////////////////////////////

void Vector::setMin( const Vector& a, const Vector& b )
{
   m_quad.v[0] = min2( a.m_quad.v[0], b.m_quad.v[0] );
   m_quad.v[1] = min2( a.m_quad.v[1], b.m_quad.v[1] );
   m_quad.v[2] = min2( a.m_quad.v[2], b.m_quad.v[2] );
   m_quad.v[3] = min2( a.m_quad.v[3], b.m_quad.v[3] );
}

///////////////////////////////////////////////////////////////////////////////

void Vector::setClamped( const Vector& a, const Vector& minVal, const Vector& maxVal )
{
   float x = max2( a.m_quad.v[0], minVal.m_quad.v[0] );
   float y = max2( a.m_quad.v[1], minVal.m_quad.v[1] );
   float z = max2( a.m_quad.v[2], minVal.m_quad.v[2] );
   float w = max2( a.m_quad.v[3], minVal.m_quad.v[3] );

   m_quad.v[0] = min2( x, maxVal.m_quad.v[0] );
   m_quad.v[1] = min2( y, maxVal.m_quad.v[1] );
   m_quad.v[2] = min2( z, maxVal.m_quad.v[2] );
   m_quad.v[3] = min2( w, maxVal.m_quad.v[3] );
}

///////////////////////////////////////////////////////////////////////////////

void Vector::setAbs( const Vector& rhs )
{
   m_quad.v[0] = fabs( rhs.m_quad.v[0] );
   m_quad.v[1] = fabs( rhs.m_quad.v[1] );
   m_quad.v[2] = fabs( rhs.m_quad.v[2] );
   m_quad.v[3] = fabs( rhs.m_quad.v[3] );
}

///////////////////////////////////////////////////////////////////////////////

void Vector::abs()
{
   m_quad.v[0] = fabs( m_quad.v[0] );
   m_quad.v[1] = fabs( m_quad.v[1] );
   m_quad.v[2] = fabs( m_quad.v[2] );
   m_quad.v[3] = fabs( m_quad.v[3] );
}

///////////////////////////////////////////////////////////////////////////////

void Vector::setReflected( const Vector& incomingVector, const Vector& reflectionNormal )
{
   float dot2 = 2.0f * ( incomingVector.m_quad.v[0] * reflectionNormal.m_quad.v[0]
      + incomingVector.m_quad.v[1] * reflectionNormal.m_quad.v[1]
      + incomingVector.m_quad.v[2] * reflectionNormal.m_quad.v[2] );

   m_quad.v[0] = incomingVector.m_quad.v[0] - dot2 * reflectionNormal.m_quad.v[0];
   m_quad.v[1] = incomingVector.m_quad.v[1] - dot2 * reflectionNormal.m_quad.v[1];
   m_quad.v[2] = incomingVector.m_quad.v[2] - dot2 * reflectionNormal.m_quad.v[2];

   m_quad.v[3] = incomingVector.m_quad.v[3]; // copy the value of the last component
}

///////////////////////////////////////////////////////////////////////////////

void Vector::reflect( const Vector& reflectionNormal )
{
   float dot2 = 2.0f * ( m_quad.v[0] * reflectionNormal.m_quad.v[0]
                         + m_quad.v[1] * reflectionNormal.m_quad.v[1]
                         + m_quad.v[2] * reflectionNormal.m_quad.v[2] );

   m_quad.v[0] -= dot2 * reflectionNormal.m_quad.v[0];
   m_quad.v[1] -= dot2 * reflectionNormal.m_quad.v[1];
   m_quad.v[2] -= dot2 * reflectionNormal.m_quad.v[2];
}

///////////////////////////////////////////////////////////////////////////////

void Vector::setSelect( const VectorComparison& comparisonResult, const Vector& trueVec, const Vector& falseVec )
{
   m_quad.v[0] = comparisonResult.isAnySet<VectorComparison::MASK_X>() ? trueVec.m_quad.v[0] : falseVec.m_quad.v[0];
   m_quad.v[1] = comparisonResult.isAnySet<VectorComparison::MASK_Y>() ? trueVec.m_quad.v[1] : falseVec.m_quad.v[1];
   m_quad.v[2] = comparisonResult.isAnySet<VectorComparison::MASK_Z>() ? trueVec.m_quad.v[2] : falseVec.m_quad.v[2];
   m_quad.v[3] = comparisonResult.isAnySet<VectorComparison::MASK_W>() ? trueVec.m_quad.v[3] : falseVec.m_quad.v[3];
}

///////////////////////////////////////////////////////////////////////////////

template< VectorComparison::Mask M >
void Vector::setSelect( const Vector& trueVec, const Vector& falseVec )
{
   m_quad.v[0] = ( M & VectorComparison::MASK_X ) ? trueVec.m_quad.v[0] : falseVec.m_quad.v[0];
   m_quad.v[1] = ( M & VectorComparison::MASK_Y ) ? trueVec.m_quad.v[1] : falseVec.m_quad.v[1];
   m_quad.v[2] = ( M & VectorComparison::MASK_Z ) ? trueVec.m_quad.v[2] : falseVec.m_quad.v[2];
   m_quad.v[3] = ( M & VectorComparison::MASK_W ) ? trueVec.m_quad.v[3] : falseVec.m_quad.v[3];
}

///////////////////////////////////////////////////////////////////////////////

void Vector::less( const Vector& rhs, VectorComparison& outResult ) const
{
   outResult.m_mask =
      ( ( m_quad.v[0] < rhs.m_quad.v[0] ) ? VectorComparison::MASK_X : VectorComparison::MASK_NONE ) |
      ( ( m_quad.v[1] < rhs.m_quad.v[1] ) ? VectorComparison::MASK_Y : VectorComparison::MASK_NONE ) |
      ( ( m_quad.v[2] < rhs.m_quad.v[2] ) ? VectorComparison::MASK_Z : VectorComparison::MASK_NONE ) |    
      ( ( m_quad.v[3] < rhs.m_quad.v[3] ) ? VectorComparison::MASK_W : VectorComparison::MASK_NONE );
}

///////////////////////////////////////////////////////////////////////////////

void Vector::lessEqual( const Vector& rhs, VectorComparison& outResult ) const
{
   outResult.m_mask =
      ( ( m_quad.v[0] <= rhs.m_quad.v[0] ) ? VectorComparison::MASK_X : VectorComparison::MASK_NONE ) |
      ( ( m_quad.v[1] <= rhs.m_quad.v[1] ) ? VectorComparison::MASK_Y : VectorComparison::MASK_NONE ) |
      ( ( m_quad.v[2] <= rhs.m_quad.v[2] ) ? VectorComparison::MASK_Z : VectorComparison::MASK_NONE ) |    
      ( ( m_quad.v[3] <= rhs.m_quad.v[3] ) ? VectorComparison::MASK_W : VectorComparison::MASK_NONE );
}

///////////////////////////////////////////////////////////////////////////////

void Vector::greater( const Vector& rhs, VectorComparison& outResult ) const
{
   outResult.m_mask =
      ( ( m_quad.v[0] > rhs.m_quad.v[0] ) ? VectorComparison::MASK_X : VectorComparison::MASK_NONE ) |
      ( ( m_quad.v[1] > rhs.m_quad.v[1] ) ? VectorComparison::MASK_Y : VectorComparison::MASK_NONE ) |
      ( ( m_quad.v[2] > rhs.m_quad.v[2] ) ? VectorComparison::MASK_Z : VectorComparison::MASK_NONE ) |    
      ( ( m_quad.v[3] > rhs.m_quad.v[3] ) ? VectorComparison::MASK_W : VectorComparison::MASK_NONE );
}

///////////////////////////////////////////////////////////////////////////////

void Vector::greaterEqual( const Vector& rhs, VectorComparison& outResult ) const
{
   outResult.m_mask =
      ( ( m_quad.v[0] >= rhs.m_quad.v[0] ) ? VectorComparison::MASK_X : VectorComparison::MASK_NONE ) |
      ( ( m_quad.v[1] >= rhs.m_quad.v[1] ) ? VectorComparison::MASK_Y : VectorComparison::MASK_NONE ) |
      ( ( m_quad.v[2] >= rhs.m_quad.v[2] ) ? VectorComparison::MASK_Z : VectorComparison::MASK_NONE ) |    
      ( ( m_quad.v[3] >= rhs.m_quad.v[3] ) ? VectorComparison::MASK_W : VectorComparison::MASK_NONE );
}

///////////////////////////////////////////////////////////////////////////////

void Vector::equal( const Vector& rhs, VectorComparison& outResult ) const
{
   outResult.m_mask =
      ( ( m_quad.v[0] == rhs.m_quad.v[0] ) ? VectorComparison::MASK_X : VectorComparison::MASK_NONE ) |
      ( ( m_quad.v[1] == rhs.m_quad.v[1] ) ? VectorComparison::MASK_Y : VectorComparison::MASK_NONE ) |
      ( ( m_quad.v[2] == rhs.m_quad.v[2] ) ? VectorComparison::MASK_Z : VectorComparison::MASK_NONE ) |    
      ( ( m_quad.v[3] == rhs.m_quad.v[3] ) ? VectorComparison::MASK_W : VectorComparison::MASK_NONE );
}

///////////////////////////////////////////////////////////////////////////////

void Vector::notEqual( const Vector& rhs, VectorComparison& outResult ) const
{
   outResult.m_mask =
      ( ( m_quad.v[0] != rhs.m_quad.v[0] ) ? VectorComparison::MASK_X : VectorComparison::MASK_NONE ) |
      ( ( m_quad.v[1] != rhs.m_quad.v[1] ) ? VectorComparison::MASK_Y : VectorComparison::MASK_NONE ) |
      ( ( m_quad.v[2] != rhs.m_quad.v[2] ) ? VectorComparison::MASK_Z : VectorComparison::MASK_NONE ) |    
      ( ( m_quad.v[3] != rhs.m_quad.v[3] ) ? VectorComparison::MASK_W : VectorComparison::MASK_NONE );
}

///////////////////////////////////////////////////////////////////////////////

float& Vector::operator[]( int idx )
{
   return m_quad.v[idx];
}

///////////////////////////////////////////////////////////////////////////////

float Vector::operator[]( int idx ) const
{
   return m_quad.v[idx];
}

///////////////////////////////////////////////////////////////////////////////

void Vector::store( QuadStorage& storage ) const
{
   memcpy( storage.v, m_quad.v, sizeof( QuadStorage ) );
}

///////////////////////////////////////////////////////////////////////////////

void Vector::load( const QuadStorage& storage )
{
   memcpy( m_quad.v, storage.v, sizeof( QuadStorage ) );
}

///////////////////////////////////////////////////////////////////////////////

void Vector::store( float* arrFloats ) const
{
   memcpy( arrFloats, m_quad.v, sizeof( QuadStorage ) );
}

///////////////////////////////////////////////////////////////////////////////

void Vector::load( const float* arrFloats )
{
   memcpy( m_quad.v, arrFloats, sizeof( QuadStorage ) );
}

///////////////////////////////////////////////////////////////////////////////

const FastFloat Vector::getComponent( uint idx ) const
{
   return *( const FastFloat* )&m_quad.v[idx];
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

template< VectorComparison::Mask M >
void VectorComparison::set()
{
   m_mask = M;
}

///////////////////////////////////////////////////////////////////////////////

void VectorComparison::setAnd( const VectorComparison& c1, const VectorComparison& c2 )
{
   m_mask = c1.m_mask & c2.m_mask;
}

///////////////////////////////////////////////////////////////////////////////

void VectorComparison::setOr( const VectorComparison& c1, const VectorComparison& c2 )
{
   m_mask = c1.m_mask | c2.m_mask;
}

///////////////////////////////////////////////////////////////////////////////

void VectorComparison::not()
{
   m_mask = !m_mask;
}

///////////////////////////////////////////////////////////////////////////////

template< VectorComparison::Mask M >
bool VectorComparison::areAllSet() const
{
   if (M == MASK_NONE)
   {
      return true;
   }
   else if (M == MASK_XYZW)
   {
      return m_mask == MASK_XYZW;
   }
   else
   {
      return (m_mask & M) == M;
   }
}

///////////////////////////////////////////////////////////////////////////////

template< VectorComparison::Mask M >
bool VectorComparison::isAnySet() const
{
   if (M == MASK_NONE)
   {
      return false;
   }
   else if ( M == MASK_XYZW )
   {
      return m_mask != MASK_NONE;
   }
   else
   {
      return ( m_mask & M ) != MASK_NONE;
   }
}

///////////////////////////////////////////////////////////////////////////////

template< VectorComparison::Mask M >
bool VectorComparison::areAllClear() const
{
   if (M == MASK_NONE)
   {
      return false;
   }
   else if (M == MASK_XYZW)
   {
      return m_mask == MASK_NONE;
   }
   else
   {
      return ( m_mask & M ) == MASK_NONE;
   }
}

///////////////////////////////////////////////////////////////////////////////

int VectorComparison::toIndex( ) const
{
   return g_maskIndicesLookupTable[m_mask];
}

///////////////////////////////////////////////////////////////////////////////

#endif // _VECTOR_H
