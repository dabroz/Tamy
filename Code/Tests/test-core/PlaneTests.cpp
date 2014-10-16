#include "core-TestFramework\TestFramework.h"
#include "core\Plane.h"
#include "core\Vector.h"
#include <d3dx9.h>


///////////////////////////////////////////////////////////////////////////////

TEST( Plane, setFromPointNormal )
{
   Plane tamyPlane;
   D3DXPLANE dxPlane;
   Vector point, normal;
   
   point.set( 10, 20, 30 );
   normal.set( Quad_1000 );
   D3DXPlaneFromPointNormal( &dxPlane, ( const D3DXVECTOR3* )&point, ( const D3DXVECTOR3* )&normal );
   tamyPlane.setFromPointNormal( point, normal );
   COMPARE_PLANE( dxPlane, tamyPlane );

   point.set( -4, 0, 5.5f );
   normal.set( Quad_0100 );
   D3DXPlaneFromPointNormal( &dxPlane, ( const D3DXVECTOR3* )&point, ( const D3DXVECTOR3* )&normal );
   tamyPlane.setFromPointNormal( point, normal );
   COMPARE_PLANE( dxPlane, tamyPlane );

   point.set( 0, 0, -20.0f );
   normal.set( Quad_0010 );
   D3DXPlaneFromPointNormal( &dxPlane, ( const D3DXVECTOR3* )&point, ( const D3DXVECTOR3* )&normal );
   tamyPlane.setFromPointNormal( point, normal );
   COMPARE_PLANE( dxPlane, tamyPlane );

   point.set( 10, 20, 30 );
   normal.set( Quad_Neg_1000 );
   D3DXPlaneFromPointNormal( &dxPlane, ( const D3DXVECTOR3* )&point, ( const D3DXVECTOR3* )&normal );
   tamyPlane.setFromPointNormal( point, normal );
   COMPARE_PLANE( dxPlane, tamyPlane );

   point.set( -4, 0, 5.5f );
   normal.set( Quad_Neg_0100 );
   D3DXPlaneFromPointNormal( &dxPlane, ( const D3DXVECTOR3* )&point, ( const D3DXVECTOR3* )&normal );
   tamyPlane.setFromPointNormal( point, normal );
   COMPARE_PLANE( dxPlane, tamyPlane );

   point.set( 0, 0, -20.0f );
   normal.set( Quad_Neg_0010 );
   D3DXPlaneFromPointNormal( &dxPlane, ( const D3DXVECTOR3* )&point, ( const D3DXVECTOR3* )&normal );
   tamyPlane.setFromPointNormal( point, normal );
   COMPARE_PLANE( dxPlane, tamyPlane );
}

///////////////////////////////////////////////////////////////////////////////

TEST( Plane, setFromPoints )
{
   Plane tamyPlane;
   D3DXPLANE dxPlane;
   Vector ptA, ptB, ptC;

   ptA.set( 0, 0, 0 ); ptB.set( 10, 0, 0 ); ptC.set( 0, 10, 0 );
   D3DXPlaneFromPoints( &dxPlane, ( const D3DXVECTOR3* )&ptA, ( const D3DXVECTOR3* )&ptB, ( const D3DXVECTOR3* )&ptC );
   tamyPlane.setFromPoints( ptA, ptB, ptC );
   COMPARE_PLANE( dxPlane, tamyPlane );

   ptA.set( 0, 0, 0 ); ptB.set( 10, 0, 0 ); ptC.set( 0, 0, 10 );
   D3DXPlaneFromPoints( &dxPlane, ( const D3DXVECTOR3* )&ptA, ( const D3DXVECTOR3* )&ptB, ( const D3DXVECTOR3* )&ptC );
   tamyPlane.setFromPoints( ptA, ptB, ptC );
   COMPARE_PLANE( dxPlane, tamyPlane );

   ptA.set( 0, 0, 0 ); ptB.set( 0, 10, 0 ); ptC.set( 0, 0, 10 );
   D3DXPlaneFromPoints( &dxPlane, ( const D3DXVECTOR3* )&ptA, ( const D3DXVECTOR3* )&ptB, ( const D3DXVECTOR3* )&ptC );
   tamyPlane.setFromPoints( ptA, ptB, ptC );
   COMPARE_PLANE( dxPlane, tamyPlane );

   ptA.set( 0, 0, 0 ); ptB.set( 3, 0, 0 ); ptC.set( 0, -4, 0 );
   D3DXPlaneFromPoints( &dxPlane, ( const D3DXVECTOR3* )&ptA, ( const D3DXVECTOR3* )&ptB, ( const D3DXVECTOR3* )&ptC );
   tamyPlane.setFromPoints( ptA, ptB, ptC );
   COMPARE_PLANE( dxPlane, tamyPlane );

   ptA.set( 0, 0, 0 ); ptB.set( -10, 0, 0 ); ptC.set( 0, 0, -5 );
   D3DXPlaneFromPoints( &dxPlane, ( const D3DXVECTOR3* )&ptA, ( const D3DXVECTOR3* )&ptB, ( const D3DXVECTOR3* )&ptC );
   tamyPlane.setFromPoints( ptA, ptB, ptC );
   COMPARE_PLANE( dxPlane, tamyPlane );

   ptA.set( 0, 0, 0 ); ptB.set( 0, -10, 0 ); ptC.set( 0, 0, -10 );
   D3DXPlaneFromPoints( &dxPlane, ( const D3DXVECTOR3* )&ptA, ( const D3DXVECTOR3* )&ptB, ( const D3DXVECTOR3* )&ptC );
   tamyPlane.setFromPoints( ptA, ptB, ptC );
   COMPARE_PLANE( dxPlane, tamyPlane );

   ptA.set( 20, 3, -4 ); ptB.set( 5, -10, 1 ); ptC.set( 2, -1, -10 );
   D3DXPlaneFromPoints( &dxPlane, ( const D3DXVECTOR3* )&ptA, ( const D3DXVECTOR3* )&ptB, ( const D3DXVECTOR3* )&ptC );
   tamyPlane.setFromPoints( ptA, ptB, ptC );
   COMPARE_PLANE( dxPlane, tamyPlane );
}

///////////////////////////////////////////////////////////////////////////////

TEST( Plane, normalize )
{
   Plane tamyPlane;
   D3DXPLANE dxPlane;

   tamyPlane.set( Float_1, Float_2, FastFloat::fromFloat( -3.0f ), FastFloat::fromFloat( 10.0f ) );
   D3DXPlaneNormalize( &dxPlane, ( const D3DXPLANE* )&tamyPlane );
   tamyPlane.normalize();
   COMPARE_PLANE( dxPlane, tamyPlane );

   tamyPlane.set( Float_Minus1, Float_Minus1, Float_0, Float_Minus1 );
   D3DXPlaneNormalize( &dxPlane, ( const D3DXPLANE* )&tamyPlane );
   tamyPlane.normalize();
   COMPARE_PLANE( dxPlane, tamyPlane );
}

///////////////////////////////////////////////////////////////////////////////

TEST( Plane, dotCoord )
{
   Plane plane;
   Vector testPt;
   
   testPt.set( 3, 2, -1 );
   plane.set( Quad_1000, FastFloat::fromFloat( 10.0f ) );
   float dxDot = D3DXPlaneDotCoord( ( const D3DXPLANE* )&plane, ( const D3DXVECTOR3* )&testPt );
   float tamyDot = plane.dotCoord( testPt ).getFloat();
   CPPUNIT_ASSERT_DOUBLES_EQUAL( dxDot, tamyDot, 1e-3f );

   testPt.set( 3, 2, -1 );
   plane.set( Quad_Neg_0100, FastFloat::fromFloat( -12.0f ) );
   dxDot = D3DXPlaneDotCoord( ( const D3DXPLANE* )&plane, ( const D3DXVECTOR3* )&testPt );
   tamyDot = plane.dotCoord( testPt ).getFloat();
   CPPUNIT_ASSERT_DOUBLES_EQUAL( dxDot, tamyDot, 1e-3f );
}

///////////////////////////////////////////////////////////////////////////////

TEST( Plane, dotNormal )
{
   Plane plane;
   Vector testPt;

   testPt.set( 3, 2, -1 );
   plane.set( Quad_1000, FastFloat::fromFloat( 10.0f ) );
   float dxDot = D3DXPlaneDotNormal( ( const D3DXPLANE* )&plane, ( const D3DXVECTOR3* )&testPt );
   float tamyDot = plane.dotNormal( testPt ).getFloat();
   CPPUNIT_ASSERT_DOUBLES_EQUAL( dxDot, tamyDot, 1e-3f );

   testPt.set( 3, 2, -1 );
   plane.set( Quad_Neg_0100, FastFloat::fromFloat( -12.0f ) );
   dxDot = D3DXPlaneDotNormal( ( const D3DXPLANE* )&plane, ( const D3DXVECTOR3* )&testPt );
   tamyDot = plane.dotNormal( testPt ).getFloat();
   CPPUNIT_ASSERT_DOUBLES_EQUAL( dxDot, tamyDot, 1e-3f );
}

///////////////////////////////////////////////////////////////////////////////
