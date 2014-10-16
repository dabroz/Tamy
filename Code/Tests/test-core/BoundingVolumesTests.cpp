#include "core-TestFramework\TestFramework.h"
#include "core\AxisAlignedBox.h"
#include "core\RaycastResult.h"
#include "core\IntersectionResult.h"
#include "core\Ray.h"
#include "core\Sphere.h"
#include "core\Frustum.h"
#include "core\Triangle.h"
#include "core-TestFramework\MatrixWriter.h"


///////////////////////////////////////////////////////////////////////////////

TEST(AxisAlignedBox, intersects_AABoundingBox)
{
   AxisAlignedBox centralBB(Vector(-1, -1, -1), Vector(1, 1, 1));

   CPPUNIT_ASSERT_EQUAL(false, centralBB.testIntersection( AxisAlignedBox(Vector(-2, -2, -2), Vector(-1.5, -1.5, -1.5))));
   CPPUNIT_ASSERT_EQUAL(false, centralBB.testIntersection( AxisAlignedBox(Vector(1.5, 1.5, 1.5), Vector(2, 2, 2))));

   CPPUNIT_ASSERT_EQUAL(false, centralBB.testIntersection( AxisAlignedBox(Vector(-2, -1, -1), Vector(-1.5, 1, 1))));
   CPPUNIT_ASSERT_EQUAL(false, centralBB.testIntersection( AxisAlignedBox(Vector(-1, -2, -1), Vector(1, -1.5, 1))));
   CPPUNIT_ASSERT_EQUAL(false, centralBB.testIntersection( AxisAlignedBox(Vector(-1, -1, -2), Vector(1, 1, -1.5))));
   CPPUNIT_ASSERT_EQUAL(false, centralBB.testIntersection( AxisAlignedBox(Vector(1.5, -1, -1), Vector(2, 1, 1))));
   CPPUNIT_ASSERT_EQUAL(false, centralBB.testIntersection( AxisAlignedBox(Vector(-1, 1.5, -1), Vector(1, 2, 1))));
   CPPUNIT_ASSERT_EQUAL(false, centralBB.testIntersection( AxisAlignedBox(Vector(-1, -1, 1.5), Vector(1, 1, 2))));

   CPPUNIT_ASSERT_EQUAL(true, centralBB.testIntersection( AxisAlignedBox(Vector(-1, -1, -1), Vector(1, 1, 1))));
   CPPUNIT_ASSERT_EQUAL(true, centralBB.testIntersection( AxisAlignedBox(Vector(-1, -1, -1), Vector(0, 1, 1))));
   CPPUNIT_ASSERT_EQUAL(true, centralBB.testIntersection( AxisAlignedBox(Vector(-1, -1, -1), Vector(1, 0, 1))));
   CPPUNIT_ASSERT_EQUAL(true, centralBB.testIntersection( AxisAlignedBox(Vector(-1, -1, -1), Vector(1, 1, 0))));
   CPPUNIT_ASSERT_EQUAL(true, centralBB.testIntersection( AxisAlignedBox(Vector(0, -1, -1), Vector(1, 1, 1))));
   CPPUNIT_ASSERT_EQUAL(true, centralBB.testIntersection( AxisAlignedBox(Vector(-1, 0, -1), Vector(1, 1, 1))));
   CPPUNIT_ASSERT_EQUAL(true, centralBB.testIntersection( AxisAlignedBox(Vector(-1, -1, 0), Vector(1, 1, 1))));
}

///////////////////////////////////////////////////////////////////////////////

TEST(AxisAlignedBox, intersects_Ray)
{
   AxisAlignedBox centralBB(Vector(-1, -1, -1), Vector(1, 1, 1));
   Ray ray;
   RaycastResult result;
   
   ray.setFromPointDirection( Vector(-2, 0, 0), Vector(0, 0, 1) );
   CPPUNIT_ASSERT_EQUAL(false, ray.rayCast( centralBB, result ) );

   ray.setFromPointDirection( Vector(-2, 0, 0), Vector(0, 0, -1) );
   CPPUNIT_ASSERT_EQUAL(false, ray.rayCast( centralBB, result ) );


   ray.setFromPointDirection( Vector(0, 2, 0), Vector(0, -1, 0) );
   CPPUNIT_ASSERT_EQUAL(true, ray.rayCast( centralBB, result ) );

   ray.setFromPointDirection( Vector(0, 2, 0), Vector(0, 1, 0) );
   CPPUNIT_ASSERT_EQUAL(false, ray.rayCast( centralBB, result ) );


   ray.setFromPointDirection( Vector(0, -2, 0), Vector(0, 1, 0) );
   CPPUNIT_ASSERT_EQUAL(true, ray.rayCast( centralBB, result ) );

   ray.setFromPointDirection( Vector(0, -2, 0), Vector(0, -1, 0) );
   CPPUNIT_ASSERT_EQUAL(false, ray.rayCast( centralBB, result ) );


   ray.setFromPointDirection( Vector(-2, 0, 0), Vector(1, 0, 0) );
   CPPUNIT_ASSERT_EQUAL(true, ray.rayCast( centralBB, result ) );

   ray.setFromPointDirection( Vector(-2, 0, 0), Vector(-1, 0, 0) );
   CPPUNIT_ASSERT_EQUAL(false, ray.rayCast( centralBB, result ) );


   ray.setFromPointDirection( Vector(0, 0, -2), Vector(0, 0, 1) );
   CPPUNIT_ASSERT_EQUAL(true, ray.rayCast( centralBB, result ) );

   ray.setFromPointDirection( Vector(0, 0, -2), Vector(0, 0, -1) );
   CPPUNIT_ASSERT_EQUAL(false, ray.rayCast( centralBB, result ) );


   ray.setFromPointDirection( Vector(2, 2, 2), Vector(-1, -1, -1) );
   CPPUNIT_ASSERT_EQUAL(true, ray.rayCast( centralBB, result ) );

   ray.setFromPointDirection( Vector(2, 2, 2), Vector(1, 1, 1) );
   CPPUNIT_ASSERT_EQUAL(false, ray.rayCast( centralBB, result ) );


   // ray originating inside the box
   ray.setFromPointDirection( Vector(0, 0, 0), Vector(0, 0, 1) );
   CPPUNIT_ASSERT_EQUAL(true, ray.rayCast( centralBB, result ) );
}

///////////////////////////////////////////////////////////////////////////////

TEST(AxisAlignedBox, intersects_Point)
{
   AxisAlignedBox centralBB(Vector(-1, -1, -1), Vector(1, 1, 1));

   IntersectionResult contactPoint;
   CPPUNIT_ASSERT_EQUAL( false, centralBB.isInside( Vector( -2.0f, 0.0f, 0.0f ), contactPoint ) );
   CPPUNIT_ASSERT_EQUAL( false, centralBB.isInside( Vector( 2.0f, 0.0f, 0.0f ), contactPoint ) );
   CPPUNIT_ASSERT_EQUAL( false, centralBB.isInside( Vector( 0.0f, -2.0f, 0.0f ), contactPoint ) );
   CPPUNIT_ASSERT_EQUAL( false, centralBB.isInside( Vector( 0.0f, 2.0f, 0.0f ), contactPoint ) );
   CPPUNIT_ASSERT_EQUAL( false, centralBB.isInside( Vector( 0.0f, 0.0f, -2.0f ), contactPoint ) );
   CPPUNIT_ASSERT_EQUAL( false, centralBB.isInside( Vector( 0.0f, 0.0f, 2.0f ), contactPoint ) );

   CPPUNIT_ASSERT_EQUAL( true, centralBB.isInside( Vector( 0.0f, 0.0f, 0.0f ), contactPoint ) );
   CPPUNIT_ASSERT_DOUBLES_EQUAL( 1.0f, contactPoint.m_penetrationDepth.getFloat(), 1e-3 );

   CPPUNIT_ASSERT_EQUAL( true, centralBB.isInside( Vector( 1.0f, 0.0f, 0.5f ), contactPoint ) );
   CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.0f, contactPoint.m_penetrationDepth.getFloat(), 1e-3 );
   COMPARE_VEC( Vector_OX, contactPoint.m_contactNormal );
   COMPARE_VEC( Vector( 1.0f, 0.0f, 0.0f ), contactPoint.m_contactNormal );

   CPPUNIT_ASSERT_EQUAL( true, centralBB.isInside( Vector( 0.0f, -0.8f, 0.5f ), contactPoint ) );
   CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.2f, contactPoint.m_penetrationDepth.getFloat(), 1e-3 );
   COMPARE_VEC( Vector_NEG_OY, contactPoint.m_contactNormal );
   COMPARE_VEC( Vector( 0.0f, -1.0f, 0.0f ), contactPoint.m_contactNormal );
}

///////////////////////////////////////////////////////////////////////////////

TEST(AxisAlignedBox, intersects_BoundingSphere)
{
   AxisAlignedBox centralBB(Vector(-1, -1, -1), Vector(1, 1, 1));

   Sphere sphere;
   IntersectionResult result;

   sphere.set( Vector(0, 0, 0), 1 );
   CPPUNIT_ASSERT_EQUAL( true, sphere.testIntersection( centralBB, result ) );
   COMPARE_VEC( Vector_OX, result.m_contactNormal );
   COMPARE_VEC( Vector( 0.0f, 0.0f, 0.0f ), result.m_contactPoint );
   CPPUNIT_ASSERT_DOUBLES_EQUAL( 1.0f, result.m_penetrationDepth.getFloat(), 1e-3 );


   sphere.set( Vector(1.5f, 0, 0), 1 );
   CPPUNIT_ASSERT_EQUAL( true, sphere.testIntersection( centralBB, result ) );
   COMPARE_VEC( Vector_OX, result.m_contactNormal );
   COMPARE_VEC( Vector( 1.0f, 0.0f, 0.0f ), result.m_contactPoint );
   CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.5f, result.m_penetrationDepth.getFloat(), 1e-3 );

   sphere.set( Vector(0, 1.5f, 0), 1 );
   CPPUNIT_ASSERT_EQUAL( true, sphere.testIntersection( centralBB, result ) );
   COMPARE_VEC( Vector_OY, result.m_contactNormal );
   COMPARE_VEC( Vector( 0.0f, 1.0f, 0.0f ), result.m_contactPoint );
   CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.5f, result.m_penetrationDepth.getFloat(), 1e-3 );

   sphere.set( Vector(0, 0, 1.5f), 1 );
   CPPUNIT_ASSERT_EQUAL( true, sphere.testIntersection( centralBB, result ) );
   COMPARE_VEC( Vector_OZ, result.m_contactNormal );
   COMPARE_VEC( Vector( 0.0f, 0.0f, 1.0f ), result.m_contactPoint );
   CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.5f, result.m_penetrationDepth.getFloat(), 1e-3 );

   sphere.set( Vector(-1.5f, 0, 0), 1 );
   CPPUNIT_ASSERT_EQUAL( true, sphere.testIntersection( centralBB, result ) );
   COMPARE_VEC( Vector_NEG_OX, result.m_contactNormal );
   COMPARE_VEC( Vector( -1.0f, 0.0f, 0.0f ), result.m_contactPoint );
   CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.5f, result.m_penetrationDepth.getFloat(), 1e-3 );

   sphere.set( Vector(0, -1.5f, 0), 1 );
   CPPUNIT_ASSERT_EQUAL( true, sphere.testIntersection( centralBB, result ) );
   COMPARE_VEC( Vector_NEG_OY, result.m_contactNormal );
   COMPARE_VEC( Vector( 0.0f, -1.0f, 0.0f ), result.m_contactPoint );
   CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.5f, result.m_penetrationDepth.getFloat(), 1e-3 );

   sphere.set( Vector(0, 0, -1.5f), 1 );
   CPPUNIT_ASSERT_EQUAL( true, sphere.testIntersection( centralBB, result ) );
   COMPARE_VEC( Vector_NEG_OZ, result.m_contactNormal );
   COMPARE_VEC( Vector( 0.0f, 0.0f, -1.0f ), result.m_contactPoint );
   CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.5f, result.m_penetrationDepth.getFloat(), 1e-3 );


   sphere.set( Vector(2.5f, 0, 0), 1 );
   CPPUNIT_ASSERT_EQUAL( false, sphere.testIntersection( centralBB, result ) );

   sphere.set( Vector(0, 2.5f, 0), 1 );
   CPPUNIT_ASSERT_EQUAL( false, sphere.testIntersection( centralBB, result ) );

   sphere.set( Vector(0, 0, 2.5f), 1 );
   CPPUNIT_ASSERT_EQUAL( false, sphere.testIntersection( centralBB, result ) );

   sphere.set( Vector(-2.5f, 0, 0), 1 );
   CPPUNIT_ASSERT_EQUAL( false, sphere.testIntersection( centralBB, result ) );

   sphere.set( Vector(0, -2.5f, 0), 1 );
   CPPUNIT_ASSERT_EQUAL( false, sphere.testIntersection( centralBB, result ) );

   sphere.set( Vector(0, 0, 2.5f), 1 );
   CPPUNIT_ASSERT_EQUAL( false, sphere.testIntersection( centralBB, result ) );
}

///////////////////////////////////////////////////////////////////////////////

TEST(Sphere, intersects_Frustrum)
{
   Frustum frustum;
   const FastFloat ff_707 = FastFloat::fromFloat( 0.707107f );
   const FastFloat ff_neg_707 = FastFloat::fromFloat( -0.707107f );

   frustum.planes[0].set( Float_0,    Float_0,    Float_1,       FastFloat::fromFloat( -1.01f ) );
   frustum.planes[1].set( Float_0,    Float_0,    Float_Minus1,  FastFloat::fromFloat( 5002.28f ) );
   frustum.planes[2].set( ff_707,     Float_0,    ff_707,        Float_0 );
   frustum.planes[3].set( ff_neg_707, Float_0,    ff_707,        Float_0 );
   frustum.planes[4].set( Float_0,    ff_neg_707, ff_707,        Float_0 );
   frustum.planes[5].set( Float_0,    ff_707,     ff_707,        Float_0 );

   // completely inside
   CPPUNIT_ASSERT_EQUAL(true, frustum.isInside( Sphere(Vector(0, 0, 100), 1)));

   // approaching left plane
   CPPUNIT_ASSERT_EQUAL(true, frustum.isInside( Sphere(Vector(-9, 0, 10), 1)));
   CPPUNIT_ASSERT_EQUAL(true, frustum.isInside( Sphere(Vector(-10, 0, 10), 1)));
   CPPUNIT_ASSERT_EQUAL(true, frustum.isInside( Sphere(Vector(-11, 0, 10), 1)));
   CPPUNIT_ASSERT_EQUAL(false, frustum.isInside( Sphere(Vector(-12, 0, 10), 1)));

   // approaching right plane
   CPPUNIT_ASSERT_EQUAL(true, frustum.isInside( Sphere(Vector(9, 0, 10), 1)));
   CPPUNIT_ASSERT_EQUAL(true, frustum.isInside( Sphere(Vector(10, 0, 10), 1)));
   CPPUNIT_ASSERT_EQUAL(true, frustum.isInside( Sphere(Vector(11, 0, 10), 1)));
   CPPUNIT_ASSERT_EQUAL(false, frustum.isInside( Sphere(Vector(12, 0, 10), 1)));

   // approaching upper plane
   CPPUNIT_ASSERT_EQUAL(true, frustum.isInside( Sphere(Vector(0, 9, 10), 1)));
   CPPUNIT_ASSERT_EQUAL(true, frustum.isInside( Sphere(Vector(0, 10, 10), 1)));
   CPPUNIT_ASSERT_EQUAL(true, frustum.isInside( Sphere(Vector(0, 11, 10), 1)));
   CPPUNIT_ASSERT_EQUAL(false, frustum.isInside( Sphere(Vector(0, 12, 10), 1)));

   // approaching lower plane
   CPPUNIT_ASSERT_EQUAL(true, frustum.isInside( Sphere(Vector(0, -9, 10), 1)));
   CPPUNIT_ASSERT_EQUAL(true, frustum.isInside( Sphere(Vector(0, -10, 10), 1)));
   CPPUNIT_ASSERT_EQUAL(true, frustum.isInside( Sphere(Vector(0, -11, 10), 1)));
   CPPUNIT_ASSERT_EQUAL(false, frustum.isInside( Sphere(Vector(0, -12, 10), 1)));

   // approaching front plane
   CPPUNIT_ASSERT_EQUAL(true, frustum.isInside( Sphere(Vector(0, 0, 5001), 1)));
   CPPUNIT_ASSERT_EQUAL(true, frustum.isInside( Sphere(Vector(0, 0, 5002), 1)));
   CPPUNIT_ASSERT_EQUAL(true, frustum.isInside( Sphere(Vector(0, 0, 5003), 1)));
   CPPUNIT_ASSERT_EQUAL(false, frustum.isInside( Sphere(Vector(0, 0, 5004), 1)));

   // approaching near plane
   CPPUNIT_ASSERT_EQUAL(true, frustum.isInside( Sphere(Vector(0, 0, 2), 1)));
   CPPUNIT_ASSERT_EQUAL(true, frustum.isInside( Sphere(Vector(0, 0, 1), 1)));
   CPPUNIT_ASSERT_EQUAL(false, frustum.isInside( Sphere(Vector(0, 0, 0), 1)));
}

///////////////////////////////////////////////////////////////////////////////

TEST(Sphere, intersects_Ray)
{
   RaycastResult result;
   Sphere sphere(Vector(0, 0, 0), 3);
   Ray ray;
   // various tests for rays going directly through or away from the sphere's center

   ray.setFromPointDirection( Vector(0, 0, -10), Vector(0, 0, 1) );
   CPPUNIT_ASSERT_EQUAL(true,  ray.rayCast( sphere, result ) );

   ray.setFromPointDirection( Vector(0, 0, -10), Vector(0, 0, -1) );
   CPPUNIT_ASSERT_EQUAL(false,  ray.rayCast( sphere, result ) );


   ray.setFromPointDirection( Vector(0, 0, 10), Vector(0, 0, 1) );
   CPPUNIT_ASSERT_EQUAL(false,  ray.rayCast( sphere, result ) );

   ray.setFromPointDirection( Vector(0, 0, 10), Vector(0, 0, -1) );
   CPPUNIT_ASSERT_EQUAL(true,  ray.rayCast( sphere, result ) );


   ray.setFromPointDirection( Vector(-10, 0, 0), Vector(1, 0, 0) );
   CPPUNIT_ASSERT_EQUAL(true,  ray.rayCast( sphere, result ) );

   ray.setFromPointDirection( Vector(-10, 0, 0), Vector(-1, 0, 0) );
   CPPUNIT_ASSERT_EQUAL(false,  ray.rayCast( sphere, result ) );


   ray.setFromPointDirection( Vector(10, 0, 0), Vector(1, 0, 0) );
   CPPUNIT_ASSERT_EQUAL(false,  ray.rayCast( sphere, result ) );

   ray.setFromPointDirection( Vector(10, 0, 0), Vector(-1, 0, 0) );
   CPPUNIT_ASSERT_EQUAL(true,  ray.rayCast( sphere, result ) );


   ray.setFromPointDirection( Vector(0, -10, 0), Vector(0, 1, 0) );
   CPPUNIT_ASSERT_EQUAL(true,  ray.rayCast( sphere, result ) );

   ray.setFromPointDirection( Vector(0, -10,  0), Vector(0, -1, 0) );
   CPPUNIT_ASSERT_EQUAL(false,  ray.rayCast( sphere, result ) );
 

   ray.setFromPointDirection( Vector(0, 10, 0), Vector(0, 1, 0) );
   CPPUNIT_ASSERT_EQUAL(false,  ray.rayCast( sphere, result ) );

   ray.setFromPointDirection( Vector(0, 10,  0), Vector(0, -1, 0) );
   CPPUNIT_ASSERT_EQUAL(true,  ray.rayCast( sphere, result ) );


   // tests testing rays going through the sphere, but not directly through it's center
   ray.setFromPointDirection( Vector(2, 0, -10), Vector(0, 0, 1) );
   CPPUNIT_ASSERT_EQUAL(true,  ray.rayCast( sphere, result ) );

   ray.setFromPointDirection( Vector(2, 0,  -10), Vector(0, 0, -1) );
   CPPUNIT_ASSERT_EQUAL(false,  ray.rayCast( sphere, result ) );


   ray.setFromPointDirection( Vector(-2, 0, 10), Vector(0, 0, 1) );
   CPPUNIT_ASSERT_EQUAL(false,  ray.rayCast( sphere, result ) );

   ray.setFromPointDirection( Vector(-2, 0,  10), Vector(0, 0, -1) );
   CPPUNIT_ASSERT_EQUAL(true,  ray.rayCast( sphere, result ) );


   ray.setFromPointDirection( Vector(-10, 0, 2), Vector(1, 0, 0) );
   CPPUNIT_ASSERT_EQUAL(true,  ray.rayCast( sphere, result ) );

   ray.setFromPointDirection( Vector(-10, 0, 2), Vector(-1, 0, 0) );
   CPPUNIT_ASSERT_EQUAL(false,  ray.rayCast( sphere, result ) );


   ray.setFromPointDirection( Vector(10, 0, -2), Vector(1, 0, 0) );
   CPPUNIT_ASSERT_EQUAL(false,  ray.rayCast( sphere, result ) );

   ray.setFromPointDirection( Vector(10, 0, -2), Vector(-1, 0, 0) );
   CPPUNIT_ASSERT_EQUAL(true,  ray.rayCast( sphere, result ) );


   ray.setFromPointDirection( Vector(2, -10, 0), Vector(0, 1, 0) );
   CPPUNIT_ASSERT_EQUAL(true,  ray.rayCast( sphere, result ) );

   ray.setFromPointDirection( Vector(2, -10,  0), Vector(0, -1, 0) );
   CPPUNIT_ASSERT_EQUAL(false,  ray.rayCast( sphere, result ) );


   ray.setFromPointDirection( Vector(0, 10, 2), Vector(0, 1, 0) );
   CPPUNIT_ASSERT_EQUAL(false,  ray.rayCast( sphere, result ) );

   ray.setFromPointDirection( Vector(0, 10,  2), Vector(0, -1, 0) );
   CPPUNIT_ASSERT_EQUAL(true,  ray.rayCast( sphere, result ) );

   // tests testing rays that originate inside the sphere
   ray.setFromPointDirection( Vector(0, 0, 0), Vector(0, 0, 1) );
   CPPUNIT_ASSERT_EQUAL(true,  ray.rayCast( sphere, result ) );

   ray.setFromPointDirection( Vector(0, 0,  0), Vector(0, 0, -1) );
   CPPUNIT_ASSERT_EQUAL(true,  ray.rayCast( sphere, result ) );

   ray.setFromPointDirection( Vector(0, 0, 0), Vector(1, 0, 0) );
   CPPUNIT_ASSERT_EQUAL(true,  ray.rayCast( sphere, result ) );

   ray.setFromPointDirection( Vector(0, 0, 0), Vector(-1, 0, 0) );   
   CPPUNIT_ASSERT_EQUAL(true,  ray.rayCast( sphere, result ) );

   ray.setFromPointDirection( Vector(0, 0, 0), Vector(0, 1, 0) );
   CPPUNIT_ASSERT_EQUAL(true,  ray.rayCast( sphere, result ) );

   ray.setFromPointDirection( Vector(0, 0, 0), Vector(0, -1, 0) );   
   CPPUNIT_ASSERT_EQUAL(true,  ray.rayCast( sphere, result ) );


   ray.setFromPointDirection( Vector(1, 1, 0), Vector(0, 0, 1) );   
   CPPUNIT_ASSERT_EQUAL(true,  ray.rayCast( sphere, result ) );

   ray.setFromPointDirection( Vector(0, 1, -1), Vector(0, 0, -1) );   
   CPPUNIT_ASSERT_EQUAL(true,  ray.rayCast( sphere, result ) );

   ray.setFromPointDirection( Vector(-1, 0, 1), Vector(1, 0, 0) );   
   CPPUNIT_ASSERT_EQUAL(true,  ray.rayCast( sphere, result ) );

   ray.setFromPointDirection( Vector(0, 1, 0), Vector(-1, 0, 0) );   
   CPPUNIT_ASSERT_EQUAL(true,  ray.rayCast( sphere, result ) );

   ray.setFromPointDirection( Vector(0, 2, 0.5f), Vector(0, 1, 0) );   
   CPPUNIT_ASSERT_EQUAL(true,  ray.rayCast( sphere, result ) );

   ray.setFromPointDirection( Vector(1.5f, 0, 1.5f), Vector(0, -1, 0) );   
   CPPUNIT_ASSERT_EQUAL(true,  ray.rayCast( sphere, result ) );
}

///////////////////////////////////////////////////////////////////////////////

TEST(Ray, intersects_Plane)
{
   RaycastResult result;
   Plane plane;
   plane.set( Quad_Neg_0010, Float_0 );
   Ray ray;

   ray.setFromPointDirection( Vector(0, 0, -1), Vector(0, 0, 1) );   
   CPPUNIT_ASSERT_EQUAL(true,  ray.rayCast( plane, result ) );
   COMPARE_VEC(Vector(0, 0, 0), result.m_contactPt);
   
   ray.setFromPointDirection( Vector(0, 0, -1), Vector(0, 0, -1) );   
   CPPUNIT_ASSERT_EQUAL(false,  ray.rayCast( plane, result ) );

   // ray coming in from behind the plane - in that case even if it crosses it, 
   // there will be no intersection - the plane points elsewhere
   ray.setFromPointDirection( Vector(0, 0, 1), Vector(0, 0, -1) );   
   CPPUNIT_ASSERT_EQUAL(true,  ray.rayCast( plane, result ) );
   COMPARE_VEC(Vector(0, 0, 0), result.m_contactPt);

   ray.setFromPointDirection( Vector(0, 0, 1), Vector(0, 0, 1) );   
   CPPUNIT_ASSERT_EQUAL(false,  ray.rayCast( plane, result ) );

   // another case
   Plane testPlane;
   testPlane.set( Quad_Neg_0100, Float_0 );
   ray.setFromPointDirection( Vector(0, 0, 0.5f), Vector(0, 0, 1) );   
   CPPUNIT_ASSERT_EQUAL(true,  ray.rayCast( testPlane, result ) );
   COMPARE_VEC(Vector(0, 0, 0.5f), result.m_contactPt);

   // another case
   testPlane.set( Quad_Neg_1000, Float_Minus1 );
   ray.setFromPointDirection( Vector(-2, 0, 0), Vector(0, 0, 1) );   
   CPPUNIT_ASSERT_EQUAL(false,  ray.rayCast( testPlane, result ) );
}

///////////////////////////////////////////////////////////////////////////////

TEST(Triangle, intersects_Ray)
{
   Triangle tri(Vector(-3, 1, 10), Vector(3, 1, 10), Vector(-3, -1, 10));
   RaycastResult result;
   Ray ray;

   // test against triangle interior
   ray.setFromPointDirection( Vector(-1, 0, 0), Vector(0, 0, 1) );   
   CPPUNIT_ASSERT_EQUAL(true, ray.rayCast( tri, result ) );

   // test against triangle edges
   ray.setFromPointDirection( Vector(-3, 0, 0), Vector(0, 0, 1) );   
   CPPUNIT_ASSERT_EQUAL(true, ray.rayCast( tri, result ) );

   ray.setFromPointDirection( Vector(0, 1, 0), Vector(0, 0, 1) );   
   CPPUNIT_ASSERT_EQUAL(true, ray.rayCast( tri, result ) );

   ray.setFromPointDirection( Vector(0, 0, 0), Vector(0, 0, 1) );   
   CPPUNIT_ASSERT_EQUAL(true, ray.rayCast( tri, result ) );

   // test against triangle vertices
   ray.setFromPointDirection( Vector(-3, 1, 0), Vector(0, 0, 1) );   
   CPPUNIT_ASSERT_EQUAL(true, ray.rayCast( tri, result ) );

   ray.setFromPointDirection( Vector(3, 1, 0), Vector(0, 0, 1) );   
   CPPUNIT_ASSERT_EQUAL(true, ray.rayCast( tri, result ) );

   ray.setFromPointDirection( Vector(-3, -1, 0), Vector(0, 0, 1) );   
   CPPUNIT_ASSERT_EQUAL(true, ray.rayCast( tri, result ) );

   // test against triangle exterior
   ray.setFromPointDirection( Vector(-3.1f, 1.1f, 0), Vector(0, 0, 1) );   
   CPPUNIT_ASSERT_EQUAL(false, ray.rayCast( tri, result ) );

   ray.setFromPointDirection( Vector(3.1f, 1.1f, 0), Vector(0, 0, 1) );   
   CPPUNIT_ASSERT_EQUAL(false, ray.rayCast( tri, result ) );

   ray.setFromPointDirection( Vector(-3.1f, -1.1f, 0), Vector(0, 0, 1) );   
   CPPUNIT_ASSERT_EQUAL(false, ray.rayCast( tri, result ) );

   ray.setFromPointDirection( Vector(-3.1f, 0, 0), Vector(0, 0, 1) );   
   CPPUNIT_ASSERT_EQUAL(false, ray.rayCast( tri, result ) );

   ray.setFromPointDirection( Vector(0, 1.1f, 0), Vector(0, 0, 1) );   
   CPPUNIT_ASSERT_EQUAL(false, ray.rayCast( tri, result ) );

   ray.setFromPointDirection( Vector(0, -0.1f, 0), Vector(0, 0, 1) );   
   CPPUNIT_ASSERT_EQUAL(false, ray.rayCast( tri, result ) );


   ray.setFromPointDirection( Vector(0, 0, 8), Vector(0, 1, 0) );   
   CPPUNIT_ASSERT_EQUAL(false, ray.rayCast( tri, result ) );

   ray.setFromPointDirection( Vector(0, 0, 12), Vector(0, 1, 0) );   
   CPPUNIT_ASSERT_EQUAL(false, ray.rayCast( tri, result ) );

   ray.setFromPointDirection( Vector(0, 0, 8), Vector(1, 0, 0) );   
   CPPUNIT_ASSERT_EQUAL(false, ray.rayCast( tri, result ) );

   ray.setFromPointDirection( Vector(0, 0, 12), Vector(1, 0, 0) );   
   CPPUNIT_ASSERT_EQUAL(false, ray.rayCast( tri, result ) );
}

///////////////////////////////////////////////////////////////////////////////
