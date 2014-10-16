#ifndef _QUATERNION_H
#error "This file can be included only in Quaternion.h"
#else

#include "core\MathDefs.h"
#include "core\SimdUtils.h"
#include "core\Vector.h"
#include "core\Assert.h"


///////////////////////////////////////////////////////////////////////////////

Quaternion::Quaternion()
{
   m_quad = Quad_0001;
}

///////////////////////////////////////////////////////////////////////////////

Quaternion::Quaternion( float x, float y, float z, float w )
{
   SimdUtils::toSimd( x, y, z, w, &m_quad );
}

///////////////////////////////////////////////////////////////////////////////

Quaternion::Quaternion( const QuadStorage& quad )
   : m_quad( quad )
{
}

///////////////////////////////////////////////////////////////////////////////

void Quaternion::setIdentity()
{
   m_quad = Quad_0001;
}

///////////////////////////////////////////////////////////////////////////////

void Quaternion::set( const QuadStorage& quad )
{
   m_quad = quad;
}

///////////////////////////////////////////////////////////////////////////////

void Quaternion::set( float x, float y, float z, float w )
{
   SimdUtils::toSimd( x, y, z, w, &m_quad );
}

///////////////////////////////////////////////////////////////////////////////

void Quaternion::setAxisAngle( const Vector& axis, const FastFloat& angle )
{
   ASSERT_MSG( axis.isNormalized(), "Axis vector is not unit length" );

   FastFloat halfAngle;
   halfAngle.setMul( angle, Float_Inv2 );
   FastFloat s, c;
   s.setSin( halfAngle );
   c.setCos( halfAngle );

   __m128 rotatedAxis = _mm_mul_ps( axis.m_quad, s.m_val );
   SimdUtils::setXYZ_W( &rotatedAxis, &c.m_val, &m_quad );
}

///////////////////////////////////////////////////////////////////////////////

void Quaternion::operator=( const Quaternion& rhs )
{
   m_quad = rhs.m_quad;
}

///////////////////////////////////////////////////////////////////////////////

bool Quaternion::operator==( const Quaternion& rhs ) const
{
   __m128 cmpResult = _mm_cmpeq_ps( m_quad, rhs.m_quad );
   int vmask = _mm_movemask_ps( cmpResult );
   return vmask == 0xf;
}

///////////////////////////////////////////////////////////////////////////////

bool Quaternion::operator!=( const Quaternion& rhs ) const
{
   __m128 cmpResult = _mm_cmpeq_ps( m_quad, rhs.m_quad );
   int vmask = _mm_movemask_ps( cmpResult );
   return vmask == 0x0;
}

///////////////////////////////////////////////////////////////////////////////

float& Quaternion::operator[]( int idx )
{
   return m_quad.m128_f32[idx];
}

///////////////////////////////////////////////////////////////////////////////

float Quaternion::operator[]( int idx ) const
{
   return m_quad.m128_f32[idx];
}

///////////////////////////////////////////////////////////////////////////////

void Quaternion::setAdd( const Quaternion& q1, const Quaternion& q2 )
{
   m_quad = _mm_add_ps( q1.m_quad, q2.m_quad );
}

///////////////////////////////////////////////////////////////////////////////

void Quaternion::setSub( const Quaternion& q1, const Quaternion& q2 )
{
   m_quad = _mm_sub_ps( q1.m_quad, q2.m_quad );
}

///////////////////////////////////////////////////////////////////////////////

void Quaternion::add( const Quaternion& rhs )
{
   m_quad = _mm_add_ps( m_quad, rhs.m_quad );
}

///////////////////////////////////////////////////////////////////////////////

void Quaternion::sub( const Quaternion& rhs )
{
   m_quad = _mm_sub_ps( m_quad, rhs.m_quad );
}

///////////////////////////////////////////////////////////////////////////////

void Quaternion::setMul( const Quaternion& q1, const Quaternion& q2 )
{
   __m128 q1Real, q2Real;

   SimdUtils::getComponent( &q1.m_quad, 3, &q1Real );
   SimdUtils::getComponent( &q2.m_quad, 3, &q2Real );

   __m128 quadMul;
   SimdUtils::cross( &q1.m_quad, &q2.m_quad, &quadMul );

   __m128 wPart = _mm_setzero_ps();
   SimdUtils::addMul( &wPart, &q1Real, &q2.m_quad, &wPart );
   SimdUtils::addMul( &wPart, &q2Real, &q1.m_quad, &wPart );

   quadMul = _mm_sub_ps( wPart, quadMul );
   
   __m128 w;
   SimdUtils::dot<3>( &q1.m_quad, &q2.m_quad, &w );

   __m128 realMul = _mm_mul_ps( q1Real, q2Real );
   w = _mm_sub_ps( realMul, w );
   
   SimdUtils::setXYZ_W( &quadMul, &w, &m_quad );
}

///////////////////////////////////////////////////////////////////////////////

void Quaternion::mul( const Quaternion& rhs )
{
   setMul( *this, rhs );
}

///////////////////////////////////////////////////////////////////////////////

void Quaternion::neg()
{
   m_quad = _mm_xor_ps( m_quad, _MM_SIGN_MASK );
}

///////////////////////////////////////////////////////////////////////////////

void Quaternion::setConjugated( const Quaternion& rhs )
{
   m_quad = _mm_xor_ps( rhs.m_quad, _MM_QUAT_CONJUGATE_MASK );
}

///////////////////////////////////////////////////////////////////////////////

void Quaternion::conjugate()
{
   m_quad = _mm_xor_ps( m_quad, _MM_QUAT_CONJUGATE_MASK );
}

///////////////////////////////////////////////////////////////////////////////

void Quaternion::normalize()
{
   __m128 dot;
   SimdUtils::dot<4>( &m_quad, &m_quad, &dot );

   // at this point we have a squared length of the plane normal - so calculate its square root
   __m128 equalsZero = _mm_cmple_ps( dot, _mm_setzero_ps() );
   __m128 sqrtDot = _mm_sqrt_ps( dot );
   __m128 length = _mm_andnot_ps( equalsZero, sqrtDot );

   m_quad = _mm_div_ps( m_quad, length );
}

///////////////////////////////////////////////////////////////////////////////

void Quaternion::setSlerp( const Quaternion& a, const Quaternion& b, const FastFloat& t )
{
   const __m128 delta = SimdUtils::fromFloat( 1.0f - 1e-3f );

   // Calculate angle between them.
   __m128 cosTheta;
   SimdUtils::dot<4>( &a.m_quad, &b.m_quad, &cosTheta );

   // if theta = 180 degrees then result is not fully defined
   // we could rotate around any axis normal to a or b
   __m128 cosThetaLessZero;
   SimdUtils::lessZeroMask( &cosTheta, &cosThetaLessZero );
   SimdUtils::flipSign( &cosTheta, &cosThetaLessZero, &cosTheta );

   // if theta = 180 degrees then result is not fully defined
   // we could rotate around any axis normal to a or b
   __m128 t0, t1;
   if ( _mm_ucomilt_ss( cosTheta, delta ) == 1 ) // cosTheta < delta
   { 
      const __m128 theta = SimdUtils::fromFloat( acos( cosTheta.m128_f32[0] ) );

      // use sqrtInv(1+c^2) instead of 1.0/sin(theta) 
      __m128 iSinTheta = _mm_sub_ps( Float_1.m_val, _mm_mul_ps( cosTheta, cosTheta ) );
      SimdUtils::sqrtInverse( &iSinTheta, &iSinTheta );
      
      const __m128 tTheta = _mm_mul_ps( t.m_val, theta );

      const __m128 thetaDiff = _mm_sub_ss( theta, tTheta ); // an 'ss' instruction, 'cause we're interested only in the W component
      const __m128 s0 = SimdUtils::fromFloat( sin( thetaDiff.m128_f32[0] ) );
      const __m128 s1 = SimdUtils::fromFloat( sin( tTheta.m128_f32[0] ) );

      t0 = _mm_mul_ps( s0, iSinTheta );
      t1 = _mm_mul_ps( s1, iSinTheta );
   }
   else
   {
      t0 = _mm_sub_ps( Float_1.m_val, t.m_val );
      t1 = t.m_val;
   }

   SimdUtils::flipSign( &t1, &cosThetaLessZero, &t1 );

   __m128 slerp = _mm_mul_ps( t0, a.m_quad );
   SimdUtils::addMul( &slerp, &t1, &b.m_quad, &slerp );
   SimdUtils::normalize<4>( &slerp, &slerp );

   m_quad = slerp;
}

///////////////////////////////////////////////////////////////////////////////

void Quaternion::setNlerp( const Quaternion& a, const Quaternion& b, const FastFloat& t )
{
   m_quad = _mm_sub_ps( b.m_quad, a.m_quad );
   m_quad = _mm_mul_ps( m_quad, t.m_val );
   m_quad = _mm_add_ps( m_quad, a.m_quad );

   SimdUtils::normalize<4>( &m_quad, &m_quad );
}

///////////////////////////////////////////////////////////////////////////////

void Quaternion::setAddScaledVector( const Quaternion& a, const Vector& v, const FastFloat& t )
{
   Quaternion q;
   q.m_quad = _mm_mul_ps( v.m_quad, t.m_val );
   SimdUtils::setXYZ_W( &q.m_quad, &Float_0.m_val, &q.m_quad );
   q.mul( a );

   q.m_quad = _mm_mul_ps( q.m_quad, Float_Inv2.m_val );
   m_quad = _mm_add_ps( a.m_quad, q.m_quad );

   SimdUtils::normalize<4>( &m_quad, &m_quad );
}

///////////////////////////////////////////////////////////////////////////////

void Quaternion::addScaledVector( const Vector& v, const FastFloat& t )
{
   Quaternion q;
   q.m_quad = _mm_mul_ps( v.m_quad, t.m_val );
   SimdUtils::setXYZ_W( &q.m_quad, &Float_0.m_val, &q.m_quad );
   q.mul( *this );

   q.m_quad = _mm_mul_ps( q.m_quad, Float_Inv2.m_val );
   m_quad = _mm_add_ps( m_quad, q.m_quad );

   SimdUtils::normalize<4>( &m_quad, &m_quad );
}

///////////////////////////////////////////////////////////////////////////////

void Quaternion::getAxis( Vector& outAxis ) const
{
   __m128 axis = m_quad;
   SimdUtils::normalize<3>( &axis, &axis );

   __m128 wComp;
   SimdUtils::getComponent( &axis, 3, &wComp );
   __m128 wLessZero;
   SimdUtils::lessZeroMask( &wComp, &wLessZero );
   SimdUtils::flipSign( &axis, &wLessZero, &axis );

   outAxis.m_quad = axis;
}

///////////////////////////////////////////////////////////////////////////////

void Quaternion::getAngle( FastFloat& outAngle ) const
{
   __m128 wComp;
   SimdUtils::getComponent( &m_quad, 3, &wComp );
   float v = ::acos( wComp.m128_f32[0] );
   outAngle.m_val = _mm_set1_ps( v * 2.0f );
}

///////////////////////////////////////////////////////////////////////////////

void Quaternion::transform( const Vector& inVec, Vector& outVec ) const
{
   __m128 simdVec = inVec.m_quad;

   __m128 qreal;
   SimdUtils::getComponent( &m_quad, 3, &qreal );

   __m128 q2minus1;
   SimdUtils::addMul( &Float_MinusInv2.m_val, &qreal, &qreal, &q2minus1 );

   __m128 ret = _mm_mul_ps( simdVec, q2minus1 );

   __m128 imagDotDir;
   SimdUtils::dot<3>( &m_quad, &simdVec, &imagDotDir );
   SimdUtils::addMul( &ret, &m_quad, &imagDotDir, &ret );

   __m128 imagCrossDir;
   SimdUtils::cross( &m_quad, &simdVec, &imagCrossDir );
   SimdUtils::addMul( &ret, &imagCrossDir, &qreal, &ret );

   outVec.m_quad = _mm_add_ps( ret, ret );
}

///////////////////////////////////////////////////////////////////////////////

void Quaternion::store( float* arrFloats ) const
{
   _mm_store_ps( arrFloats, m_quad );
}

///////////////////////////////////////////////////////////////////////////////

void Quaternion::load( const float* arrFloats )
{
   m_quad = _mm_load_ps( arrFloats );
}

///////////////////////////////////////////////////////////////////////////////


#endif // _QUATERNION_H
