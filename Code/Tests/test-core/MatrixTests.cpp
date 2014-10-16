#include "core-TestFramework\TestFramework.h"
#include "core\EulerAngles.h"
#include "core\Vector.h"
#include "core\Matrix.h"
#include "core\Quaternion.h"
#include "core\Plane.h"
#include "core\MathDefs.h"
#include <d3dx9.h>


///////////////////////////////////////////////////////////////////////////////

TEST( Matrix, fromEulerAngles )
{
   D3DXMATRIX dxRotMtx;
   Matrix tamyRotMtx;
   EulerAngles testAngle;

   for ( float roll = -179.0f; roll <= 179.0f; roll += 15.0f )
   {
      for ( float yaw = -179.0f; yaw <= 179.0f; yaw += 15.0f )
      {
         for ( float pitch = -179.0f; pitch <= 179.0f; pitch += 15.0f )
         {
            D3DXMatrixRotationYawPitchRoll( &dxRotMtx, DEG2RAD(yaw), DEG2RAD(pitch), DEG2RAD(roll) );

            testAngle.set( FastFloat::fromFloat( yaw ), FastFloat::fromFloat( pitch ), FastFloat::fromFloat( roll ) );
            tamyRotMtx.setRotation( testAngle );
            COMPARE_MTX( dxRotMtx, tamyRotMtx );
         }
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

TEST( Matrix, vectorTransformation )
{
   Matrix m;
   Vector result;

   // translations
   m.setTranslation( Vector( 1, 2, 3 ) );
   m.transform( Vector( 0, 0, 0 ), result );
   CPPUNIT_ASSERT_DOUBLES_EQUAL(1.f, result[0], 1e-3);
   CPPUNIT_ASSERT_DOUBLES_EQUAL(2.f, result[1], 1e-3);
   CPPUNIT_ASSERT_DOUBLES_EQUAL(3.f, result[2], 1e-3);

   m.transform( Vector( 3, 2, 1 ), result );
   CPPUNIT_ASSERT_DOUBLES_EQUAL(4.f, result[0], 1e-3);
   CPPUNIT_ASSERT_DOUBLES_EQUAL(4.f, result[1], 1e-3);
   CPPUNIT_ASSERT_DOUBLES_EQUAL(4.f, result[2], 1e-3);

   // rotations
   EulerAngles angles;
   angles.set( FastFloat::fromFloat( 90.0f ), Float_0, Float_0 );
   m.setRotation( angles );
   m.transform( Vector( 0, 0, 1 ), result );
   CPPUNIT_ASSERT_DOUBLES_EQUAL(1.f, result[0], 1e-3);
   CPPUNIT_ASSERT_DOUBLES_EQUAL(0.f, result[1], 1e-3);
   CPPUNIT_ASSERT_DOUBLES_EQUAL(0.f, result[2], 1e-3);
}

///////////////////////////////////////////////////////////////////////////////

TEST( Matrix, matrixConcatenation )
{
   Matrix m1, m2;
   EulerAngles angles;
   angles.set( FastFloat::fromFloat( 90.0f ), Float_0, Float_0 );
   m1.setRotation( angles );
   m2.setTranslation( Vector( 1, 2, 3 ) );

   // compare the matrices - one concatenated using DX methods,
   // and the other using our method
   Matrix tamyResult;
   tamyResult.setMul( m2, m1 );

   D3DXMATRIX dxResult;
   D3DXMatrixMultiply( &dxResult, ( const D3DXMATRIX* )&m2, ( const D3DXMATRIX* )&m1 );
   COMPARE_MTX( tamyResult, dxResult );
}

///////////////////////////////////////////////////////////////////////////////

TEST( Matrix, preMultiplyingSelf )
{
   Matrix m1, m2;
   EulerAngles angles;
   angles.set( FastFloat::fromFloat( 90.0f ), Float_0, Float_0 );
   m1.setRotation( angles );
   m2.setTranslation( Vector( 1, 2, 3 ) );

   Matrix cleanMulResult;
   cleanMulResult.setMul( m2, m1 );

   m1.preMul( m2 );

   COMPARE_MTX( cleanMulResult, m1 );
}

///////////////////////////////////////////////////////////////////////////////

TEST( Matrix, transformingSimpleCoordinate )
{
   Matrix m1, m2;
   EulerAngles angles;
   angles.set( FastFloat::fromFloat( 90.0f ), Float_0, Float_0 );
   m1.setRotation( angles );
   m2.setTranslation( Vector( 1, 2, 3 ) );

   Matrix transformMtx;
   transformMtx.setMul( m2, m1 );

   // this transform will first translate a vector by ( 1, 2, 3 ) and then rotate it clockwise by 90 degrees

   D3DXVECTOR3 dxVecTransformed;
   D3DXVec3TransformCoord( &dxVecTransformed, ( const D3DXVECTOR3* )&Quad_0010, ( const D3DXMATRIX* )&transformMtx );

   Vector tamyVecTransformed;
   transformMtx.transform( Vector( Quad_0010 ), tamyVecTransformed );

   COMPARE_VEC( dxVecTransformed, tamyVecTransformed );

   CPPUNIT_ASSERT_DOUBLES_EQUAL( 4.f, tamyVecTransformed[0], 1e-3);
   CPPUNIT_ASSERT_DOUBLES_EQUAL(2.f, tamyVecTransformed[1], 1e-3);
   CPPUNIT_ASSERT_DOUBLES_EQUAL(-1.f, tamyVecTransformed[2], 1e-3);
}

///////////////////////////////////////////////////////////////////////////////

TEST( Matrix, transformingSimpleNormal )
{
   Matrix mtx;

   // rotation
   {
      EulerAngles angles;
      angles.set( FastFloat::fromFloat( 90.0f ), Float_0, Float_0 );
      mtx.setRotation( angles );

      Vector tamyNormalTransformed;
      mtx.transformNorm( Quad_1000, tamyNormalTransformed );

      D3DXVECTOR3 dxNormalTransformed;
      D3DXVec3TransformNormal( &dxNormalTransformed, ( const D3DXVECTOR3* )&Quad_1000, ( const D3DXMATRIX* )&mtx );

      COMPARE_VEC( dxNormalTransformed, tamyNormalTransformed );

      CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.f, tamyNormalTransformed[0], 1e-3 );
      CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.f, tamyNormalTransformed[1], 1e-3 );
      CPPUNIT_ASSERT_DOUBLES_EQUAL( -1.f, tamyNormalTransformed[2], 1e-3 );
   }

   // translation
   {
      mtx.setTranslation( Vector( 0, 0, 10 ) );

      Vector tamyNormalTransformed;
      mtx.transformNorm( Quad_1000, tamyNormalTransformed );

      D3DXVECTOR3 dxNormalTransformed;
      D3DXVec3TransformNormal( &dxNormalTransformed, ( const D3DXVECTOR3* )&Quad_1000, ( const D3DXMATRIX* )&mtx );

      COMPARE_VEC( dxNormalTransformed, tamyNormalTransformed );

      CPPUNIT_ASSERT_DOUBLES_EQUAL( 1.f, tamyNormalTransformed[0], 1e-3 );
      CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.f, tamyNormalTransformed[1], 1e-3 );
      CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.f, tamyNormalTransformed[2], 1e-3 );
   }
}


///////////////////////////////////////////////////////////////////////////////

TEST( Matrix, transformingCoordinates )
{
   Matrix mtx;
   Vector result;
   EulerAngles angles;

   angles.set( FastFloat::fromFloat( 90.0f ), Float_0, Float_0 );
   mtx.setRotation( angles);
   mtx.transform( Quad_0010, result );
   CPPUNIT_ASSERT_DOUBLES_EQUAL(1.f, result[0], 1e-3);
   CPPUNIT_ASSERT_DOUBLES_EQUAL(0.f, result[1], 1e-3);
   CPPUNIT_ASSERT_DOUBLES_EQUAL(0.f, result[2], 1e-3);

   angles.set( FastFloat::fromFloat( -90.0f ), Float_0, Float_0 );
   mtx.setRotation( angles ); 
   mtx.transform( Quad_0010, result );
   CPPUNIT_ASSERT_DOUBLES_EQUAL(-1.f, result[0], 1e-3);
   CPPUNIT_ASSERT_DOUBLES_EQUAL(0.f, result[1], 1e-3);
   CPPUNIT_ASSERT_DOUBLES_EQUAL(0.f, result[2], 1e-3);

   angles.set( Float_0, FastFloat::fromFloat( 90.0f ), Float_0 );
   mtx.setRotation( angles ); 
   mtx.transform( Quad_0100, result );
   CPPUNIT_ASSERT_DOUBLES_EQUAL(0.f, result[0], 1e-3);
   CPPUNIT_ASSERT_DOUBLES_EQUAL(0.f, result[1], 1e-3);
   CPPUNIT_ASSERT_DOUBLES_EQUAL(1.f, result[2], 1e-3);

   mtx.setTranslation( Vector(1, 2, 3) );
   mtx.transform( Vector( 10, 20, 30 ), result );
   CPPUNIT_ASSERT_DOUBLES_EQUAL(11.f, result[0], 1e-3);
   CPPUNIT_ASSERT_DOUBLES_EQUAL(22.f, result[1], 1e-3);
   CPPUNIT_ASSERT_DOUBLES_EQUAL(33.f, result[2], 1e-3);
}

///////////////////////////////////////////////////////////////////////////////

TEST( Matrix, fromVectorAngle )
{
   Matrix mtx;
   Vector result;
   EulerAngles angles;

   angles.set( FastFloat::fromFloat( 90.0f ), Float_0, Float_0 );
   mtx.setRotation( angles );
   mtx.transform( Quad_0010, result );
   CPPUNIT_ASSERT_DOUBLES_EQUAL(1.f, result[0], 1e-3);
   CPPUNIT_ASSERT_DOUBLES_EQUAL(0.f, result[1], 1e-3);
   CPPUNIT_ASSERT_DOUBLES_EQUAL(0.f, result[2], 1e-3);

   angles.set( FastFloat::fromFloat( -90.0f ), Float_0, Float_0 );
   mtx.setRotation( angles );
   mtx.transform( Quad_0010, result );
   CPPUNIT_ASSERT_DOUBLES_EQUAL(-1.f, result[0], 1e-3);
   CPPUNIT_ASSERT_DOUBLES_EQUAL(0.f, result[1], 1e-3);
   CPPUNIT_ASSERT_DOUBLES_EQUAL(0.f, result[2], 1e-3);

   angles.set( Float_0, FastFloat::fromFloat( 90.0f ), Float_0 );
   mtx.setRotation( angles );
   mtx.transform( Quad_0100, result );
   CPPUNIT_ASSERT_DOUBLES_EQUAL(0.f, result[0], 1e-3);
   CPPUNIT_ASSERT_DOUBLES_EQUAL(0.f, result[1], 1e-3);
   CPPUNIT_ASSERT_DOUBLES_EQUAL(1.f, result[2], 1e-3);

   mtx.setTranslation( Vector(1, 2, 3) );
   mtx.transform( Vector( 10, 20, 30 ), result );
   CPPUNIT_ASSERT_DOUBLES_EQUAL(11.f, result[0], 1e-3);
   CPPUNIT_ASSERT_DOUBLES_EQUAL(22.f, result[1], 1e-3);
   CPPUNIT_ASSERT_DOUBLES_EQUAL(33.f, result[2], 1e-3);
}

///////////////////////////////////////////////////////////////////////////////

TEST( Matrix, directAccessToVectorComponents )
{
   const ALIGN_16 float mtxCoeffs[16] = { 1,  2,  3,  4,
      5,  6,  7,  8,
      9, 10, 11, 12,
      13, 14, 15, 16 };

   Matrix mtx;
   mtx.set( mtxCoeffs );

   Vector expectedSideVec( 1, 2, 3, 4 );
   Vector expectedUpVec( 5, 6, 7, 8 );
   Vector expectedForwardVec( 9, 10, 11, 12 );
   Vector expectedPosVec( 13, 14, 15, 16 );

   const Vector& sideVec = mtx.sideVec();
   const Vector& upVec = mtx.upVec();
   const Vector& forwardVec = mtx.forwardVec();
   const Vector& posVec = mtx.position();
   
   COMPARE_VEC4( expectedSideVec, sideVec );
   COMPARE_VEC4( expectedUpVec, upVec );
   COMPARE_VEC4( expectedForwardVec, forwardVec );
   COMPARE_VEC4( expectedPosVec, posVec );
}

///////////////////////////////////////////////////////////////////////////////

TEST( Matrix, invertingMatrix )
{
   Matrix m1, m2, mtx;
   EulerAngles angles;
   angles.set( FastFloat::fromFloat( 45.0f ), FastFloat::fromFloat( 60.0f ), FastFloat::fromFloat( -10.0f ) );
   m1.setTranslation( Vector( 1, 2, 3 ) );
   m2.setRotation( angles );
   mtx.setMul( m2, m1 );

   Matrix tamyInvertedMtx;
   tamyInvertedMtx.setInverse( mtx );

   D3DXMATRIX dxInvertedMtx;
   D3DXMatrixInverse( &dxInvertedMtx, NULL, ( const D3DXMATRIX* )&mtx );

   COMPARE_MTX( dxInvertedMtx, tamyInvertedMtx );

   // now check the self-inverted matrix
   mtx.invert();
   COMPARE_MTX( dxInvertedMtx, mtx );
}

///////////////////////////////////////////////////////////////////////////////

TEST( Matrix, transposingMatrix )
{
   Matrix mtx, mtxExpected;
   mtx.set( 1,  2,  3,  4,
            5,  6,  7,  8,
            9, 10, 11, 12,
            13, 14, 15, 16 );

   mtxExpected.set( 1, 5, 9, 13,
            2, 6, 10, 14,
            3, 7, 11, 15,
            4, 8, 12, 16 );

   Matrix tamyTransposedMtx;
   tamyTransposedMtx.setTransposed( mtx );

   D3DXMATRIX dxTransposedMtx = ( const D3DXMATRIX& )mtx;
   const Vector& side = mtx.sideVec();
   D3DXMatrixTranspose( &dxTransposedMtx, ( const D3DXMATRIX* )&mtx );

   COMPARE_MTX( mtxExpected, tamyTransposedMtx );
   COMPARE_MTX( dxTransposedMtx, tamyTransposedMtx );

   // now check the self-inverted matrix
   mtx.transpose();
   COMPARE_MTX( dxTransposedMtx, mtx );
}

///////////////////////////////////////////////////////////////////////////////

TEST( Matrix, planeTransformation )
{
   Plane testPlane;
   testPlane.set( Quad_1000, FastFloat::fromFloat( 10.0f ) );

   Matrix m1, m2, mtx;
   EulerAngles angles;
   angles.set( FastFloat::fromFloat( 45.0f ), FastFloat::fromFloat( 60.0f ), FastFloat::fromFloat( -10.0f ) );
   m1.setTranslation( Vector( 1, 2, 3 ) );
   m2.setRotation( angles );
   mtx.setMul( m2, m1 );

   Plane tamyTransformedPlane;
   mtx.transform( testPlane, tamyTransformedPlane );

   D3DXPLANE dxTransformedPlane;
   D3DXMATRIX dxMtx = ( const D3DXMATRIX& )mtx;
   D3DXMatrixInverse( &dxMtx, NULL, &dxMtx );
   D3DXMatrixTranspose( &dxMtx, &dxMtx );
   D3DXPlaneTransform( &dxTransformedPlane, ( const D3DXPLANE* )&testPlane, &dxMtx );

   COMPARE_PLANE( dxTransformedPlane, tamyTransformedPlane );
}

///////////////////////////////////////////////////////////////////////////////

TEST( Matrix, setFromQuaternion )
{
   Vector arbitraryAxis( 0.3f, 0.5f, -0.2f );
   arbitraryAxis.normalize();

   Vector axis[] = { Vector( Quad_1000 ), Vector( Quad_0100 ), Vector( Quad_0010 ), Vector( Quad_Neg_1000 ), Vector( Quad_Neg_0100 ), Vector( Quad_Neg_0010 ), arbitraryAxis };
   int axisCount = 7;

   Quaternion q;
   Quaternion testQ;
   Matrix tamyMtx;
   D3DXMATRIX dxMtx;

   for ( int axisIdx = 0; axisIdx < axisCount; ++axisIdx )
   {
      for ( float angle = -179.0f; angle <= 179.0f; angle += 1.0f )
      {
         q.setAxisAngle( axis[axisIdx], FastFloat::fromFloat( DEG2RAD( angle ) ) );

         // test if the matrix generated by our method is the same as the one generated by DirectX
         tamyMtx.setRotation( q );
         D3DXMatrixRotationQuaternion( &dxMtx, ( const D3DXQUATERNION* )&q );
         COMPARE_MTX( dxMtx, tamyMtx );

         // and test the backwards operation
         tamyMtx.getRotation( testQ );
         COMPARE_QUAT( q, testQ );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

TEST( Matrix, approximateEquality )
{
   Matrix rhs, lhs;

   // both matrices are exactly alike
   {
      rhs.setRows(   Vector( 1.0f, 2.0f, 3.0f, 4.0f )
                  ,  Vector( 4.0f, 3.0f, 2.0f, 1.0f )
                  ,  Vector( 5.0f, 6.0f, 7.0f, 8.0f )
                  ,  Vector( 8.0f, 7.0f, 6.0f, 5.0f ) );

      lhs.setRows(   Vector( 1.0f, 2.0f, 3.0f, 4.0f )
                  ,  Vector( 4.0f, 3.0f, 2.0f, 1.0f )
                  ,  Vector( 5.0f, 6.0f, 7.0f, 8.0f )
                  ,  Vector( 8.0f, 7.0f, 6.0f, 5.0f ) );

      bool result = isApproximatelyEqual( rhs, lhs );
      CPPUNIT_ASSERT( result );
   }

   // both matrices differ, but the differences are less significant then the assumed epsilon of 1e-3
   {
      rhs.setRows(   Vector( 1.0004f, 2.0003f, 3.0002f, 4.0001f )
                  ,  Vector( 4.0001f, 3.0002f, 2.0003f, 1.0004f )
                  ,  Vector( 5.0004f, 6.0003f, 7.0002f, 8.0001f )
                  ,  Vector( 8.0001f, 7.0002f, 6.0003f, 5.0004f ) );

      lhs.setRows(   Vector( 1.0f, 2.0f, 3.0f, 4.0f )
                  ,  Vector( 4.0f, 3.0f, 2.0f, 1.0f )
                  ,  Vector( 5.0f, 6.0f, 7.0f, 8.0f )
                  ,  Vector( 8.0f, 7.0f, 6.0f, 5.0f ) );

      bool result = isApproximatelyEqual( rhs, lhs );
      CPPUNIT_ASSERT( result );
   }

   // both matrices differ, and the differences exceed the assumed epsilon of 1e-3
   {
      rhs.setRows(   Vector( 1.004f, 2.003f, 3.002f, 4.001f )
                  ,  Vector( 4.001f, 3.002f, 2.003f, 1.004f )
                  ,  Vector( 5.004f, 6.003f, 7.002f, 8.001f )
                  ,  Vector( 8.001f, 7.002f, 6.003f, 5.004f ) );

      lhs.setRows(   Vector( 1.0f, 2.0f, 3.0f, 4.0f )
                  ,  Vector( 4.0f, 3.0f, 2.0f, 1.0f )
                  ,  Vector( 5.0f, 6.0f, 7.0f, 8.0f )
                  ,  Vector( 8.0f, 7.0f, 6.0f, 5.0f ) );

      bool result = isApproximatelyEqual( rhs, lhs );
      CPPUNIT_ASSERT( result == false );
   }

   // both matrices are exactly alike ( special case from a bug )
   {
      rhs.setRows(   Vector( 1.0f, -1.161353e-008f, -6.31241e-008f, 0.0f )
                  ,  Vector( -6.3833e-008f, -0.0770571f, -0.997027f, 0.0f )
                  ,  Vector( 6.73655e-009f, 0.997027f, -0.0770571f, 0.0f )
                  ,  Vector( 1.09999f, -0.23543f, -1.46f, 1.0f ) );

      lhs.setRows(   Vector( 1.0f, -1.161353e-008f, -6.31241e-008f, 0.0f )
                  ,  Vector( -6.3833e-008f, -0.0770571f, -0.997027f, 0.0f )
                  ,  Vector( 6.73655e-009f, 0.997027f, -0.0770571f, 0.0f )
                  ,  Vector( 1.09999f, -0.23543f, -1.46f, 1.0f ) );

      bool result = isApproximatelyEqual( rhs, lhs );
      CPPUNIT_ASSERT( result );
   }
}

///////////////////////////////////////////////////////////////////////////////
