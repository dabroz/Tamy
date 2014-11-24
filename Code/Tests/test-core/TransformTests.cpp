#include "core-TestFramework\TestFramework.h"
#include "core/Transform.h"
#include "core/Vector.h"
#include "core/Plane.h"
#include "core/Matrix.h"
#include "core/EulerAngles.h"
#include "core/MathDefs.h"
#include <d3dx9.h>


///////////////////////////////////////////////////////////////////////////////

TEST( Transform, matrixConversion )
{
   // construct a transformation matrix we'll use for testing
   Matrix templateTranslationMtx;
   Quaternion testRot;
   Vector testTrans;
   {
      testRot.setAxisAngle( Quad_1000, FastFloat::fromFloat( DEG2RAD( 90.0f ) ) );
      templateTranslationMtx.setRotation( testRot );

      Matrix translationMtx;
      testTrans.set( 10, 20, 30 );
      translationMtx.setTranslation( testTrans );
      templateTranslationMtx.mul( translationMtx );
   }

   // first create a transform from the matrix
   Transform transform;
   transform.set( templateTranslationMtx );

   COMPARE_QUAT( testRot, transform.m_rotation );
   COMPARE_VEC( testTrans, transform.m_translation );

   // and then convert that transform back to a matrix form
   Matrix recoveredMtx;
   transform.toMatrix( recoveredMtx );
   COMPARE_MTX( templateTranslationMtx, recoveredMtx );
}

///////////////////////////////////////////////////////////////////////////////

TEST( Transform, vectorTransformation )
{
   // construct a transformation we'll use for testing
   Transform trans;
   {
      trans.m_rotation.setAxisAngle( Quad_1000, FastFloat::fromFloat( DEG2RAD( 90.0f ) ) );
      trans.m_translation.set( 10, 20, 30 );
   }

   // create the test vector
   Vector testVec( 0, 20, 0 );

   // transform the vector
   Vector transformedVec;
   trans.transform( testVec, transformedVec );

   // the transform should:
   //  1. rotate the vector around the X axis, transforming it to ( 0, 0, 20 )
   //  2. translate it by ( 10, 20, 30 ), effectively transforming it to ( 10, 20, 50 )
   COMPARE_VEC( Vector( 10, 20, 50 ), transformedVec );
}

///////////////////////////////////////////////////////////////////////////////

TEST( Transform, normalTransformation )
{
   // construct a transformation we'll use for testing
   Transform trans;
   {
      trans.m_rotation.setAxisAngle( Quad_1000, FastFloat::fromFloat( DEG2RAD( 90.0f ) ) );
      trans.m_translation.set( 10, 20, 30 );
   }

   // create the test vector
   Vector testVec( 0, 1, 0 );

   // transform the vector
   Vector transformedVec;
   trans.transformNorm( testVec, transformedVec );

   // the transform should:
   //  1. rotate the vector around the X axis, transforming it to ( 0, 0, 1 )
   //  2. doesn't translate it
   COMPARE_VEC( Vector( 0, 0, 1 ), transformedVec );
}

///////////////////////////////////////////////////////////////////////////////

TEST( Transform, planeTransformation )
{
  // construct a transformation we'll use for testing
   Transform trans;
   Matrix transformMtx;
   {
      Matrix m1, m2;
      EulerAngles angles;
      angles.set( FastFloat::fromFloat( 45.0f ), FastFloat::fromFloat( 60.0f ), FastFloat::fromFloat( -10.0f ) );
      m1.setTranslation( Vector( 1, 2, 3 ) );
      m2.setRotation( angles );
      transformMtx.setMul( m2, m1 );

      trans.set( transformMtx );
   }

   // create a test plane
   Plane testPlane;
   testPlane.set( Float_1, Float_0, Float_0, FastFloat::fromFloat( 10.0f ) );

   // transform the plane
   Plane tamyTransformedPlane;
   trans.transform( testPlane, tamyTransformedPlane );

   // let DX help us generate a transformed plane we can compare our results against
   D3DXPLANE dxTransformedPlane;
   D3DXMATRIX dxMtx = ( const D3DXMATRIX& )transformMtx;
   D3DXMatrixInverse( &dxMtx, NULL, &dxMtx );
   D3DXMatrixTranspose( &dxMtx, &dxMtx );
   D3DXPlaneTransform( &dxTransformedPlane, ( const D3DXPLANE* )&testPlane, &dxMtx );

   COMPARE_PLANE( dxTransformedPlane, tamyTransformedPlane );
}

///////////////////////////////////////////////////////////////////////////////

TEST( Transform, multiplication )
{
   // The test is gonna be based on the comparison of this unverified method against
   // some verified method.
   // We'll use matrix multiplication as the verified form, and then we'll turn
   // our concatenated transform into a matrix and compare the two.

   // construct the component transformations
   Transform transA;
   {
      transA.m_rotation.setAxisAngle( Quad_1000, FastFloat::fromFloat( DEG2RAD( 45.0f ) ) );
      transA.m_translation.set( 2, 0, 0 );
   }
   Transform transB;
   {
      transB.m_rotation.setAxisAngle( Quad_0100, FastFloat::fromFloat( DEG2RAD( 45.0f ) ) );
      transB.m_translation.set( 0, 0, 1 );
   }

   // create the component transformation matrices
   Matrix expectedTransformationMtx;
   {
      Matrix mtxA, mtxB;
      transA.toMatrix( mtxA );
      transB.toMatrix( mtxB );
      expectedTransformationMtx.setMul( mtxA, mtxB );
   }


   // test the two param multiplication
   {
      // multiply the transforms using
      Transform concatenatedTransform;
      concatenatedTransform.setMul( transA, transB );

      Matrix transformationMtx;
      concatenatedTransform.toMatrix( transformationMtx );
   
      COMPARE_MTX( expectedTransformationMtx, transformationMtx );
   }

   // test the in-place post-multiplication
   {
      // multiply the transforms using
      Transform concatenatedTransform = transA;
      concatenatedTransform.mul( transB );

      Matrix transformationMtx;
      concatenatedTransform.toMatrix( transformationMtx );

      COMPARE_MTX( expectedTransformationMtx, transformationMtx );
   }

   // test the in-place pre-multiplication
   {
      // multiply the transforms using
      Transform concatenatedTransform = transB;
      concatenatedTransform.preMul( transA );

      Matrix transformationMtx;
      concatenatedTransform.toMatrix( transformationMtx );

      COMPARE_MTX( expectedTransformationMtx, transformationMtx );
   }
}

///////////////////////////////////////////////////////////////////////////////

TEST( Transform, inversion )
{
   // let's create a transform, then create its inverse and multiply the two.
   // they should add up to an identity transformation
   Transform transform;
   {
      transform.m_rotation.setAxisAngle( Quad_1000, FastFloat::fromFloat( DEG2RAD( 45.0f ) ) );
      transform.m_translation.set( 2, 0, 0 );
   }

   Transform invTransform;
   invTransform.setInverse( transform );

   Transform concatenatedTransformA;
   concatenatedTransformA.setMul( transform, invTransform );

   // no matter what order we multiply the component transforms in, it should always result in an identity transform
   Transform concatenatedTransformB;
   concatenatedTransformB.setMul( invTransform , transform );

   COMPARE_QUAT( concatenatedTransformA.m_rotation, concatenatedTransformB.m_rotation );
   COMPARE_QUAT( Quaternion::getIdentity(), concatenatedTransformB.m_rotation );

   COMPARE_VEC( concatenatedTransformA.m_translation, concatenatedTransformB.m_translation );
   COMPARE_VEC( Vector( Quad_0 ), concatenatedTransformB.m_translation );
}

///////////////////////////////////////////////////////////////////////////////

TEST( Transform, advancedInversion )
{
   Transform transform1;
   {
      transform1.m_rotation.setAxisAngle( Quad_1000, FastFloat::fromFloat( DEG2RAD( 70.0f ) ) );
      transform1.m_translation.set( 2, 5, -1 );
   }

   Transform transform2;
   {
      transform2.m_rotation.setAxisAngle( Quad_0100, FastFloat::fromFloat( DEG2RAD( -75.0f ) ) );
      transform2.m_translation.set( -4, 2, 4 );
   }

   Matrix mtx1, mtx2;
   transform1.toMatrix( mtx1 );
   transform2.toMatrix( mtx2 );

   // test 1
   {
      Transform invTransform1;
      invTransform1.setInverse( transform1 );

      Matrix invMtx1;
      invMtx1.setInverse( mtx1 );

      Matrix testMtx;
      invTransform1.toMatrix( testMtx );
      COMPARE_MTX( testMtx, invMtx1 );
   }

   // test 2
   {
      Transform invTransform1;
      invTransform1.setInverse( transform1 );

      Transform subTransform;
      subTransform.setMul( transform2, invTransform1 );

      Matrix invMtx1;
      invMtx1.setInverse( mtx1 );

      Matrix subMtx;
      subMtx.setMul( mtx2, invMtx1 );

      Matrix testMtx;
      subTransform.toMatrix( testMtx );
      COMPARE_MTX( testMtx, subMtx );
   }

   // test 3
   {
      Transform subTransform;
      subTransform.setMulInverse( transform2, transform1 );

      Matrix invMtx1;
      invMtx1.setInverse( mtx1 );

      Matrix subMtx;
      subMtx.setMul( mtx2, invMtx1 );

      Matrix testMtx;
      subTransform.toMatrix( testMtx );
      COMPARE_MTX( testMtx, subMtx );
   }
}

///////////////////////////////////////////////////////////////////////////////
