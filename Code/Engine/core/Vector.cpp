#include "core\Vector.h"
#include "core\OutStream.h"
#include "core\InStream.h"
#include "core\Math.h"
#include "core\FastFloat.h"

// common code

///////////////////////////////////////////////////////////////////////////////

bool Vector::isNormalized() const
{
   FastFloat v;
   v.setSub( lengthSq(), Float_1 );
   v.abs();
   return v < Float_1e_4;
}

///////////////////////////////////////////////////////////////////////////////

bool Vector::isOk() const
{
   bool isNotANumber = ( ( (*this)[0] != (*this)[0] ) || ( (*this)[1] != (*this)[1] ) || ( (*this)[2] != (*this)[2] ) || ( (*this)[3] != (*this)[3] ) );
   return !isNotANumber;
}

///////////////////////////////////////////////////////////////////////////////

std::ostream& operator<<( std::ostream& stream, const Vector& rhs )
{
   stream << rhs[0] << rhs[1] << rhs[2] << rhs[3];
   return stream;
}

///////////////////////////////////////////////////////////////////////////////

OutStream& operator<<( OutStream& serializer, const Vector& rhs )
{
   serializer << rhs[0] << rhs[1] << rhs[2] << rhs[3];
   return serializer;
}

///////////////////////////////////////////////////////////////////////////////

InStream& operator>>( InStream& serializer, Vector& rhs )
{
   serializer >> rhs[0] >> rhs[1] >> rhs[2] >> rhs[3];
   return serializer;
}

///////////////////////////////////////////////////////////////////////////////

QuadStorage g_quadConstants[QuadMathConst_MAX] = 
{
   QUAD_CONSTANT( 0.0f, 0.0f, 0.0f, 0.0f ),
   QUAD_CONSTANT( 1.0f, 1.0f, 1.0f, 1.0f ),

   QUAD_CONSTANT( -1.0f, -1.0f, -1.0f, -1.0f ),

   QUAD_CONSTANT( 1.0f, 0.0f, 0.0f, 0.0f ),
   QUAD_CONSTANT( 0.0f, 1.0f, 0.0f, 0.0f ),
   QUAD_CONSTANT( 0.0f, 0.0f, 1.0f, 0.0f ),
   QUAD_CONSTANT( 0.0f, 0.0f, 0.0f, 1.0f ),
   QUAD_CONSTANT(-1.0f, 0.0f, 0.0f, 0.0f ),
   QUAD_CONSTANT( 0.0f,-1.0f, 0.0f, 0.0f ),
   QUAD_CONSTANT( 0.0f, 0.0f,-1.0f, 0.0f ),
   QUAD_CONSTANT( 0.0f, 0.0f, 0.0f,-1.0f ),

   QUAD_CONSTANT( 1e-1f, 1e-1f, 1e-1f, 1e-1f ), // QuadMathConst_Eps_1
   QUAD_CONSTANT( 1e-2f, 1e-2f, 1e-2f, 1e-2f ), // QuadMathConst_Eps_2
   QUAD_CONSTANT( 1e-3f, 1e-3f, 1e-3f, 1e-3f ), // QuadMathConst_Eps_3
};

///////////////////////////////////////////////////////////////////////////////

Vector  q_vecConstants[VecConstants_MAX] =
{
   Vector( Quad_1000 ),
   Vector( Quad_0100 ),
   Vector( Quad_0010 ),
   Vector( Quad_0001 ),
   Vector( Quad_Neg_1000 ),
   Vector( Quad_Neg_0100 ),
   Vector( Quad_Neg_0010 ),
   Vector( Quad_Neg_0001 ),
   Vector( Quad_0 ),
   Vector( Quad_1 ),
   Vector( Quad_Minus1 ),
};

///////////////////////////////////////////////////////////////////////////////

int g_maskIndicesLookupTable[] = {
   -1, // MASK_NONE
    0, // MASK_X
    1, // MASK_Y
   -1, // MASK_XY
    2, // MASK_Z
   -1, // MASK_XZ
   -1, // MASK_YZ
   -1, // MASK_XYZ
    3, // MASK_W
   -1, // MASK_XW
   -1, // MASK_YW
   -1, // MASK_XYW
   -1, // MASK_YZW
   -1, // MASK_ZW
   -1, // MASK_XZW
   -1, // MASK_XYZW
};



///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

// end of common code

#ifndef _USE_SIMD

// Code that runs on FPU

///////////////////////////////////////////////////////////////////////////////

template<>
const FastFloat Vector::getMin<2>() const
{
   float val = min2( m_quad.v[0], m_quad.v[1] );
   return ( const FastFloat& )val;
}

///////////////////////////////////////////////////////////////////////////////

template<>
const FastFloat Vector::getMin<3>() const
{
   float val = min2( m_quad.v[0], min2( m_quad.v[1], m_quad.v[2] ) );
   return ( const FastFloat& )val;
}

///////////////////////////////////////////////////////////////////////////////

template<>
const FastFloat Vector::getMin<4>() const
{
   float val = min2( m_quad.v[0], min2( m_quad.v[1], min2( m_quad.v[2], m_quad.v[3] ) ) );
   return ( const FastFloat& )val;
}

///////////////////////////////////////////////////////////////////////////////

template<>
const FastFloat Vector::getMax<2>() const
{
   float val = max2( m_quad.v[0], m_quad.v[1] );
   return ( const FastFloat& )val;
}

///////////////////////////////////////////////////////////////////////////////

template<>
const FastFloat Vector::getMax<3>() const
{
   float val = max2( m_quad.v[0], max2( m_quad.v[1], m_quad.v[2] ) );
   return ( const FastFloat& )val;
}

///////////////////////////////////////////////////////////////////////////////

template<>
const FastFloat Vector::getMax<4>() const
{
   float val = max2( m_quad.v[0], max2( m_quad.v[1], max2( m_quad.v[2], m_quad.v[3] ) ) );
   return ( const FastFloat& )val;
}

///////////////////////////////////////////////////////////////////////////////


template<>
void Vector::selectMin<2>( VectorComparison& outResult ) const
{
   if ( m_quad.v[0] < m_quad.v[1] )
   {
      outResult.set< VectorComparison::MASK_X >( );
   }
   else
   {
      outResult.set< VectorComparison::MASK_Y >( );
   }
}

///////////////////////////////////////////////////////////////////////////////

template<>
void Vector::selectMin<3>( VectorComparison& outResult ) const
{
   int minIdx;

   if ( m_quad.v[0] < m_quad.v[1] )
   {
      minIdx = 0;
      outResult.set< VectorComparison::MASK_X >( );
   }
   else
   {
      minIdx = 1;
      outResult.set< VectorComparison::MASK_Y >( );
   }

   if ( m_quad.v[minIdx] >= m_quad.v[2] )
   {
      minIdx = 2;
      outResult.set< VectorComparison::MASK_Z >( );
   }
}

///////////////////////////////////////////////////////////////////////////////

template<>
void Vector::selectMin<4>( VectorComparison& outResult ) const
{
   int minIdx;

   if ( m_quad.v[0] < m_quad.v[1] )
   {
      minIdx = 0;
      outResult.set< VectorComparison::MASK_X >( );
   }
   else
   {
      minIdx = 1;
      outResult.set< VectorComparison::MASK_Y >( );
   }

   if ( m_quad.v[minIdx] >= m_quad.v[2] )
   {
      minIdx = 2;
      outResult.set< VectorComparison::MASK_Z >( );
   }

   if ( m_quad.v[minIdx] >= m_quad.v[3] )
   {
      minIdx = 3;
      outResult.set< VectorComparison::MASK_W >( );
   }
}

///////////////////////////////////////////////////////////////////////////////

template<>
void Vector::selectMax<2>( VectorComparison& outResult ) const
{
   if ( m_quad.v[0] > m_quad.v[1] )
   {
      outResult.set< VectorComparison::MASK_X >( );
   }
   else
   {
      outResult.set< VectorComparison::MASK_Y >( );
   }
}

///////////////////////////////////////////////////////////////////////////////

template<>
void Vector::selectMax<3>( VectorComparison& outResult ) const
{
   int maxIdx;

   if ( m_quad.v[0] > m_quad.v[1] )
   {
      maxIdx = 0;
      outResult.set< VectorComparison::MASK_X >( );
   }
   else
   {
      maxIdx = 1;
      outResult.set< VectorComparison::MASK_Y >( );
   }

   if ( m_quad.v[maxIdx] <= m_quad.v[2] )
   {
      maxIdx = 2;
      outResult.set< VectorComparison::MASK_Z >( );
   }
}

///////////////////////////////////////////////////////////////////////////////

template<>
void Vector::selectMax<4>( VectorComparison& outResult ) const
{
   int maxIdx;

   if ( m_quad.v[0] > m_quad.v[1] )
   {
      maxIdx = 0;
      outResult.set< VectorComparison::MASK_X >( );
   }
   else
   {
      maxIdx = 1;
      outResult.set< VectorComparison::MASK_Y >( );
   }

   if ( m_quad.v[maxIdx] <= m_quad.v[2] )
   {
      maxIdx = 2;
      outResult.set< VectorComparison::MASK_Z >( );
   }

   if ( m_quad.v[maxIdx] <= m_quad.v[3] )
   {
      maxIdx = 3;
      outResult.set< VectorComparison::MASK_W >( );
   }
}

///////////////////////////////////////////////////////////////////////////////

template<>
void Vector::store( TVector< 2 >& rawVector ) const
{
   rawVector.v[0] = m_quad.v[0];
   rawVector.v[1] = m_quad.v[1];
}

///////////////////////////////////////////////////////////////////////////////

template<>
void Vector::store( TVector< 3 >& rawVector ) const
{
   rawVector.v[0] = m_quad.v[0];
   rawVector.v[1] = m_quad.v[1];
   rawVector.v[2] = m_quad.v[2];
}

///////////////////////////////////////////////////////////////////////////////

template<>
void Vector::store( TVector< 4 >& rawVector ) const
{
   rawVector.v[0] = m_quad.v[0];
   rawVector.v[1] = m_quad.v[1];
   rawVector.v[2] = m_quad.v[2];
   rawVector.v[3] = m_quad.v[3];
}

///////////////////////////////////////////////////////////////////////////////

template<>
void Vector::load( const TVector< 2 >& rawVector )
{
   m_quad.v[0] = rawVector.v[0];
   m_quad.v[1] = rawVector.v[1];
}

///////////////////////////////////////////////////////////////////////////////

template<>
void Vector::load( const TVector< 3 >& rawVector )
{
   m_quad.v[0] = rawVector.v[0];
   m_quad.v[1] = rawVector.v[1];
   m_quad.v[2] = rawVector.v[2];
}

///////////////////////////////////////////////////////////////////////////////

template<>
void Vector::load( const TVector< 4 >& rawVector )
{
   m_quad.v[0] = rawVector.v[0];
   m_quad.v[1] = rawVector.v[1];
   m_quad.v[2] = rawVector.v[2];
   m_quad.v[3] = rawVector.v[3];
}

///////////////////////////////////////////////////////////////////////////////

// End of the code that runs on FPU

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#else // _USE_SIMD

// Code that runs on SIMD

#include "core\SimdUtils.h"

///////////////////////////////////////////////////////////////////////////////

template<>
const FastFloat Vector::getMin<2>() const
{
   const __m128 minVal = _mm_min_ps( _mm_shuffle_ps( m_quad, m_quad, _MM_SHUFFLE( 1, 1, 1, 1) ), _mm_shuffle_ps( m_quad, m_quad, _MM_SHUFFLE( 0, 0, 0, 0 ) ) );
   return *( const FastFloat* )&minVal;
}

///////////////////////////////////////////////////////////////////////////////

template<>
const FastFloat Vector::getMin<3>() const
{
   const __m128 xy = _mm_min_ps( _mm_shuffle_ps( m_quad, m_quad, _MM_SHUFFLE( 1, 1, 1, 1 ) ), _mm_shuffle_ps( m_quad, m_quad, _MM_SHUFFLE( 0, 0, 0, 0 ) ) );
   const __m128 minVal = _mm_min_ps( _mm_shuffle_ps( m_quad, m_quad, _MM_SHUFFLE( 2, 2, 2, 2 ) ), xy );
   return *( const FastFloat* )&minVal;
}

///////////////////////////////////////////////////////////////////////////////

template<>
const FastFloat Vector::getMin<4>() const
{
   __m128 sum0 = _mm_min_ps( _mm_shuffle_ps( m_quad, m_quad, _MM_SHUFFLE( 1, 0, 3, 2) ), m_quad );
   const __m128 sum1 = _mm_shuffle_ps( sum0, sum0, _MM_SHUFFLE( 2, 3, 0, 1 ) );
   sum0 = _mm_min_ps( sum0, sum1 );

   return *( const FastFloat* )&sum0;
}

///////////////////////////////////////////////////////////////////////////////

template<>
const FastFloat Vector::getMax<2>() const
{
   const __m128 maxVal = _mm_max_ps( _mm_shuffle_ps( m_quad, m_quad, _MM_SHUFFLE( 1, 1, 1, 1) ), _mm_shuffle_ps( m_quad, m_quad, _MM_SHUFFLE( 0, 0, 0, 0 ) ) );
   return *( const FastFloat* )&maxVal;
}

///////////////////////////////////////////////////////////////////////////////

template<>
const FastFloat Vector::getMax<3>() const
{
   const __m128 xy = _mm_max_ps( _mm_shuffle_ps( m_quad, m_quad, _MM_SHUFFLE( 1, 1, 1, 1 ) ), _mm_shuffle_ps( m_quad, m_quad, _MM_SHUFFLE( 0, 0, 0, 0 ) ) );
   const __m128 maxVal = _mm_max_ps( _mm_shuffle_ps( m_quad, m_quad, _MM_SHUFFLE( 2, 2, 2, 2 ) ), xy );
   return *( const FastFloat* )&maxVal;
}

///////////////////////////////////////////////////////////////////////////////

template<>
const FastFloat Vector::getMax<4>() const
{
   __m128 sum0 = _mm_max_ps( _mm_shuffle_ps( m_quad, m_quad, _MM_SHUFFLE( 1, 0, 3, 2) ), m_quad );
   const __m128 sum1 = _mm_shuffle_ps( sum0, sum0, _MM_SHUFFLE( 2, 3, 0, 1 ) );
   sum0 = _mm_max_ps( sum0, sum1 );

   return *( const FastFloat* )&sum0;
}

///////////////////////////////////////////////////////////////////////////////


template<>
void Vector::selectMin<2>( VectorComparison& outResult ) const
{
   if ( m_quad.m128_f32[0] < m_quad.m128_f32[1] )
   {
      outResult.set< VectorComparison::MASK_X >( );
   }
   else
   {
      outResult.set< VectorComparison::MASK_Y >( );
   }
}

///////////////////////////////////////////////////////////////////////////////

template<>
void Vector::selectMin<3>( VectorComparison& outResult ) const
{
   int minIdx;

   if ( m_quad.m128_f32[0] < m_quad.m128_f32[1] )
   {
      minIdx = 0;
      outResult.set< VectorComparison::MASK_X >( );
   }
   else
   {
      minIdx = 1;
      outResult.set< VectorComparison::MASK_Y >( );
   }

   if ( m_quad.m128_f32[minIdx] >= m_quad.m128_f32[2] )
   {
      minIdx = 2;
      outResult.set< VectorComparison::MASK_Z >( );
   }
}

///////////////////////////////////////////////////////////////////////////////

template<>
void Vector::selectMin<4>( VectorComparison& outResult ) const
{
   int minIdx;

   if ( m_quad.m128_f32[0] < m_quad.m128_f32[1] )
   {
      minIdx = 0;
      outResult.set< VectorComparison::MASK_X >( );
   }
   else
   {
      minIdx = 1;
      outResult.set< VectorComparison::MASK_Y >( );
   }

   if ( m_quad.m128_f32[minIdx] >= m_quad.m128_f32[2] )
   {
      minIdx = 2;
      outResult.set< VectorComparison::MASK_Z >( );
   }

   if ( m_quad.m128_f32[minIdx] >= m_quad.m128_f32[3] )
   {
      minIdx = 3;
      outResult.set< VectorComparison::MASK_W >( );
   }
}

///////////////////////////////////////////////////////////////////////////////

template<>
void Vector::selectMax<2>( VectorComparison& outResult ) const
{
   if ( m_quad.m128_f32[0] > m_quad.m128_f32[1] )
   {
      outResult.set< VectorComparison::MASK_X >( );
   }
   else
   {
      outResult.set< VectorComparison::MASK_Y >( );
   }
}

///////////////////////////////////////////////////////////////////////////////

template<>
void Vector::selectMax<3>( VectorComparison& outResult ) const
{
   int maxIdx;

   if ( m_quad.m128_f32[0] > m_quad.m128_f32[1] )
   {
      maxIdx = 0;
      outResult.set< VectorComparison::MASK_X >( );
   }
   else
   {
      maxIdx = 1;
      outResult.set< VectorComparison::MASK_Y >( );
   }

   if ( m_quad.m128_f32[maxIdx] <= m_quad.m128_f32[2] )
   {
      maxIdx = 2;
      outResult.set< VectorComparison::MASK_Z >( );
   }
}

///////////////////////////////////////////////////////////////////////////////

template<>
void Vector::selectMax<4>( VectorComparison& outResult ) const
{
   int maxIdx;

   if ( m_quad.m128_f32[0] > m_quad.m128_f32[1] )
   {
      maxIdx = 0;
      outResult.set< VectorComparison::MASK_X >( );
   }
   else
   {
      maxIdx = 1;
      outResult.set< VectorComparison::MASK_Y >( );
   }

   if ( m_quad.m128_f32[maxIdx] <= m_quad.m128_f32[2] )
   {
      maxIdx = 2;
      outResult.set< VectorComparison::MASK_Z >( );
   }

   if ( m_quad.m128_f32[maxIdx] <= m_quad.m128_f32[3] )
   {
      maxIdx = 3;
      outResult.set< VectorComparison::MASK_W >( );
   }
}

///////////////////////////////////////////////////////////////////////////////

template<>
void Vector::store( TVector< 2 >& rawVector ) const
{
   rawVector.v[0] = m_quad.m128_f32[0];
   rawVector.v[1] = m_quad.m128_f32[1];
}

///////////////////////////////////////////////////////////////////////////////

template<>
void Vector::store( TVector< 3 >& rawVector ) const
{
   rawVector.v[0] = m_quad.m128_f32[0];
   rawVector.v[1] = m_quad.m128_f32[1];
   rawVector.v[2] = m_quad.m128_f32[2];
}

///////////////////////////////////////////////////////////////////////////////

template<>
void Vector::store( TVector< 4 >& rawVector ) const
{
   rawVector.v[0] = m_quad.m128_f32[0];
   rawVector.v[1] = m_quad.m128_f32[1];
   rawVector.v[2] = m_quad.m128_f32[2];
   rawVector.v[3] = m_quad.m128_f32[3];
}

///////////////////////////////////////////////////////////////////////////////

template<>
void Vector::load( const TVector< 2 >& rawVector )
{
   SimdUtils::toSimd( rawVector.v[0], rawVector.v[1], 0.0f, 0.0f, &m_quad );
}

///////////////////////////////////////////////////////////////////////////////

template<>
void Vector::load( const TVector< 3 >& rawVector )
{
   SimdUtils::toSimd( rawVector.v[0], rawVector.v[1], rawVector.v[2], 0.0f, &m_quad );
}

///////////////////////////////////////////////////////////////////////////////

template<>
void Vector::load( const TVector< 4 >& rawVector )
{
   SimdUtils::toSimd( rawVector.v[0], rawVector.v[1], rawVector.v[2], rawVector.v[3], &m_quad );
}

///////////////////////////////////////////////////////////////////////////////

// End of the code that runs on SIMD

#endif // _USE_SIMD

///////////////////////////////////////////////////////////////////////////////
