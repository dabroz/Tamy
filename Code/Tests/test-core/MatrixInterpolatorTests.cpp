#include "core-TestFramework\TestFramework.h"
#include "core\MatrixUtils.h"
#include "core\Vector.h"
#include "core\FastFloat.h"
#include "core\Matrix.h"
#include "core\EulerAngles.h"
#include "core\MathDefs.h"


///////////////////////////////////////////////////////////////////////////////

TEST( MatrixInterpolator, translation )
{
   Matrix start;
   Matrix end;
   Matrix expectedResult;

   start.setTranslation( Vector( 0, 0, 10 ) );
   end.setTranslation( Vector( 0, 0, 20 ) );

   Matrix result;

   expectedResult.setTranslation( Vector( 0, 0, 10 ) );
   MatrixUtils::lerp( start, end, Float_0, result );
   COMPARE_MTX( expectedResult, result );

   expectedResult.setTranslation( Vector( 0, 0, 20 ) );
   MatrixUtils::lerp( start, end, Float_1, result );
   COMPARE_MTX( expectedResult, result );

   expectedResult.setTranslation( Vector( 0, 0, 15 ) );
   MatrixUtils::lerp( start, end, Float_Inv2, result );
   COMPARE_MTX( expectedResult, result );
}

///////////////////////////////////////////////////////////////////////////////

TEST( MatrixInterpolator, rotation )
{
   Matrix start;
   Matrix end;

   start.setIdentity();
   EulerAngles ea;
   ea.set( FastFloat::fromFloat( 90.0f ), Float_0, Float_0 );
   end.setRotation( ea );

   Matrix result;

   MatrixUtils::lerp( start, end, Float_0, result );
   COMPARE_MTX( start, result );

   MatrixUtils::lerp( start, end, Float_1, result );
   COMPARE_MTX( end, result );

   Matrix expectedResult;
   ea.set( FastFloat::fromFloat( 45.0f ), Float_0, Float_0 );
   expectedResult.setRotation( ea );
   MatrixUtils::lerp( start, end, Float_Inv2, result );
   COMPARE_MTX(expectedResult, result );
}

///////////////////////////////////////////////////////////////////////////////
