#ifndef _MATRIX_H
#error "This file can be included only in Matrix.h"
#else

#include "core\Matrix.h"
#include "core\Vector.h"
#include "core\EulerAngles.h"
#include "core\Quaternion.h"
#include "core\Assert.h"
#include "core\MathDefs.h"
#include "core\FastFloat.h"
#include "core\SimdUtils.h"


///////////////////////////////////////////////////////////////////////////////

namespace 
{
   static ALIGN_16 const uint OX[4] = { 0x3f800000, 0x00000000, 0x00000000, 0x00000000 };
   static ALIGN_16 const uint OY[4] = { 0x00000000, 0x3f800000, 0x00000000, 0x00000000 };
   static ALIGN_16 const uint OZ[4] = { 0x00000000, 0x00000000, 0x3f800000, 0x00000000 };
   static ALIGN_16 const uint OW[4] = { 0x00000000, 0x00000000, 0x00000000, 0x3f800000 };
   static ALIGN_16 const uint XYZW[4] = { 0x3f800000, 0x3f800000, 0x3f800000, 0x3f800000 };
}

///////////////////////////////////////////////////////////////////////////////

Matrix::Matrix()
{
   m_rows[0] = *( const QuadStorage*)&OX;
   m_rows[1] = *( const QuadStorage*)&OY;
   m_rows[2] = *( const QuadStorage*)&OZ;
   m_rows[3] = *( const QuadStorage*)&OW;
}

///////////////////////////////////////////////////////////////////////////////

Matrix::Matrix( float _11, float _12, float _13, float _14,
                float _21, float _22, float _23, float _24,
                float _31, float _32, float _33, float _34,
                float _41, float _42, float _43, float _44 )
{
   SimdUtils::toSimd( _11, _12, _13, _14, &m_rows[0] );
   SimdUtils::toSimd( _21, _22, _23, _24, &m_rows[1] );
   SimdUtils::toSimd( _31, _32, _33, _34, &m_rows[2] );
   SimdUtils::toSimd( _41, _42, _43, _44, &m_rows[3] );
}

///////////////////////////////////////////////////////////////////////////////

void Matrix::setIdentity()
{
   m_rows[0] = *( const QuadStorage*)&OX;
   m_rows[1] = *( const QuadStorage*)&OY;
   m_rows[2] = *( const QuadStorage*)&OZ;
   m_rows[3] = *( const QuadStorage*)&OW;}

///////////////////////////////////////////////////////////////////////////////

void Matrix::set( const float* values )
{
   m_rows[0] = _mm_load_ps( values );
   m_rows[1] = _mm_load_ps( values + 4 );
   m_rows[2] = _mm_load_ps( values + 8 );
   m_rows[3] = _mm_load_ps( values + 12 );
}

///////////////////////////////////////////////////////////////////////////////

void Matrix::set( float _11, float _12, float _13, float _14, 
   float _21, float _22, float _23, float _24, 
   float _31, float _32, float _33, float _34,
   float _41, float _42, float _43, float _44 )
{
   SimdUtils::toSimd( _11, _12, _13, _14, &m_rows[0] );
   SimdUtils::toSimd( _21, _22, _23, _24, &m_rows[1] );
   SimdUtils::toSimd( _31, _32, _33, _34, &m_rows[2] );
   SimdUtils::toSimd( _41, _42, _43, _44, &m_rows[3] );
}

///////////////////////////////////////////////////////////////////////////////

void Matrix::setColumn( int colIdx, const Vector& vec )
{
   m_rows[0].m128_f32[colIdx] = vec.m_quad.m128_f32[0];
   m_rows[1].m128_f32[colIdx] = vec.m_quad.m128_f32[1];
   m_rows[2].m128_f32[colIdx] = vec.m_quad.m128_f32[2];
   m_rows[3].m128_f32[colIdx] = vec.m_quad.m128_f32[3];
}

///////////////////////////////////////////////////////////////////////////////

bool Matrix::operator==( const Matrix& rhs ) const
{
   __m128 cmpResult = _mm_cmpeq_ps( m_rows[0], rhs.m_rows[0] );
   int result = _mm_movemask_ps( cmpResult );

   cmpResult = _mm_cmpeq_ps( m_rows[1], rhs.m_rows[1] );
   result += _mm_movemask_ps( cmpResult );

   cmpResult = _mm_cmpeq_ps( m_rows[2], rhs.m_rows[2] );
   result += _mm_movemask_ps( cmpResult );

   cmpResult = _mm_cmpeq_ps( m_rows[3], rhs.m_rows[3] );
   result += _mm_movemask_ps( cmpResult );

   return result == 0x3c; // 4 * 0xf
}

///////////////////////////////////////////////////////////////////////////////

bool Matrix::operator!=(const Matrix& rhs) const
{
   __m128 cmpResult = _mm_cmpeq_ps( m_rows[0], rhs.m_rows[0] );
   int result = _mm_movemask_ps( cmpResult );

   cmpResult = _mm_cmpeq_ps( m_rows[1], rhs.m_rows[1] );
   result += _mm_movemask_ps( cmpResult );

   cmpResult = _mm_cmpeq_ps( m_rows[2], rhs.m_rows[2] );
   result += _mm_movemask_ps( cmpResult );

   cmpResult = _mm_cmpeq_ps( m_rows[3], rhs.m_rows[3] );
   result += _mm_movemask_ps( cmpResult );

   return result != 0x3c; // 4 * 0xf
}

///////////////////////////////////////////////////////////////////////////////

void Matrix::operator=( const Matrix& rhs )
{
   m_rows[0] = rhs.m_rows[0];
   m_rows[1] = rhs.m_rows[1];
   m_rows[2] = rhs.m_rows[2];
   m_rows[3] = rhs.m_rows[3];
}

///////////////////////////////////////////////////////////////////////////////

float Matrix::operator()( int row, int col ) const
{
   return m_rows[row].m128_f32[col];
}

///////////////////////////////////////////////////////////////////////////////

float& Matrix::operator()( int row, int col )
{
   return m_rows[row].m128_f32[col];
}

///////////////////////////////////////////////////////////////////////////////

void Matrix::setMul( const Matrix& a, const Matrix& b )
{
   ASSERT_MSG( &a != this, "Passing itself as one of this method parameters will yield unpredicted results" );
   ASSERT_MSG( &b != this, "Passing itself as one of this method parameters will yield unpredicted results" );

   __m128 t, c1, c2, c3, c4;
   const __m128* row;

   // unroll the loop once it works
   for ( int i = 0; i < 4; ++i )
   {
      row = &a.m_rows[i];
      SimdUtils::getComponent( row, 0, &t );
      c1 = _mm_mul_ps( b.m_rows[0], t );

      SimdUtils::getComponent( row, 1, &t );
      c2 = _mm_mul_ps( b.m_rows[1], t );

      SimdUtils::getComponent( row, 2, &t );
      c3 = _mm_mul_ps( b.m_rows[2], t );

      SimdUtils::getComponent( row, 3, &t );
      c4 = _mm_mul_ps( b.m_rows[3], t );

      c1 = _mm_add_ps( c1, c2 );
      c2 = _mm_add_ps( c3, c4 );
      m_rows[i] = _mm_add_ps( c1, c2 );
   }
}

///////////////////////////////////////////////////////////////////////////////

void Matrix::setMul( const Matrix& a, const FastFloat& t )
{
   m_rows[0] = _mm_mul_ps( a.m_rows[0], t.m_val );
   m_rows[1] = _mm_mul_ps( a.m_rows[1], t.m_val );
   m_rows[2] = _mm_mul_ps( a.m_rows[2], t.m_val );
   m_rows[3] = _mm_mul_ps( a.m_rows[3], t.m_val );
}

/////////////////////////////////////////////////////////////////

void Matrix::mul( const FastFloat& t )
{
   m_rows[0] = _mm_mul_ps( m_rows[0], t.m_val );
   m_rows[1] = _mm_mul_ps( m_rows[1], t.m_val );
   m_rows[2] = _mm_mul_ps( m_rows[2], t.m_val );
   m_rows[3] = _mm_mul_ps( m_rows[3], t.m_val );
}

/////////////////////////////////////////////////////////////////

const Vector& Matrix::sideVec() const
{
   return *( const Vector* )( &m_rows[0] );
}

///////////////////////////////////////////////////////////////////////////////

const Vector& Matrix::upVec() const
{
   return *( const Vector* )( &m_rows[1] );
}

///////////////////////////////////////////////////////////////////////////////

const Vector& Matrix::forwardVec() const
{
  return *( const Vector* )( &m_rows[2] );
}

///////////////////////////////////////////////////////////////////////////////

const Vector& Matrix::position() const
{
   return *( const Vector* )( &m_rows[3] );
}

///////////////////////////////////////////////////////////////////////////////

const Vector& Matrix::getRow( byte rowIdx ) const
{
   ASSERT_MSG( rowIdx >= 0 && rowIdx <= 3, "Matrix row index out of bounds" );
   return *( const Vector* )( &m_rows[rowIdx] );
}

///////////////////////////////////////////////////////////////////////////////

void Matrix::getScale( Vector& outScaleVec ) const
{
   outScaleVec.set( m_rows[0].m128_f32[0], m_rows[1].m128_f32[1], m_rows[2].m128_f32[2] );
}

///////////////////////////////////////////////////////////////////////////////

void Matrix::scaleUniform( const FastFloat& scale )
{
   __m128 tmp = _mm_mul_ps( *( const QuadStorage*)&OX, scale.m_val );
   m_rows[0] = _mm_mul_ps( m_rows[0], tmp );

   tmp = _mm_mul_ps( *( const QuadStorage*)&OY, scale.m_val );
   m_rows[1] = _mm_mul_ps( m_rows[1], tmp );

   tmp = _mm_mul_ps( *( const QuadStorage*)&OZ, scale.m_val );
   m_rows[2] = _mm_mul_ps( m_rows[2], tmp );
}

///////////////////////////////////////////////////////////////////////////////

void Matrix::scale( const Vector& scaleVec )
{
   __m128 simdVec;
   SimdUtils::setXYZ_W( &scaleVec.m_quad, &Float_1.m_val, &simdVec );
   
   __m128 comp;
   SimdUtils::getComponent( &simdVec, 0, &comp );
   __m128 tmp = _mm_mul_ps( *( const QuadStorage*)&OX, comp );
   m_rows[0] = _mm_mul_ps( m_rows[0], tmp );

   SimdUtils::getComponent( &simdVec, 1, &comp );
   tmp = _mm_mul_ps( *( const QuadStorage*)&OY, comp );
   m_rows[1] = _mm_mul_ps( m_rows[1], tmp );

   SimdUtils::getComponent( &simdVec, 2, &comp );
   tmp = _mm_mul_ps( *( const QuadStorage*)&OZ, comp );
   m_rows[2] = _mm_mul_ps( m_rows[2], tmp );
}

///////////////////////////////////////////////////////////////////////////////

void Matrix::setInverse( const Matrix& rhs )
{
   // implementation of the algorithm taken from here: ftp://download.intel.com/design/PentiumIII/sml/24504301.pdf

   __m128 minor0, minor1, minor2, minor3;
   __m128 row0, row1, row2, row3;
   __m128 det, tmp1;

   ALIGN_16 float src[16];
   rhs.getRowMajor( src );

   tmp1 = _mm_setzero_ps();
   row0 = _mm_setzero_ps();
   row1 = _mm_setzero_ps();
   row2 = _mm_setzero_ps();
   row3 = _mm_setzero_ps();

   tmp1 = _mm_loadh_pi(_mm_loadl_pi(tmp1, (__m64*)(src)), (__m64*)(src+ 4));
   row1 = _mm_loadh_pi(_mm_loadl_pi(row1, (__m64*)(src+8)), (__m64*)(src+12));
   row0 = _mm_shuffle_ps(tmp1, row1, 0x88);
   row1 = _mm_shuffle_ps(row1, tmp1, 0xDD);
   tmp1 = _mm_loadh_pi(_mm_loadl_pi(tmp1, (__m64*)(src+ 2)), (__m64*)(src+ 6));
   row3 = _mm_loadh_pi(_mm_loadl_pi(row3, (__m64*)(src+10)), (__m64*)(src+14));
   row2 = _mm_shuffle_ps(tmp1, row3, 0x88);
   row3 = _mm_shuffle_ps(row3, tmp1, 0xDD);

   // -----------------------------------------------
   tmp1 = _mm_mul_ps(row2, row3);
   tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0xB1);
   minor0 = _mm_mul_ps(row1, tmp1);
   minor1 = _mm_mul_ps(row0, tmp1);
   tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0x4E);
   minor0 = _mm_sub_ps(_mm_mul_ps(row1, tmp1), minor0);
   minor1 = _mm_sub_ps(_mm_mul_ps(row0, tmp1), minor1);
   minor1 = _mm_shuffle_ps(minor1, minor1, 0x4E);

   // -----------------------------------------------
   tmp1 = _mm_mul_ps(row1, row2);
   tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0xB1);
   minor0 = _mm_add_ps(_mm_mul_ps(row3, tmp1), minor0);
   minor3 = _mm_mul_ps(row0, tmp1);
   tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0x4E);
   minor0 = _mm_sub_ps(minor0, _mm_mul_ps(row3, tmp1));
   minor3 = _mm_sub_ps(_mm_mul_ps(row0, tmp1), minor3);
   minor3 = _mm_shuffle_ps(minor3, minor3, 0x4E);
   // -----------------------------------------------

   tmp1 = _mm_mul_ps(_mm_shuffle_ps(row1, row1, 0x4E), row3);
   tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0xB1);
   row2 = _mm_shuffle_ps(row2, row2, 0x4E);
   minor0 = _mm_add_ps(_mm_mul_ps(row2, tmp1), minor0);
   minor2 = _mm_mul_ps(row0, tmp1);
   tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0x4E);
   minor0 = _mm_sub_ps(minor0, _mm_mul_ps(row2, tmp1));
   minor2 = _mm_sub_ps(_mm_mul_ps(row0, tmp1), minor2);
   minor2 = _mm_shuffle_ps(minor2, minor2, 0x4E);

   // -----------------------------------------------
   tmp1 = _mm_mul_ps(row0, row1);
   tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0xB1);
   minor2 = _mm_add_ps(_mm_mul_ps(row3, tmp1), minor2);
   minor3 = _mm_sub_ps(_mm_mul_ps(row2, tmp1), minor3);
   tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0x4E);
   minor2 = _mm_sub_ps(_mm_mul_ps(row3, tmp1), minor2);
   minor3 = _mm_sub_ps(minor3, _mm_mul_ps(row2, tmp1));
   
   // -----------------------------------------------
   tmp1 = _mm_mul_ps(row0, row3);
   tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0xB1);
   minor1 = _mm_sub_ps(minor1, _mm_mul_ps(row2, tmp1));
   minor2 = _mm_add_ps(_mm_mul_ps(row1, tmp1), minor2);
   tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0x4E);
   minor1 = _mm_add_ps(_mm_mul_ps(row2, tmp1), minor1);
   minor2 = _mm_sub_ps(minor2, _mm_mul_ps(row1, tmp1));
   
   // -----------------------------------------------
   tmp1 = _mm_mul_ps(row0, row2);
   tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0xB1);
   minor1 = _mm_add_ps(_mm_mul_ps(row3, tmp1), minor1);
   minor3 = _mm_sub_ps(minor3, _mm_mul_ps(row1, tmp1));
   tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0x4E);
   minor1 = _mm_sub_ps(minor1, _mm_mul_ps(row3, tmp1));
   minor3 = _mm_add_ps(_mm_mul_ps(row1, tmp1), minor3);
   
   // -----------------------------------------------
   det = _mm_mul_ps(row0, minor0);
   det = _mm_add_ps(_mm_shuffle_ps(det, det, 0x4E), det);
   det = _mm_add_ss(_mm_shuffle_ps(det, det, 0xB1), det);
   tmp1 = _mm_rcp_ss(det);
   det = _mm_sub_ss(_mm_add_ss(tmp1, tmp1), _mm_mul_ss(det, _mm_mul_ss(tmp1, tmp1)));
   det = _mm_shuffle_ps(det, det, 0x00);
   
   // transpose back
   minor0 = _mm_mul_ps(det, minor0);
   _mm_storel_pi((__m64*)(src), minor0);
   _mm_storeh_pi((__m64*)(src+2), minor0);
   minor1 = _mm_mul_ps(det, minor1);
   _mm_storel_pi((__m64*)(src+4), minor1);
   _mm_storeh_pi((__m64*)(src+6), minor1);
   minor2 = _mm_mul_ps(det, minor2);
   _mm_storel_pi((__m64*)(src+ 8), minor2);
   _mm_storeh_pi((__m64*)(src+10), minor2);
   minor3 = _mm_mul_ps(det, minor3);
   _mm_storel_pi((__m64*)(src+12), minor3);
   _mm_storeh_pi((__m64*)(src+14), minor3);

   // store the output
   set( src );
}

///////////////////////////////////////////////////////////////////////////////

void Matrix::setTransposed( const Matrix& rhs )
{
   SimdUtils::transpose( rhs.m_rows, m_rows );
}

///////////////////////////////////////////////////////////////////////////////

void Matrix::getVectors( Vector& outSideVec, Vector& outUpVec, Vector& outForwardVec, Vector& outPos ) const
{
   outSideVec.m_quad       = m_rows[0];
   outUpVec.m_quad         = m_rows[1];
   outForwardVec.m_quad    = m_rows[2];
   outPos.m_quad           = m_rows[3];
}

///////////////////////////////////////////////////////////////////////////////

void Matrix::setRows( const Vector& sideVec, const Vector& upVec, const Vector& forwardVec, const Vector& pos )
{
   m_rows[0]               = sideVec.m_quad;
   m_rows[1]               = upVec.m_quad;
   m_rows[2]               = forwardVec.m_quad;
   m_rows[3]               = pos.m_quad;
}

///////////////////////////////////////////////////////////////////////////////

void Matrix::transform( const Vector& rhs, Vector& outVec ) const
{
   __m128 simdVec;
   SimdUtils::setXYZ_W( &rhs.m_quad, &Float_1.m_val, &simdVec );

   __m128 x, y, z, w; 
   SimdUtils::getComponent( &simdVec, 0, &x );
   SimdUtils::getComponent( &simdVec, 1, &y );
   SimdUtils::getComponent( &simdVec, 2, &z );
   SimdUtils::getComponent( &simdVec, 3, &w );

   __m128 m, t;
   t = _mm_mul_ps( x, m_rows[0] );
   m = _mm_mul_ps( y, m_rows[1] );
   t = _mm_add_ps( t, m );

   m = _mm_mul_ps( z, m_rows[2] );
   t = _mm_add_ps( t, m );

   m = _mm_mul_ps( w, m_rows[3] );
   t = _mm_add_ps( t, m );

   // divide by W
   SimdUtils::getComponent( &t, 3, &w );
   outVec.m_quad = _mm_div_ps( t, w );
}

///////////////////////////////////////////////////////////////////////////////

void Matrix::transformNorm( const Vector& rhs, Vector& outNormal ) const
{
   __m128 simdVec;
   SimdUtils::setXYZ_W( &rhs.m_quad, &Float_1.m_val, &simdVec );

   __m128 x, y, z; 
   SimdUtils::getComponent( &simdVec, 0, &x );
   SimdUtils::getComponent( &simdVec, 1, &y );
   SimdUtils::getComponent( &simdVec, 2, &z );

   __m128 m, t;
   t = _mm_mul_ps( x, m_rows[0] );
   m = _mm_mul_ps( y, m_rows[1] );
   t = _mm_add_ps( t, m );

   m = _mm_mul_ps( z, m_rows[2] );
   t = _mm_add_ps( t, m );

   SimdUtils::setXYZ_W( &t, &Float_1.m_val, &outNormal.m_quad );
}

///////////////////////////////////////////////////////////////////////////////

void Matrix::transform4( const Vector& rhs, Vector& outVec ) const
{
   __m128 simdVec = rhs.m_quad;

   __m128 x, y, z, w; 
   SimdUtils::getComponent( &simdVec, 0, &x );
   SimdUtils::getComponent( &simdVec, 1, &y );
   SimdUtils::getComponent( &simdVec, 2, &z );
   SimdUtils::getComponent( &simdVec, 3, &w );

   __m128 m, t;
   t = _mm_mul_ps( x, m_rows[0] );
   m = _mm_mul_ps( y, m_rows[1] );
   t = _mm_add_ps( t, m );

   m = _mm_mul_ps( z, m_rows[2] );
   t = _mm_add_ps( t, m );

   m = _mm_mul_ps( w, m_rows[3] );
   outVec.m_quad = _mm_add_ps( t, m );
}

///////////////////////////////////////////////////////////////////////////////

void Matrix::getRowMajor( float* __restrict outRowMajorMtx ) const
{
   _mm_store_ps( outRowMajorMtx, m_rows[0] );
   _mm_store_ps( outRowMajorMtx + 4, m_rows[1] );
   _mm_store_ps( outRowMajorMtx + 8, m_rows[2] );
   _mm_store_ps( outRowMajorMtx + 12, m_rows[3] );
}

///////////////////////////////////////////////////////////////////////////////

void Matrix::getColumnMajor( float* __restrict outColumnMajorMtx ) const
{
}

///////////////////////////////////////////////////////////////////////////////

void Matrix::getRightHanded( float* __restrict outRowMajorMtx ) const
{
   _mm_store_ps( outRowMajorMtx + 4, m_rows[0] );
   _mm_store_ps( outRowMajorMtx, m_rows[1] );
   _mm_store_ps( outRowMajorMtx + 8, m_rows[2] );
   _mm_store_ps( outRowMajorMtx + 12, m_rows[3] );
}

///////////////////////////////////////////////////////////////////////////////

#endif // _MATRIX_H
