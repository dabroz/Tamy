#include "core-TestFramework\TestFramework.h"
#include "core\Matrix.h"
#include "core\MatrixUtils.h"
#include "core\MathDefs.h"
#include "core\Vector.h"
#include <d3dx9.h>


///////////////////////////////////////////////////////////////////////////////

TEST( MatrixUtils, generatePrespectiveProjection )
{
   Matrix tamyMtx;
   D3DXMATRIX dxMtx;

   float fov = DEG2RAD( 60.0f );
   float aspectRatio = 1.3333f;
   float nearZ = 1.01f;
   float farZ = 5000.0f;

   D3DXMatrixPerspectiveFovLH( &dxMtx, fov, aspectRatio, nearZ, farZ );
   MatrixUtils::generatePrespectiveProjection( fov, aspectRatio, nearZ, farZ, tamyMtx );
   COMPARE_MTX( dxMtx, tamyMtx );
}

///////////////////////////////////////////////////////////////////////////////

TEST( MatrixUtils, generateOrthogonalProjection )
{
   Matrix tamyMtx;
   D3DXMATRIX dxMtx;

   float width = 2.0f;
   float height = 2.0f;
   float nearZ = 1.01f;
   float farZ = 5000.0f;

   D3DXMatrixOrthoLH( &dxMtx, width, height, nearZ, farZ );
   MatrixUtils::generateOrthogonalProjection( width, height, nearZ, farZ, tamyMtx );
   COMPARE_MTX( dxMtx, tamyMtx );
}

///////////////////////////////////////////////////////////////////////////////

TEST( MatrixUtils, generateLookAtLH )
{
   Matrix tamyLookAtMtx;

   Vector cameraOriginPos( 10, 20, -30 ); 
   Vector lookAtPos( 15, 20, -30 );
   Vector upAxis; upAxis.set( Quad_0100 );

   MatrixUtils::generateLookAtLH( cameraOriginPos, lookAtPos, upAxis, tamyLookAtMtx );

   Vector expectedLookVec;
   expectedLookVec.setSub( lookAtPos, cameraOriginPos );
   expectedLookVec.normalize();

   Vector transformedLookVec;
   tamyLookAtMtx.transformNorm( Vector( 0, 0, 1 ), transformedLookVec );

   COMPARE_VEC( cameraOriginPos, tamyLookAtMtx.position() );
   COMPARE_VEC( expectedLookVec, transformedLookVec );
}

///////////////////////////////////////////////////////////////////////////////

TEST( MatrixUtils, changeReferenceSystem_parentWithOrientation )
{
   Quaternion rotQ;
   rotQ.setAxisAngle( Quad_1000, FastFloat::fromFloat( DEG2RAD( 90.0f ) ) );

   Matrix parentSystem;
   parentSystem.setRotation( rotQ );
   parentSystem.setPosition< 4 >( Vector( 1, 2, 3, 1 ) );

   Matrix childLocal;
   childLocal.setTranslation( Vector( 0, 0, 5 ) );

   // first the basics - find child's global matrix assuming it's owned by the parentSystem
   Matrix childGlobal;
   childGlobal.setMul( childLocal, parentSystem );
   {
      Matrix expectedChildGlobal;
      expectedChildGlobal.setRotation( rotQ );
      expectedChildGlobal.setPosition< 4 >( Vector( 1, -3, 3, 1 ) );

      COMPARE_MTX( expectedChildGlobal, childGlobal );
   }

   // now we want to transform the child using an external global transform - but in the end
   // we want to get a transform that will transform the child in its local space with respect
   // to its parent so that it appears it moved in global space
   Matrix globalTransform;
   globalTransform.setTranslation( Vector( 0, 0, 10 ) );

   // change the reference system of our transformation
   Matrix localTransform;
   MatrixUtils::changeReferenceSystem( globalTransform, parentSystem, localTransform );

   // now let's apply that local transform to our node and see if it
   // moved to the expected position in global space
   {
      Matrix newChildLocal;
      newChildLocal.setMul( localTransform, childLocal );

      Matrix newChildGlobal;
      newChildGlobal.setMul( newChildLocal, parentSystem );

      Matrix expectedChildGlobal;
      expectedChildGlobal.setRotation( rotQ );
      expectedChildGlobal.setPosition< 4 >( Vector( 1, -3, 13, 1 ) );  // we moved it 10 units along global Z axis, which was 3, so now it should be equal to 13. The rest remains the same

      COMPARE_MTX( expectedChildGlobal, newChildGlobal );
   }
}

///////////////////////////////////////////////////////////////////////////////

TEST( MatrixUtils, changeReferenceSystem_childWithOrientation )
{
   Quaternion rotQ;
   rotQ.setAxisAngle( Quad_1000, FastFloat::fromFloat( DEG2RAD( 90.0f ) ) );

   Matrix parentSystem;
   parentSystem.setTranslation( Vector( 1, 2, 3, 1 ) );

   Matrix childLocal;
   childLocal.setRotation( rotQ );
   childLocal.setPosition< 4 >( Vector( 0, 0, 5, 1 ) );

   // first the basics - find child's global matrix assuming it's owned by the parentSystem
   Matrix childGlobal;
   childGlobal.setMul( childLocal, parentSystem );
   {
      Matrix expectedChildGlobal;
      expectedChildGlobal.setRotation( rotQ );
      expectedChildGlobal.setPosition< 4 >( Vector( 1, 2, 8, 1 ) );

      COMPARE_MTX( expectedChildGlobal, childGlobal );
   }

   // now we want to transform the child using an external global transform - but in the end
   // we want to get a transform that will transform the child in its local space with respect
   // to its parent so that it appears it moved in global space
   Matrix globalTransform;
   globalTransform.setTranslation( Vector( 0, 0, 10 ) );

   // change the reference system of our transformation
   Matrix localTransform;
   MatrixUtils::changeReferenceSystem( globalTransform, parentSystem, localTransform );

   // now let's apply that local transform to our node and see if it
   // moved to the expected position in global space
   {
      Matrix newChildLocal;
      newChildLocal.setMul( childLocal, localTransform );

      Matrix newChildGlobal;
      newChildGlobal.setMul( newChildLocal, parentSystem );

      Matrix expectedChildGlobal;
      expectedChildGlobal.setRotation( rotQ );
      expectedChildGlobal.setPosition< 4 >( Vector( 1, 2, 18, 1 ) );  // we moved it 10 units along global Z axis, which was 8, so now it should be equal to 18. The rest remains the same

      COMPARE_MTX( expectedChildGlobal, newChildGlobal );
   }
}

///////////////////////////////////////////////////////////////////////////////
