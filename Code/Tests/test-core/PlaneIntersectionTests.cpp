#include "core-TestFramework\TestFramework.h"
#include "core\Triangle.h"
#include "core\Plane.h"


///////////////////////////////////////////////////////////////////////////////

TEST(PlaneClassification, Triangle)
{
   Triangle vol(Vector(-1, 0, 1), Vector(1, 0, 1), Vector(-1, 0, -1));

   // oy
   Plane plane;
   plane.set( Quad_0100, Float_0 );
   CPPUNIT_ASSERT_EQUAL(0.f, vol.distanceToPlane(plane).getFloat() );
   CPPUNIT_ASSERT_EQUAL(PPC_COPLANAR, vol.classifyAgainsPlane(plane) );

   plane.set( Quad_0100, Float_1 );
   CPPUNIT_ASSERT(vol.distanceToPlane(plane) > Float_0);
   CPPUNIT_ASSERT_EQUAL(PPC_FRONT, vol.classifyAgainsPlane(plane) );

   plane.set( Quad_0100, Float_Minus1 );
   CPPUNIT_ASSERT(vol.distanceToPlane(plane) < Float_0);
   CPPUNIT_ASSERT_EQUAL(PPC_BACK, vol.classifyAgainsPlane(plane) );

   // ox
   plane.set( Quad_1000, Float_0 );
   CPPUNIT_ASSERT_EQUAL(0.f, vol.distanceToPlane(plane).getFloat() );
   CPPUNIT_ASSERT_EQUAL(PPC_SPANNING, vol.classifyAgainsPlane(plane) );

   plane.set( Quad_1000, Float_Minus1 );
   CPPUNIT_ASSERT_EQUAL(0.f, vol.distanceToPlane(plane).getFloat() );
   CPPUNIT_ASSERT_EQUAL(PPC_SPANNING, vol.classifyAgainsPlane(plane) );

   plane.set( Quad_1000, Float_1 );
   CPPUNIT_ASSERT_EQUAL(0.f, vol.distanceToPlane(plane).getFloat() );
   CPPUNIT_ASSERT_EQUAL(PPC_SPANNING, vol.classifyAgainsPlane(plane) );

   plane.set( Quad_1000, Float_2 );
   CPPUNIT_ASSERT(vol.distanceToPlane(plane) > Float_0 );
   CPPUNIT_ASSERT_EQUAL(PPC_FRONT, vol.classifyAgainsPlane(plane) );

   plane.set( Quad_1000, Float_Minus2 );
   CPPUNIT_ASSERT(vol.distanceToPlane(plane) < Float_0 );
   CPPUNIT_ASSERT_EQUAL(PPC_BACK, vol.classifyAgainsPlane(plane) );

   // oz
   plane.set( Quad_0010, Float_0 );
   CPPUNIT_ASSERT_EQUAL(0.f, vol.distanceToPlane(plane).getFloat() );
   CPPUNIT_ASSERT_EQUAL(PPC_SPANNING, vol.classifyAgainsPlane(plane) );

   plane.set( Quad_0010, Float_Minus1 );
   CPPUNIT_ASSERT_EQUAL(0.f, vol.distanceToPlane(plane).getFloat() );
   CPPUNIT_ASSERT_EQUAL(PPC_SPANNING, vol.classifyAgainsPlane(plane) );

   plane.set( Quad_0010, Float_1 );
   CPPUNIT_ASSERT_EQUAL(0.f, vol.distanceToPlane(plane).getFloat() );
   CPPUNIT_ASSERT_EQUAL(PPC_SPANNING, vol.classifyAgainsPlane(plane) );

   plane.set( Quad_0010, Float_2 );
   CPPUNIT_ASSERT(vol.distanceToPlane(plane) > Float_0 );
   CPPUNIT_ASSERT_EQUAL(PPC_FRONT, vol.classifyAgainsPlane(plane) );

   plane.set( Quad_0010, Float_Minus2 );
   CPPUNIT_ASSERT(vol.distanceToPlane(plane) < Float_0 );
   CPPUNIT_ASSERT_EQUAL(PPC_BACK, vol.classifyAgainsPlane(plane) );
}

///////////////////////////////////////////////////////////////////////////////
