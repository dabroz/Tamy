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


///////////////////////////////////////////////////////////////////////////////

Matrix::Matrix()
{
   memset( &m_rows, 0, sizeof(float) * 16 );
   m_rows[0].v[0] = m_rows[1].v[1] = m_rows[2].v[2] = m_rows[3].v[3] = 1;
}

///////////////////////////////////////////////////////////////////////////////

Matrix::Matrix( float _11, float _12, float _13, float _14,
                float _21, float _22, float _23, float _24,
                float _31, float _32, float _33, float _34,
                float _41, float _42, float _43, float _44 )
{
   m_rows[0].v[0] = _11;
   m_rows[0].v[1] = _12;
   m_rows[0].v[2] = _13;
   m_rows[0].v[3] = _14;

   m_rows[1].v[0] = _21;
   m_rows[1].v[1] = _22;
   m_rows[1].v[2] = _23;
   m_rows[1].v[3] = _24;

   m_rows[2].v[0] = _31;
   m_rows[2].v[1] = _32;
   m_rows[2].v[2] = _33;
   m_rows[2].v[3] = _34;

   m_rows[3].v[0] = _41;
   m_rows[3].v[1] = _42;
   m_rows[3].v[2] = _43;
   m_rows[3].v[3] = _44;
}

///////////////////////////////////////////////////////////////////////////////

void Matrix::setIdentity()
{
   memset( &m_rows, 0, sizeof(float) * 16 );
   m_rows[0].v[0] = m_rows[1].v[1] = m_rows[2].v[2] = m_rows[3].v[3] = 1;
}

///////////////////////////////////////////////////////////////////////////////

void Matrix::set( const float* values )
{
   memcpy( &m_rows, values, sizeof(float) * 16 );
}

///////////////////////////////////////////////////////////////////////////////

void Matrix::set( float _11, float _12, float _13, float _14, 
                  float _21, float _22, float _23, float _24, 
                  float _31, float _32, float _33, float _34,
                  float _41, float _42, float _43, float _44 )
{
   m_rows[0].v[0] = _11;
   m_rows[0].v[1] = _12;
   m_rows[0].v[2] = _13;
   m_rows[0].v[3] = _14;

   m_rows[1].v[0] = _21;
   m_rows[1].v[1] = _22;
   m_rows[1].v[2] = _23;
   m_rows[1].v[3] = _24;

   m_rows[2].v[0] = _31;
   m_rows[2].v[1] = _32;
   m_rows[2].v[2] = _33;
   m_rows[2].v[3] = _34;

   m_rows[3].v[0] = _41;
   m_rows[3].v[1] = _42;
   m_rows[3].v[2] = _43;
   m_rows[3].v[3] = _44;
}

///////////////////////////////////////////////////////////////////////////////

void Matrix::setColumn( int colIdx, const Vector& vec )
{
   m_rows[0].v[colIdx] = vec.m_quad.v[0];
   m_rows[1].v[colIdx] = vec.m_quad.v[1];
   m_rows[2].v[colIdx] = vec.m_quad.v[2];
   m_rows[3].v[colIdx] = vec.m_quad.v[3];
}

///////////////////////////////////////////////////////////////////////////////

bool Matrix::operator==(const Matrix& rhs) const
{
   for ( int row = 0; row < 4; ++row )
   {
      for ( int col = 0; col < 4; ++col )
      {
         if ( m_rows[row].v[col] != rhs.m_rows[row].v[col] )
         {
            return false;
         }
      }
   }

   return true;
}

///////////////////////////////////////////////////////////////////////////////

bool Matrix::operator!=(const Matrix& rhs) const
{
   return !(*this == rhs);
}

///////////////////////////////////////////////////////////////////////////////

void Matrix::operator=( const Matrix& rhs )
{
   for ( int row = 0; row < 4; ++row )
   {
      for ( int col = 0; col < 4; ++col )
      {
         m_rows[row].v[col] = rhs.m_rows[row].v[col];
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

float Matrix::operator()( int row, int col ) const
{
   return m_rows[row].v[col];
}

///////////////////////////////////////////////////////////////////////////////

float& Matrix::operator()( int row, int col )
{
   return m_rows[row].v[col];
}

///////////////////////////////////////////////////////////////////////////////

void Matrix::setMul( const Matrix& a, const Matrix& b )
{
   ASSERT_MSG( &a != this, "Passing itself as one of this method parameters will yield unpredicted results" );
   ASSERT_MSG( &b != this, "Passing itself as one of this method parameters will yield unpredicted results" );

   for ( int row = 0; row < 4; ++row )
   {
      for ( int col = 0; col < 4; ++col )
      {
         m_rows[row].v[col] = 0;

         for ( int i = 0; i < 4; ++i )
         {
            m_rows[row].v[col] += a.m_rows[row].v[i] * b.m_rows[i].v[col];
         }
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void Matrix::setMul( const Matrix& a, const FastFloat& t )
{
   for ( int row = 0; row < 4; ++row )
   {
      for ( int col = 0; col < 4; ++col )
      {
         m_rows[row].v[col] = a.m_rows[row].v[col] * t.m_val;
      }
   }
}

/////////////////////////////////////////////////////////////////

void Matrix::mul( const FastFloat& t )
{
   for ( int row = 0; row < 4; ++row )
   {
      for ( int col = 0; col < 4; ++col )
      {
         m_rows[row].v[col] *= t.m_val;
      }
   }
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
   outScaleVec.set( m_rows[0].v[0], m_rows[1].v[1], m_rows[2].v[2] );
}

///////////////////////////////////////////////////////////////////////////////

void Matrix::scaleUniform( const FastFloat& scale )
{
   m_rows[0].v[0] *= scale.m_val;
   m_rows[1].v[1] *= scale.m_val;
   m_rows[2].v[2] *= scale.m_val;
}

///////////////////////////////////////////////////////////////////////////////

void Matrix::scale( const Vector& scaleVec )
{
   m_rows[0].v[0] *= scaleVec.m_quad.v[0];
   m_rows[1].v[1] *= scaleVec.m_quad.v[1];
   m_rows[2].v[2] *= scaleVec.m_quad.v[2];
}

///////////////////////////////////////////////////////////////////////////////

void Matrix::setInverse( const Matrix& rhs )
{
   // implementation of the algorithm taken from here: ftp://download.intel.com/design/PentiumIII/sml/24504301.pdf

   float tmp[12]; // temp array of pairs
   float src[16]; // array of transpose source matrix
   float det; // determinant

   const ALIGN_16 float* mat = &rhs.m_rows[0].v[0];

   // transpose matrix
   for ( int i = 0; i < 4; ++i ) 
   {
      src[i] = mat[i*4];
      src[i + 4] = mat[i*4 + 1];
      src[i + 8] = mat[i*4 + 2];
      src[i + 12] = mat[i*4 + 3];
   }

   // calculate pairs for first 8 elements (cofactors)
   tmp[0] = src[10] * src[15];
   tmp[1] = src[11] * src[14];
   tmp[2] = src[9] * src[15];
   tmp[3] = src[11] * src[13];
   tmp[4] = src[9] * src[14];
   tmp[5] = src[10] * src[13];
   tmp[6] = src[8] * src[15];
   tmp[7] = src[11] * src[12];
   tmp[8] = src[8] * src[14];
   tmp[9] = src[10] * src[12];
   tmp[10] = src[8] * src[13];
   tmp[11] = src[9] * src[12];

   // calculate first 8 elements (cofactors)
   ALIGN_16 float* dst = &m_rows[0].v[0];
   dst[0] = tmp[0]*src[5] + tmp[3]*src[6] + tmp[4]*src[7];
   dst[0] -= tmp[1]*src[5] + tmp[2]*src[6] + tmp[5]*src[7];
   dst[1] = tmp[1]*src[4] + tmp[6]*src[6] + tmp[9]*src[7];
   dst[1] -= tmp[0]*src[4] + tmp[7]*src[6] + tmp[8]*src[7];
   dst[2] = tmp[2]*src[4] + tmp[7]*src[5] + tmp[10]*src[7];
   dst[2] -= tmp[3]*src[4] + tmp[6]*src[5] + tmp[11]*src[7];
   dst[3] = tmp[5]*src[4] + tmp[8]*src[5] + tmp[11]*src[6];
   dst[3] -= tmp[4]*src[4] + tmp[9]*src[5] + tmp[10]*src[6];
   dst[4] = tmp[1]*src[1] + tmp[2]*src[2] + tmp[5]*src[3];
   dst[4] -= tmp[0]*src[1] + tmp[3]*src[2] + tmp[4]*src[3];
   dst[5] = tmp[0]*src[0] + tmp[7]*src[2] + tmp[8]*src[3];
   dst[5] -= tmp[1]*src[0] + tmp[6]*src[2] + tmp[9]*src[3];
   dst[6] = tmp[3]*src[0] + tmp[6]*src[1] + tmp[11]*src[3];
   dst[6] -= tmp[2]*src[0] + tmp[7]*src[1] + tmp[10]*src[3];
   dst[7] = tmp[4]*src[0] + tmp[9]*src[1] + tmp[10]*src[2];
   dst[7] -= tmp[5]*src[0] + tmp[8]*src[1] + tmp[11]*src[2];

   // calculate pairs for second 8 elements (cofactors)
   tmp[0] = src[2]*src[7];
   tmp[1] = src[3]*src[6];
   tmp[2] = src[1]*src[7];
   tmp[3] = src[3]*src[5];
   tmp[4] = src[1]*src[6];
   tmp[5] = src[2]*src[5];
   tmp[6] = src[0]*src[7];
   tmp[7] = src[3]*src[4];
   tmp[8] = src[0]*src[6];
   tmp[9] = src[2]*src[4];
   tmp[10] = src[0]*src[5];
   tmp[11] = src[1]*src[4];

   // calculate second 8 elements (cofactors)
   dst[8] = tmp[0]*src[13] + tmp[3]*src[14] + tmp[4]*src[15];
   dst[8] -= tmp[1]*src[13] + tmp[2]*src[14] + tmp[5]*src[15];
   dst[9] = tmp[1]*src[12] + tmp[6]*src[14] + tmp[9]*src[15];
   dst[9] -= tmp[0]*src[12] + tmp[7]*src[14] + tmp[8]*src[15];
   dst[10] = tmp[2]*src[12] + tmp[7]*src[13] + tmp[10]*src[15];
   dst[10]-= tmp[3]*src[12] + tmp[6]*src[13] + tmp[11]*src[15];
   dst[11] = tmp[5]*src[12] + tmp[8]*src[13] + tmp[11]*src[14];
   dst[11]-= tmp[4]*src[12] + tmp[9]*src[13] + tmp[10]*src[14];
   dst[12] = tmp[2]*src[10] + tmp[5]*src[11] + tmp[1]*src[9];
   dst[12]-= tmp[4]*src[11] + tmp[0]*src[9] + tmp[3]*src[10];
   dst[13] = tmp[8]*src[11] + tmp[0]*src[8] + tmp[7]*src[10];
   dst[13]-= tmp[6]*src[10] + tmp[9]*src[11] + tmp[1]*src[8];
   dst[14] = tmp[6]*src[9] + tmp[11]*src[11] + tmp[3]*src[8];
   dst[14]-= tmp[10]*src[11] + tmp[2]*src[8] + tmp[7]*src[9];
   dst[15] = tmp[10]*src[10] + tmp[4]*src[8] + tmp[9]*src[9];
   dst[15]-= tmp[8]*src[9] + tmp[11]*src[10] + tmp[5]*src[8];

   // calculate determinant
   det=src[0]*dst[0]+src[1]*dst[1]+src[2]*dst[2]+src[3]*dst[3];

   // calculate matrix inverse
   det = 1/det;
   for ( int j = 0; j < 16; ++j )
   {
      dst[j] *= det;
   }
}

///////////////////////////////////////////////////////////////////////////////

void Matrix::setTransposed( const Matrix& rhs )
{
   for( int row = 0; row < 4; ++row )
   {
      for( int col = 0; col < 4; ++col )
      {
         m_rows[col].v[row] = rhs.m_rows[row].v[col];
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void Matrix::getVectors( Vector& outSideVec, Vector& outUpVec, Vector& outForwardVec, Vector& outPos ) const
{
   outSideVec.m_quad.v[0] = m_rows[0].v[0];
   outSideVec.m_quad.v[1] = m_rows[0].v[1];
   outSideVec.m_quad.v[2] = m_rows[0].v[2];

   outUpVec.m_quad.v[0] = m_rows[1].v[0];
   outUpVec.m_quad.v[1] = m_rows[1].v[1];
   outUpVec.m_quad.v[2] = m_rows[1].v[2];

   outForwardVec.m_quad.v[0] = m_rows[2].v[0];
   outForwardVec.m_quad.v[1] = m_rows[2].v[1];
   outForwardVec.m_quad.v[2] = m_rows[2].v[2];

   outPos.m_quad.v[0] = m_rows[3].v[0];
   outPos.m_quad.v[1] = m_rows[3].v[1];
   outPos.m_quad.v[2] = m_rows[3].v[2];
}

///////////////////////////////////////////////////////////////////////////////

void Matrix::setRows( const Vector& sideVec, const Vector& upVec, const Vector& forwardVec, const Vector& pos )
{
   m_rows[0].v[0] = sideVec.m_quad.v[0];
   m_rows[0].v[1] = sideVec.m_quad.v[1];
   m_rows[0].v[2] = sideVec.m_quad.v[2];
   m_rows[0].v[3] = 0.0f;

   m_rows[1].v[0] = upVec.m_quad.v[0];
   m_rows[1].v[1] = upVec.m_quad.v[1];
   m_rows[1].v[2] = upVec.m_quad.v[2];
   m_rows[1].v[3] = 0.0f;

   m_rows[2].v[0] = forwardVec.m_quad.v[0];
   m_rows[2].v[1] = forwardVec.m_quad.v[1];
   m_rows[2].v[2] = forwardVec.m_quad.v[2];
   m_rows[2].v[3] = 0.0f;

   m_rows[3].v[0] = pos.m_quad.v[0];
   m_rows[3].v[1] = pos.m_quad.v[1];
   m_rows[3].v[2] = pos.m_quad.v[2];
   m_rows[3].v[3] = 1.0f;
}

///////////////////////////////////////////////////////////////////////////////

void Matrix::transform( const Vector& rhs, Vector& outVec ) const
{
   float inV[4] = {0.0f, 0.0f, 0.0f, 1.0f};
   memcpy( inV, rhs.m_quad.v, sizeof( float ) * 3 );
   for ( int col = 0; col < 4; ++col )
   {
      outVec[col] = 0.0f;
      for ( int row = 0; row < 4; ++row )
      {
         outVec[col] += m_rows[row].v[col] * inV[row];
      }
   }

   for ( int i = 0; i < 4; ++i )
   {
      outVec[i] /= outVec[3];
   }
}

///////////////////////////////////////////////////////////////////////////////

void Matrix::transformNorm( const Vector& rhs, Vector& outNormal ) const
{
   float inV[3];
   memcpy( inV, rhs.m_quad.v, sizeof( float ) * 3 );

   for ( int col = 0; col < 3; ++col )
   {
      outNormal[col] = 0.0f;
      for ( int row = 0; row < 3; ++row )
      {
         outNormal[col] += m_rows[row].v[col] * inV[row];
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void Matrix::transform4( const Vector& rhs, Vector& outVec ) const
{
   for ( int col = 0; col < 4; ++col )
   {
      outVec[col] = 0.0f;
      for ( int row = 0; row < 4; ++row )
      {
         outVec[col] += m_rows[row].v[col] * rhs[row];
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void Matrix::getRowMajor( float* __restrict outRowMajorMtx ) const
{
   memcpy( outRowMajorMtx, &m_rows[0], sizeof( QuadStorage ) * 4 );
}

///////////////////////////////////////////////////////////////////////////////

void Matrix::getColumnMajor( float* __restrict outColumnMajorMtx ) const
{
   int idx = 0;
   for ( int col = 0; col < 4; ++col )
   {
      for ( int row = 0; row < 4; ++row )
      {
         outColumnMajorMtx[idx] = m_rows[row].v[col];
         ++idx;
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void Matrix::getRightHanded( float* __restrict outRowMajorMtx ) const
{
   memcpy( &outRowMajorMtx[0], &m_rows[0], sizeof( QuadStorage ) );
   memcpy( &outRowMajorMtx[1], &m_rows[1], sizeof( QuadStorage ) );
   memcpy( &outRowMajorMtx[2], &m_rows[2], sizeof( QuadStorage ) );
   memcpy( &outRowMajorMtx[3], &m_rows[3], sizeof( QuadStorage ) );
}

///////////////////////////////////////////////////////////////////////////////

#endif // _MATRIX_H
