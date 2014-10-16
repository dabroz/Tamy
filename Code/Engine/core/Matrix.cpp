#include "core\Matrix.h"
#include "core\Vector.h"
#include "core\EulerAngles.h"
#include "core\Quaternion.h"
#include "core\Assert.h"
#include "core\OutStream.h"
#include "core\InStream.h"
#include "core\MathDefs.h"


///////////////////////////////////////////////////////////////////////////////

Matrix Matrix::IDENTITY;

///////////////////////////////////////////////////////////////////////////////

void Matrix::setAxisAnglePos( const Vector& axis, const FastFloat& angle, const Vector& pos )
{
   Quaternion rot;
   rot.setAxisAngle( axis, angle );
   setRotation( rot );

#ifndef _USE_SIMD
   ( Vector& )( m_rows[3] ) = pos;
   m_rows[3].v[3] = 1;
#else
   SimdUtils::setXYZ_W( &pos.m_quad, &Float_1.m_val, &m_rows[3] );
#endif
}

///////////////////////////////////////////////////////////////////////////////

void Matrix::setTranslation( const Vector& translationVec )
{
   setIdentity();

   // can't use the specialized version of setPosition here for some reason ( compiler
   // complains that it's already been specialized, as if I was making an implementation here - WEIRD ),
   // so I'm just inlining the code instead
   m_rows[3] = translationVec.m_quad;

   Matrix& mtx = *this;
   mtx( 3, 3 ) = 1.0f;
}

///////////////////////////////////////////////////////////////////////////////

void Matrix::setRotation( const Quaternion& q )
{
   Vector quatVec;
   quatVec.load( q.m_quad );

   Vector q2; q2.setAdd( quatVec, quatVec );
   Vector xq2; xq2.setMul( q2, quatVec.getComponent(0) );
   Vector yq2; yq2.setMul( q2, quatVec.getComponent(1) );
   Vector zq2; zq2.setMul( q2, quatVec.getComponent(2) );
   Vector wq2; wq2.setMul( q2, quatVec.getComponent(3) );

   Vector c0; c0.set( 1.0f - ( yq2[1] + zq2[2] ), xq2[1] + wq2[2] , xq2[2] - wq2[1], 0.0f );
   Vector c1; c1.set( xq2[1] - wq2[2], 1.0f - ( xq2[0] + zq2[2] ), yq2[2] + wq2[0], 0.0f );
   Vector c2; c2.set( xq2[2] + wq2[1] , yq2[2] - wq2[0], 1.0f - ( xq2[0] + yq2[1] ), 0.0f );

   setRows( c0, c1, c2 );
}

///////////////////////////////////////////////////////////////////////////////

void Matrix::getRotation( Quaternion& q ) const
{
   const Matrix& mtx = *this;
   float trace = mtx( 0, 0 ) + mtx( 1, 1 ) + mtx( 2, 2 );

   if( trace > -1.0f ) 
   {
      float s = sqrt( trace + 1.0f ) * 2.0f;
      q[3] = 0.25f * s;
      q[0] = ( mtx( 1, 2 ) - mtx( 2, 1 ) ) / s;
      q[1] = ( mtx( 2, 0 ) - mtx( 0, 2 ) ) / s;
      q[2] = ( mtx( 0, 1 ) - mtx( 1, 0 ) ) / s;
   } 
   else if ( mtx( 0, 0 ) > mtx( 1, 1 ) && mtx( 0, 0 ) > mtx( 2, 2 ) ) 
   {
      float s = 2.0f * sqrt( 1.0f + mtx( 0, 0 ) - mtx( 1, 1 ) - mtx( 2, 2 ) );
      q[3] = ( mtx( 1, 2 ) - mtx( 2, 1 ) ) / s;
      q[0] = 0.25f * s;
      q[1] = ( mtx( 0, 1 ) + mtx( 1, 0 ) ) / s;
      q[2] = ( mtx( 0, 2 ) + mtx( 2, 0 ) ) / s;
   } 
   else if ( mtx( 1, 1 ) > mtx( 2, 2 ) ) 
   {
      float s = 2.0f * sqrt( 1.0f + mtx( 1, 1 ) - mtx( 0, 0 ) - mtx( 2, 2 ) );
      q[3] = ( mtx( 0, 2 ) - mtx( 2, 0 ) ) / s;
      q[0] = ( mtx( 0, 1 ) + mtx( 1, 0 ) ) / s;
      q[1] = 0.25f * s;
      q[2] = ( mtx( 1, 2 ) + mtx( 2, 1 ) ) / s;
   } 
   else 
   {
      float s = 2.0f * sqrt( 1.0f + mtx( 2, 2 ) - mtx( 0, 0 ) - mtx( 1, 1 ) );
      q[3] = ( mtx( 1, 0 ) - mtx( 0, 1 ) ) / s;
      q[0] = ( mtx( 0, 2 ) + mtx( 2, 0 ) ) / s;
      q[1] = ( mtx( 1, 2 ) + mtx( 2, 1 ) ) / s;
      q[2] = 0.25f * s;
   }
   
   q.normalize();
}

///////////////////////////////////////////////////////////////////////////////

void Matrix::setRotation( const EulerAngles& angles )
{
   // Assuming the angles are in radians.
   double cy = cos( DEG2RAD( angles[ EulerAngles::Yaw ] ) );
   double sy = sin( DEG2RAD( angles[ EulerAngles::Yaw ] ) );
   double cr = cos( DEG2RAD( angles[ EulerAngles::Roll ] ) );
   double sr = sin( DEG2RAD( angles[ EulerAngles::Roll ] ) );
   double cp = cos( DEG2RAD( angles[ EulerAngles::Pitch ] ) );
   double sp = sin( DEG2RAD( angles[ EulerAngles::Pitch ] ) );

   // Mtx(Roll) * Mtx(Pitch) * Mtx(Yaw) - to be in sync with the counterpart method Quaternion::setFromEulerAngles( const EulerAngles& angles )
   Matrix& mtx = *this;
   mtx( 0, 0 ) = (float)( cr*cy + sr*sp*sy );
   mtx( 0, 1 ) = (float)( sr*cp );
   mtx( 0, 2 ) = (float)( sr*sp*cy - sy*cr );
   mtx( 0, 3 ) = 0.0f;

   mtx( 1, 0 ) = (float)( cr*sp*sy - sr*cy );
   mtx( 1, 1 ) = (float)( cr*cp );
   mtx( 1, 2 ) = (float)( sr*sy + cr*sp*cy );
   mtx( 1, 3 ) = 0.0f;

   mtx( 2, 0 ) = (float)( cp*sy );
   mtx( 2, 1 ) = (float)( -sp );
   mtx( 2, 2 ) = (float)( cp*cy );
   mtx( 2, 3 ) = 0.0f; 

   mtx( 3, 0 ) = mtx( 3, 1 ) = mtx( 3, 2 ) = 0.0f;
   mtx( 3, 3 ) = 1.0f;
}

///////////////////////////////////////////////////////////////////////////////

void Matrix::getRotation( EulerAngles& outAngles ) const
{
   const Matrix& mtx = *this;
   if ( mtx( 2, 1 ) < -0.998f ) 
   { 
      // singularity at north pole
      outAngles[ EulerAngles::Yaw ] = -atan2( mtx( 0, 2 ), mtx( 1, 2 ) );
      outAngles[ EulerAngles::Pitch ] = (float)M_PI_2;
      outAngles[ EulerAngles::Roll ] = 0;
   }
   else if ( mtx( 2, 1 ) > 0.998f ) 
   { 
      // singularity at south pole
      outAngles[ EulerAngles::Yaw ] = -atan2( mtx( 0, 2 ), -mtx( 1, 2 ) );
      outAngles[ EulerAngles::Pitch ] = (float)-M_PI_2;
      outAngles[ EulerAngles::Roll ] = 0;
   }
   else
   {
      outAngles[ EulerAngles::Yaw ] = atan2( mtx( 2, 0 ), mtx(2, 2 ) );
      outAngles[ EulerAngles::Roll ] = atan2( mtx( 0, 1 ), mtx(1, 1 ) );
      outAngles[ EulerAngles::Pitch ] = asin( -mtx( 2, 1 ) );
   }

   outAngles[ EulerAngles::Yaw ] = RAD2DEG( outAngles[ EulerAngles::Yaw ] );
   outAngles[ EulerAngles::Pitch ] = RAD2DEG( outAngles[ EulerAngles::Pitch ] );
   outAngles[ EulerAngles::Roll ] = RAD2DEG( outAngles[ EulerAngles::Roll ] );
}

///////////////////////////////////////////////////////////////////////////////

void Matrix::mul( const Matrix& a )
{
   Matrix tmpMtx = *this;
   setMul( tmpMtx, a );
}

///////////////////////////////////////////////////////////////////////////////

void Matrix::preMul( const Matrix& a )
{
   Matrix tmpMtx = *this;
   setMul( a, tmpMtx );
}

///////////////////////////////////////////////////////////////////////////////

void Matrix::invert()
{
   Matrix tmpMtx;
   tmpMtx.setInverse( *this );

   memcpy( &m_rows, &tmpMtx.m_rows, sizeof( QuadStorage ) * 4 );
}

///////////////////////////////////////////////////////////////////////////////

void Matrix::transpose()
{
   Matrix tmpMtx;
   tmpMtx.setTransposed( *this );

   memcpy( &m_rows, &tmpMtx.m_rows, sizeof( QuadStorage ) * 4 );
}

///////////////////////////////////////////////////////////////////////////////

void Matrix::setInverseTranspose( const Matrix& rhs )
{
   Matrix tmpMtx;
   tmpMtx.setInverse( rhs );
   tmpMtx.transpose();

   memcpy( &m_rows, &tmpMtx.m_rows, sizeof( QuadStorage ) * 4 );
}

///////////////////////////////////////////////////////////////////////////////

void Matrix::transform( const Plane& inPlane, Plane& outPlane ) const
{
   Matrix tmpInvMtx;
   tmpInvMtx.setInverseTranspose( *this );

   tmpInvMtx.transform4( ( const Vector& )inPlane, ( Vector& )outPlane );
}

///////////////////////////////////////////////////////////////////////////////

std::ostream& operator<<( std::ostream& stream, const Matrix& rhs )
{
   stream << "[ ";
   for (int row = 0; row < 4; ++row)
   {
      stream << "[";
      for (int col = 0; col < 4; ++col )
      {
         stream << rhs( row, col );
         if ( col < 3 )
         {
            stream << ", ";
         }
      }
      stream << "] ";
   }
   stream << "]";

   return stream;
}

///////////////////////////////////////////////////////////////////////////////

OutStream& operator<<( OutStream& serializer, const Matrix& rhs )
{
   for (int row = 0; row < 4; ++row)
   {
      for (int col = 0; col < 4; ++col )
      {
         serializer << rhs( row, col );
      }
   }

   return serializer;
}

///////////////////////////////////////////////////////////////////////////////

InStream& operator>>( InStream& serializer, Matrix& rhs )
{
   for (int row = 0; row < 4; ++row)
   {
      for (int col = 0; col < 4; ++col )
      {
         serializer >> rhs( row, col );
      }
   }

   return serializer;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#ifndef _USE_SIMD

// Code that runs on FPU

///////////////////////////////////////////////////////////////////////////////

template<>
void Matrix::setSideVec<3>( const Vector& vec )
{
   ( Vector& )( m_rows[0] ) = vec;
   m_rows[0].v[3] = 0;
}

///////////////////////////////////////////////////////////////////////////////

template<>
void Matrix::setUpVec<3>( const Vector& vec )
{
   ( Vector& )( m_rows[1] ) = vec;
   m_rows[1].v[3] = 0;
}

///////////////////////////////////////////////////////////////////////////////

template<>
void Matrix::setForwardVec<3>( const Vector& vec )
{
   ( Vector& )( m_rows[2] ) = vec;
   m_rows[2].v[3] = 0;
}

///////////////////////////////////////////////////////////////////////////////

template<>
void Matrix::setPosition<3>( const Vector& pos )
{
   ( Vector& )( m_rows[3] ) = pos;
   m_rows[3].v[3] = 1;
}

///////////////////////////////////////////////////////////////////////////////

template<>
void Matrix::setSideVec<4>( const Vector& vec )
{
   ( Vector& )( m_rows[0] ) = vec;
}

///////////////////////////////////////////////////////////////////////////////

template<>
void Matrix::setUpVec<4>( const Vector& vec )
{
   ( Vector& )( m_rows[1] ) = vec;
}

///////////////////////////////////////////////////////////////////////////////

template<>
void Matrix::setForwardVec<4>( const Vector& vec )
{
   ( Vector& )( m_rows[2] ) = vec;
}

///////////////////////////////////////////////////////////////////////////////

template<>
void Matrix::setPosition<4>( const Vector& pos )
{
   ( Vector& )( m_rows[3] ) = pos;
}

///////////////////////////////////////////////////////////////////////////////

// End of the code that runs on FPU

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#else // _USE_SIMD

// Code that runs on SIMD

///////////////////////////////////////////////////////////////////////////////

template<>
void Matrix::setSideVec<3>( const Vector& vec )
{
   SimdUtils::setXYZ_W( &vec.m_quad, &Float_0.m_val, &m_rows[0] );
}

///////////////////////////////////////////////////////////////////////////////

template<>
void Matrix::setUpVec<3>( const Vector& vec )
{
   SimdUtils::setXYZ_W( &vec.m_quad, &Float_0.m_val, &m_rows[1] );
}

///////////////////////////////////////////////////////////////////////////////

template<>
void Matrix::setForwardVec<3>( const Vector& vec )
{
   SimdUtils::setXYZ_W( &vec.m_quad, &Float_0.m_val, &m_rows[2] );
}

///////////////////////////////////////////////////////////////////////////////

template<>
void Matrix::setPosition<3>( const Vector& pos )
{
   SimdUtils::setXYZ_W( &pos.m_quad, &Float_1.m_val, &m_rows[3] );
}

///////////////////////////////////////////////////////////////////////////////

template<>
void Matrix::setSideVec<4>( const Vector& vec )
{
   m_rows[0] = vec.m_quad;
}

///////////////////////////////////////////////////////////////////////////////

template<>
void Matrix::setUpVec<4>( const Vector& vec )
{
   m_rows[1] = vec.m_quad;
}

///////////////////////////////////////////////////////////////////////////////

template<>
void Matrix::setForwardVec<4>( const Vector& vec )
{
   m_rows[2] = vec.m_quad;
}

///////////////////////////////////////////////////////////////////////////////

template<>
void Matrix::setPosition<4>( const Vector& pos )
{
   m_rows[3] = pos.m_quad;
}

///////////////////////////////////////////////////////////////////////////////

// End of the code that runs on SIMD

#endif // _USE_SIMD

///////////////////////////////////////////////////////////////////////////////
