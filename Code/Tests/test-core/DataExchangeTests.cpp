#include "core-TestFramework\TestFramework.h"
#include "core\Vector.h"
#include "core\Color.h"
#include "core\Matrix.h"
#include <d3dx9.h>


///////////////////////////////////////////////////////////////////////////////

TEST( Vector3, exchangingDataWithDX )
{
   Vector dxVec( 1, 2, 3 );
   const Vector& castDxVec = ( const Vector& )(dxVec);
   CPPUNIT_ASSERT_EQUAL( 1.0f, castDxVec[0] );
   CPPUNIT_ASSERT_EQUAL( 2.0f, castDxVec[1] );
   CPPUNIT_ASSERT_EQUAL( 3.0f, castDxVec[2] );

   // now go the other way around
   Vector tamyVec( 5, 6, 7 );
   const Vector& castTamyVec = ( const Vector& )(tamyVec);
   CPPUNIT_ASSERT_EQUAL( 5.0f, castTamyVec[0] );
   CPPUNIT_ASSERT_EQUAL( 6.0f, castTamyVec[1] );
   CPPUNIT_ASSERT_EQUAL( 7.0f, castTamyVec[2] );
}

///////////////////////////////////////////////////////////////////////////////

TEST( Vector4, exchangingDataWithDX )
{
   D3DXVECTOR4 dxVec( 1, 2, 3, 4 );
   const Vector& castDxVec = (const Vector& )(dxVec);
   CPPUNIT_ASSERT_EQUAL( 1.0f, castDxVec[0] );
   CPPUNIT_ASSERT_EQUAL( 2.0f, castDxVec[1] );
   CPPUNIT_ASSERT_EQUAL( 3.0f, castDxVec[2] );
   CPPUNIT_ASSERT_EQUAL( 4.0f, castDxVec[3] );

   // now go the other way around
   Vector tamyVec( 5, 6, 7, 8 );
   const D3DXVECTOR4& castTamyVec = ( const D3DXVECTOR4& )(tamyVec);
   CPPUNIT_ASSERT_EQUAL( 5.0f, castTamyVec[0] );
   CPPUNIT_ASSERT_EQUAL( 6.0f, castTamyVec[1] );
   CPPUNIT_ASSERT_EQUAL( 7.0f, castTamyVec[2] );
   CPPUNIT_ASSERT_EQUAL( 8.0f, castTamyVec[3] );
}

///////////////////////////////////////////////////////////////////////////////

TEST( Matrix, exchangingDataWithDX )
{
   Matrix dxMtx; dxMtx.setIdentity();
   const Matrix& castDxMtx = (const Matrix&)(dxMtx);
   for ( int row = 0; row < 4; ++row )
   {
      for ( int col = 0; col < 4; ++col )
      {
         CPPUNIT_ASSERT_EQUAL( dxMtx( row, col ), castDxMtx( row, col ) );
      }
   }

   // now go the other way around
   Matrix tamyMtx; tamyMtx.setIdentity();
   const Matrix& castTamyMtx = (const Matrix&)(tamyMtx);
   for ( int row = 0; row < 4; ++row )
   {
      for ( int col = 0; col < 4; ++col )
      {
         CPPUNIT_ASSERT_EQUAL( tamyMtx( row, col ), castTamyMtx( row, col ) );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

TEST( Color, exchangingDataWithDX )
{
   D3DXVECTOR4 dxColor( 0.1f, 0.2f, 0.3f, 0.4f );
   const Color& castDxColor = (const Color& )(dxColor);
   CPPUNIT_ASSERT_EQUAL( 0.1f, castDxColor.r );
   CPPUNIT_ASSERT_EQUAL( 0.2f, castDxColor.g );
   CPPUNIT_ASSERT_EQUAL( 0.3f, castDxColor.b );
   CPPUNIT_ASSERT_EQUAL( 0.4f, castDxColor.a );

   // now go the other way around
   Color tamyColor( 0.5f, 0.6f, 0.7f, 0.8f );
   const D3DXVECTOR4& castTamyVec = ( const D3DXVECTOR4& )(tamyColor);
   CPPUNIT_ASSERT_EQUAL( 0.5f, castTamyVec.x );
   CPPUNIT_ASSERT_EQUAL( 0.6f, castTamyVec.y );
   CPPUNIT_ASSERT_EQUAL( 0.7f, castTamyVec.z );
   CPPUNIT_ASSERT_EQUAL( 0.8f, castTamyVec.w );
}

///////////////////////////////////////////////////////////////////////////////
