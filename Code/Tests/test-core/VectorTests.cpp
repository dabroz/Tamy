#include "core-TestFramework\TestFramework.h"
#include "core\Vector.h"
#include "core\FastFloat.h"
#include "core\MathDefs.h"
#include <d3dx9.h>


///////////////////////////////////////////////////////////////////////////////

TEST( Vector, crossProduct )
{
   Vector tamyCross;
   D3DXVECTOR3 dxCross;

   tamyCross.setCross( Quad_1000, Quad_0010 );
   D3DXVec3Cross( &dxCross, ( const D3DXVECTOR3* )&Quad_1000, ( const D3DXVECTOR3* )&Quad_0010 );
   COMPARE_VEC( dxCross, tamyCross );

   tamyCross.setCross( Quad_0100, Quad_Neg_1000 );
   D3DXVec3Cross( &dxCross, ( const D3DXVECTOR3* )&Quad_0100, ( const D3DXVECTOR3* )&Quad_Neg_1000 );
   COMPARE_VEC( dxCross, tamyCross );
}

///////////////////////////////////////////////////////////////////////////////

TEST( Vector, lerp )
{
   Vector start; start.set( 1, 2, 3 );
   Vector end; end.set( 2, 4, 6 );
 
   Vector result;

   result.setLerp( start, end, Float_0 );
   COMPARE_VEC( start, result );

   result.setLerp( start, end, Float_1 );
   COMPARE_VEC( end, result );

   result.setLerp( start, end, Float_Inv2 );
   Vector expectedRes; expectedRes.set( 1.5f, 3.f, 4.5f );
   COMPARE_VEC( expectedRes, result );
}


///////////////////////////////////////////////////////////////////////////////

TEST( Vector, floor )
{
   Vector testVal, result;

   testVal.set( 0.7f, 1.2f, 4.1f, 5.5f );
   result.setFloor( testVal );

   Vector expectedResult;
   expectedResult.set( 0.0f, 1.0f, 4.0f, 5.0f );
   COMPARE_VEC( expectedResult, result );
}

///////////////////////////////////////////////////////////////////////////////

TEST( Vector, round )
{
   Vector testVal, result;

   testVal.set( 0.7f, 1.2f, 4.1f, 5.5f );
   result.setRound( testVal );

   Vector expectedResult;
   expectedResult.set( 1.0f, 1.0f, 4.0f, 6.0f );
   COMPARE_VEC( expectedResult, result );
}

///////////////////////////////////////////////////////////////////////////////

TEST( Vector, floatRemainder )
{
   Vector dividend, divisor, result;

   dividend.set( 3.0f, 4.0f, 17.8f, 17.8f );
   divisor.set( 2.0f, 2.0f, 4.0f, 4.1f );
   result.setFloatRemainder( dividend, divisor );

   Vector expectedResult;
   expectedResult.set( -1.0f, 0.0f, 1.8f, 1.4f );
   COMPARE_VEC( expectedResult, result );
}

///////////////////////////////////////////////////////////////////////////////
