#ifndef _VECTOR_H
#error "This file can only be included from Vector.h"
#else

#include "core\FastFloat.h"
#include "core\Algorithms.h"
#include "core\SimdUtils.h"


///////////////////////////////////////////////////////////////////////////////

static ALIGN_16 const int VectorComparison_maskToComparison[16*4] = 
{
   0,  0, 0, 0,      // MASK_NONE
   ~0,  0, 0, 0,     // MASK_W
   0, ~0, 0, 0,      // MASK_Z
   ~0, ~0, 0, 0,     // MASK_ZW
   0,  0,~0, 0,      // MASK_Y
   ~0,  0,~0, 0,     // MASK_YW
   0, ~0,~0, 0,      // MASK_YZ
   ~0, ~0,~0, 0,     // MASK_YZW
   0,  0, 0,~0,      // MASK_X
   ~0,  0, 0,~0,     // MASK_XW
   0, ~0, 0,~0,      // MASK_XZ
   ~0, ~0, 0,~0,     // MASK_XZW
   0,  0,~0,~0,      // MASK_XY
   ~0,  0,~0,~0,     // MASK_XYW
   0, ~0,~0,~0,      // MASK_XYZ
   ~0, ~0,~0,~0,     // MASK_XYZW
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

Vector::Vector() 
{
   m_quad = _mm_setzero_ps();
}

///////////////////////////////////////////////////////////////////////////////

Vector::Vector( const QuadStorage& quad )
{
   m_quad = quad;
}

///////////////////////////////////////////////////////////////////////////////

Vector::Vector( float x, float y, float z, float w )
{
   SimdUtils::toSimd( x, y, z, w, &m_quad );
}

///////////////////////////////////////////////////////////////////////////////

void Vector::set( float* coords )
{
   m_quad = _mm_load_ps( coords );
}

///////////////////////////////////////////////////////////////////////////////

void Vector::setZero()
{
   m_quad = _mm_setzero_ps();
}

///////////////////////////////////////////////////////////////////////////////

void Vector::set( const QuadStorage& quad )
{
   m_quad = quad;
}


///////////////////////////////////////////////////////////////////////////////

void Vector::set( float x, float y, float z, float w )
{
  SimdUtils::toSimd( x, y, z, w, &m_quad );
}

///////////////////////////////////////////////////////////////////////////////

void Vector::set( const FastFloat& x, const FastFloat& y, const FastFloat& z, const FastFloat& w )
{
   m_quad = PACK_SIMD( x.m_val, y.m_val, z.m_val, w.m_val );
}

///////////////////////////////////////////////////////////////////////////////

void Vector::setBroadcast( const FastFloat& val )
{
   m_quad = val.m_val;
}

///////////////////////////////////////////////////////////////////////////////

bool Vector::equals4( const Vector& rhs ) const
{
   __m128 cmpResult = _mm_cmpeq_ps( m_quad, rhs.m_quad );
   int vmask = _mm_movemask_ps( cmpResult );
   return vmask == 0xf;
}

///////////////////////////////////////////////////////////////////////////////

bool Vector::operator==( const Vector& rhs ) const
{
   __m128 thisWithoutW = _mm_and_ps( m_quad, _MM_DISCARD_W_MASK );
   __m128 rhsWithoutW = _mm_and_ps( rhs.m_quad, _MM_DISCARD_W_MASK );

   __m128 cmpResult = _mm_cmpeq_ps( thisWithoutW, rhsWithoutW );
   int vmask = _mm_movemask_ps( cmpResult );
   return ( vmask & 0x7 ) == 0x7;

}

///////////////////////////////////////////////////////////////////////////////

bool Vector::operator!=( const Vector& rhs ) const
{
   __m128 thisWithoutW = _mm_and_ps( m_quad, _MM_DISCARD_W_MASK );
   __m128 rhsWithoutW = _mm_and_ps( rhs.m_quad, _MM_DISCARD_W_MASK );

   __m128 cmpResult = _mm_cmpeq_ps( thisWithoutW, rhsWithoutW );
   int vmask = _mm_movemask_ps( cmpResult );
   return ( vmask & 0x7 ) == 0x0;
}

///////////////////////////////////////////////////////////////////////////////

void Vector::operator=( const Vector& rhs )
{
   m_quad = rhs.m_quad;
}

///////////////////////////////////////////////////////////////////////////////

void Vector::setMul( const Vector& vec, const FastFloat& t )
{
   m_quad = _mm_mul_ps( vec.m_quad, t.m_val );
}

///////////////////////////////////////////////////////////////////////////////

void Vector::setMul( const Vector& vec1, const Vector& vec2 )
{
   m_quad = _mm_mul_ps( vec1.m_quad, vec2.m_quad );
}

///////////////////////////////////////////////////////////////////////////////

void Vector::setMulAdd( const Vector& vec1, const FastFloat& t, const Vector& vec2 )
{
   __m128 tmp = _mm_mul_ps( vec1.m_quad, t.m_val );
   m_quad = _mm_add_ps( tmp, vec2.m_quad );
}

///////////////////////////////////////////////////////////////////////////////

void Vector::setMulAdd( const Vector& vec1, const Vector& t, const Vector& vec2 )
{
   __m128 tmp = _mm_mul_ps( vec1.m_quad, t.m_quad );
   m_quad = _mm_add_ps( tmp, vec2.m_quad );
}

///////////////////////////////////////////////////////////////////////////////

void Vector::setDiv( const Vector& vec1, const Vector& vec2 )
{
   m_quad = _mm_div_ps( vec1.m_quad, vec2.m_quad );
}

///////////////////////////////////////////////////////////////////////////////

void Vector::setDiv( const Vector& vec1, const FastFloat& t )
{
   m_quad = _mm_div_ps( vec1.m_quad, t.m_val );
}

///////////////////////////////////////////////////////////////////////////////

void Vector::setAdd( const Vector& vec1, const Vector& vec2 )
{
   m_quad = _mm_add_ps( vec1.m_quad, vec2.m_quad );
}

///////////////////////////////////////////////////////////////////////////////

void Vector::setSub( const Vector& vec1, const Vector& vec2 )
{
   m_quad = _mm_sub_ps( vec1.m_quad, vec2.m_quad );
}

///////////////////////////////////////////////////////////////////////////////

void Vector::mul( const FastFloat& t )
{
   m_quad = _mm_mul_ps( m_quad, t.m_val );
}

///////////////////////////////////////////////////////////////////////////////

void Vector::mul( const Vector& vec )
{
   m_quad = _mm_mul_ps( m_quad, vec.m_quad );
}

///////////////////////////////////////////////////////////////////////////////

void Vector::div( const FastFloat& t )
{
   m_quad = _mm_div_ps( m_quad, t.m_val );
}

///////////////////////////////////////////////////////////////////////////////

void Vector::div( const Vector& vec )
{
   m_quad = _mm_div_ps( m_quad, vec.m_quad );
}

///////////////////////////////////////////////////////////////////////////////

void Vector::add( const Vector& vec )
{
   m_quad = _mm_add_ps( m_quad, vec.m_quad );
}

///////////////////////////////////////////////////////////////////////////////

void Vector::sub( const Vector& vec )
{
   m_quad = _mm_sub_ps( m_quad, vec.m_quad );
}

///////////////////////////////////////////////////////////////////////////////

void Vector::neg()
{
   m_quad = _mm_xor_ps( m_quad, _MM_SIGN_MASK );
}

///////////////////////////////////////////////////////////////////////////////

const FastFloat Vector::dot( const Vector& rhs ) const
{
   __m128 dotVal;
   SimdUtils::dot<3>( &m_quad, &rhs.m_quad, &dotVal );

   return *( const FastFloat* )&dotVal;
}

///////////////////////////////////////////////////////////////////////////////

const FastFloat Vector::dot4( const Vector& rhs ) const
{
   __m128 dotVal;
   SimdUtils::dot<4>( &m_quad, &rhs.m_quad, &dotVal );

   return *( const FastFloat* )&dotVal;
}

///////////////////////////////////////////////////////////////////////////////

void Vector::setCross( const Vector& v1, const Vector& v2 )
{
   SimdUtils::cross( &v1.m_quad, &v2.m_quad, &m_quad );
}

///////////////////////////////////////////////////////////////////////////////

void Vector::preCross( const Vector& rhs )
{
   SimdUtils::cross( &m_quad, &rhs.m_quad, &m_quad );
}

///////////////////////////////////////////////////////////////////////////////

void Vector::postCross( const Vector& rhs )
{
   SimdUtils::cross( &rhs.m_quad, &m_quad, &m_quad );
}

///////////////////////////////////////////////////////////////////////////////

void Vector::setNormalized( const Vector& rhs )
{
   SimdUtils::normalize<3>( &rhs.m_quad, &m_quad );
}

///////////////////////////////////////////////////////////////////////////////

void Vector::setNormalized( float x, float y, float z )
{
   SimdUtils::toSimd( x, y, z, 0.0f, &m_quad );
   SimdUtils::normalize<3>( &m_quad, &m_quad );
}

///////////////////////////////////////////////////////////////////////////////

void Vector::setNormalized( const FastFloat& x, const FastFloat& y, const FastFloat& z )
{
   m_quad = PACK_SIMD( x.m_val, y.m_val, z.m_val, Float_0.m_val );
   SimdUtils::normalize<3>( &m_quad, &m_quad );
}

///////////////////////////////////////////////////////////////////////////////

void Vector::normalize()
{
   SimdUtils::normalize<3>( &m_quad, &m_quad );
}

///////////////////////////////////////////////////////////////////////////////

void Vector::floor()
{
   SimdUtils::floor( &m_quad, &m_quad );
}

///////////////////////////////////////////////////////////////////////////////

void Vector::setLerp( const Vector& a, const Vector& b, const FastFloat& t )
{
   m_quad = _mm_sub_ps( b.m_quad, a.m_quad );
   m_quad = _mm_mul_ps( m_quad, t.m_val );
   m_quad = _mm_add_ps( m_quad, a.m_quad );
}

///////////////////////////////////////////////////////////////////////////////

const FastFloat Vector::length() const
{
   __m128 len;
   SimdUtils::dot<3>( &m_quad, &m_quad, &len );
   SimdUtils::sqrt( &len, &len );

   return *( const FastFloat* )&len;
}

///////////////////////////////////////////////////////////////////////////////

const FastFloat Vector::lengthSq() const
{
   __m128 lenSq;
   SimdUtils::dot<3>( &m_quad, &m_quad, &lenSq );

   return *( const FastFloat* )&lenSq;
}

///////////////////////////////////////////////////////////////////////////////

void Vector::setMax( const Vector& a, const Vector& b )
{
   m_quad = _mm_max_ps( a.m_quad, b.m_quad );
}

///////////////////////////////////////////////////////////////////////////////

void Vector::setMin( const Vector& a, const Vector& b )
{
   m_quad = _mm_min_ps( a.m_quad, b.m_quad );
}

///////////////////////////////////////////////////////////////////////////////

void Vector::setClamped( const Vector& a, const Vector& minVal, const Vector& maxVal )
{
   m_quad = _mm_max_ps( a.m_quad, minVal.m_quad );
   m_quad = _mm_min_ps( m_quad, maxVal.m_quad );
}

///////////////////////////////////////////////////////////////////////////////

void Vector::setAbs( const Vector& rhs )
{
   m_quad = _mm_castsi128_ps( _mm_srli_epi32( _mm_slli_epi32( _mm_castps_si128( rhs.m_quad ), 1 ), 1 ) );
}

///////////////////////////////////////////////////////////////////////////////

void Vector::abs()
{
   m_quad = _mm_castsi128_ps( _mm_srli_epi32( _mm_slli_epi32( _mm_castps_si128( m_quad ), 1 ), 1 ) );
}

///////////////////////////////////////////////////////////////////////////////

void Vector::setReflected( const Vector& incomingVector, const Vector& reflectionNormal )
{
   __m128 a, b;
   SimdUtils::dot<3>( &incomingVector.m_quad, &reflectionNormal.m_quad, &a );
   b = _mm_mul_ps( a, Float_2.m_val );
   a = _mm_mul_ps( b, reflectionNormal.m_quad );

   m_quad = _mm_sub_ps( incomingVector.m_quad, a );
}

///////////////////////////////////////////////////////////////////////////////

void Vector::reflect( const Vector& reflectionNormal )
{
   __m128 a, b;
   SimdUtils::dot<3>( &m_quad, &reflectionNormal.m_quad, &a );
   b = _mm_mul_ps( a, Float_2.m_val );
   a = _mm_mul_ps( b, reflectionNormal.m_quad );

   m_quad = _mm_sub_ps( m_quad, a );
}

///////////////////////////////////////////////////////////////////////////////

void Vector::setSelect( const VectorComparison& comparisonResult, const Vector& trueVec, const Vector& falseVec )
{
#if SSE_VERSION >= 0x41
   m_quad = _mm_blendv_ps( falseVec.m_quad, trueVec.m_quad, comparisonResult );
#else
   m_quad = _mm_or_ps( _mm_and_ps( comparisonResult.m_mask, trueVec.m_quad ), _mm_andnot_ps( comparisonResult.m_mask, falseVec.m_quad ) );
#endif
}

///////////////////////////////////////////////////////////////////////////////

template< VectorComparison::Mask M >
void Vector::setSelect( const Vector& trueVec, const Vector& falseVec )
{
   VectorComparison comp; 
   comp.set< M >();
   setSelect( comp, trueVec, falseVec );
}

///////////////////////////////////////////////////////////////////////////////

void Vector::less( const Vector& rhs, VectorComparison& outResult ) const
{
   outResult.m_mask = _mm_cmplt_ps( m_quad, rhs.m_quad );
}

///////////////////////////////////////////////////////////////////////////////

void Vector::lessEqual( const Vector& rhs, VectorComparison& outResult ) const
{
   outResult.m_mask = _mm_cmple_ps( m_quad, rhs.m_quad );
}

///////////////////////////////////////////////////////////////////////////////

void Vector::greater( const Vector& rhs, VectorComparison& outResult ) const
{
   outResult.m_mask = _mm_cmpgt_ps( m_quad, rhs.m_quad );
}

///////////////////////////////////////////////////////////////////////////////

void Vector::greaterEqual( const Vector& rhs, VectorComparison& outResult ) const
{
   outResult.m_mask = _mm_cmpge_ps( m_quad, rhs.m_quad );
}

///////////////////////////////////////////////////////////////////////////////

void Vector::equal( const Vector& rhs, VectorComparison& outResult ) const
{
   outResult.m_mask = _mm_cmpeq_ps( m_quad, rhs.m_quad );
}

///////////////////////////////////////////////////////////////////////////////

void Vector::notEqual( const Vector& rhs, VectorComparison& outResult ) const
{
   outResult.m_mask = _mm_cmpneq_ps( m_quad, rhs.m_quad );
}

///////////////////////////////////////////////////////////////////////////////

float& Vector::operator[]( int idx )
{
   return m_quad.m128_f32[idx];
}

///////////////////////////////////////////////////////////////////////////////

float Vector::operator[]( int idx ) const
{
   return m_quad.m128_f32[idx];
}

///////////////////////////////////////////////////////////////////////////////

void Vector::store( QuadStorage& storage ) const
{
   storage = m_quad;
}

///////////////////////////////////////////////////////////////////////////////

void Vector::load( const QuadStorage& storage )
{
   m_quad = storage;
}

///////////////////////////////////////////////////////////////////////////////

void Vector::store( float* arrFloats ) const
{
   _mm_store_ps( arrFloats, m_quad );
}

///////////////////////////////////////////////////////////////////////////////

void Vector::load( const float* arrFloats )
{
   m_quad = _mm_load_ps( arrFloats );
}

///////////////////////////////////////////////////////////////////////////////

const FastFloat Vector::getComponent( uint idx ) const
{
   __m128 comp;
   SimdUtils::getComponent( &m_quad, idx, &comp );
   return *( const FastFloat* )&comp;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

template< VectorComparison::Mask M >
void VectorComparison::set()
{
   if (M == MASK_NONE)
   {
      m_mask = _mm_setzero_ps();
   }
   else
   {
      m_mask = _mm_load_ps( (const float*)&(VectorComparison_maskToComparison[M*4]) );
   }
}

///////////////////////////////////////////////////////////////////////////////

void VectorComparison::setAnd( const VectorComparison& c1, const VectorComparison& c2 )
{
   m_mask = _mm_and_ps( c1.m_mask, c2.m_mask );
}

///////////////////////////////////////////////////////////////////////////////

void VectorComparison::setOr( const VectorComparison& c1, const VectorComparison& c2 )
{
   m_mask = _mm_or_ps( c1.m_mask, c2.m_mask );
}

///////////////////////////////////////////////////////////////////////////////

void VectorComparison::not()
{
   m_mask = _mm_xor_ps( m_mask, m_mask );
}

///////////////////////////////////////////////////////////////////////////////

template< VectorComparison::Mask M >
bool VectorComparison::areAllSet() const
{
   int vmask = _mm_movemask_ps( m_mask );
   return ( vmask & M ) == M;
}

///////////////////////////////////////////////////////////////////////////////

template< VectorComparison::Mask M >
bool VectorComparison::isAnySet() const
{
   int vmask = _mm_movemask_ps( m_mask );
   return vmask & (int)M;
}

///////////////////////////////////////////////////////////////////////////////

template< VectorComparison::Mask M >
bool VectorComparison::areAllClear() const
{
   int vmask = _mm_movemask_ps( m_mask );
   return ( vmask & M ) == 0;
}
///////////////////////////////////////////////////////////////////////////////

int VectorComparison::toIndex( ) const
{
   int vmask = _mm_movemask_ps( m_mask );
   return g_maskIndicesLookupTable[vmask];
}

///////////////////////////////////////////////////////////////////////////////

#endif // _VECTOR_H
