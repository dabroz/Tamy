#include "core-TestFramework\TestFramework.h"
#include "core-Renderer\Camera.h"
#include "core-TestFramework\MatrixWriter.h"
#include "core\Frustum.h"
#include "core\Sphere.h"
#include "core\AxisAlignedBox.h"
#include "core\Ray.h"
#include "core-Renderer\Renderer.h"
#include "core-Renderer\RendererImplementation.h"
#include "core\Math.h"


///////////////////////////////////////////////////////////////////////////////

TEST(Camera, frustrumCreation)
{
   Camera camera( "camera", Camera::PT_PERSPECTIVE );
   camera.setNearPlaneDimensions(10, 10);
   camera.setClippingPlanes(10, 100);
   camera.setFOV(90);

   Frustum frustrum;
   camera.updateTransforms();
   camera.calculateFrustum( frustrum );

   CPPUNIT_ASSERT_EQUAL(false, frustrum.isInside( Sphere(Vector(0, 0, -2), 1) ) );
   CPPUNIT_ASSERT_EQUAL(false, frustrum.isInside( Sphere(Vector(0, 0, 8), 1) ) );
   CPPUNIT_ASSERT_EQUAL(false, frustrum.isInside( Sphere(Vector(0, 0, 102), 1)));
   CPPUNIT_ASSERT_EQUAL(false, frustrum.isInside( Sphere(Vector(0, 0, 0), 1)));

   CPPUNIT_ASSERT_EQUAL(true, frustrum.isInside( Sphere(Vector(0, 0, 100), 1)));
   CPPUNIT_ASSERT_EQUAL(true, frustrum.isInside( Sphere(Vector(0, 0, 8), 2)));
   CPPUNIT_ASSERT_EQUAL(true, frustrum.isInside( Sphere(Vector(0, 0, 10), 1)));
   CPPUNIT_ASSERT_EQUAL(true, frustrum.isInside( Sphere(Vector(0, 0, 50), 1)));
   CPPUNIT_ASSERT_EQUAL(true, frustrum.isInside( Sphere(Vector(-20, 0, 50), 1)));
   CPPUNIT_ASSERT_EQUAL(true, frustrum.isInside( Sphere(Vector(20, 0, 50), 1)));
   CPPUNIT_ASSERT_EQUAL(true, frustrum.isInside( Sphere(Vector(0, -20, 50), 1)));
   CPPUNIT_ASSERT_EQUAL(true, frustrum.isInside( Sphere(Vector(0, 20, 50), 1)));

   // camera rotated
   Quaternion rotY;
   rotY.setAxisAngle( Quad_0100, FastFloat::fromFloat( DEG2RAD( 90 ) ) );
   camera.accessLocalMtx().setRotation( rotY );
   camera.updateTransforms();
   camera.calculateFrustum( frustrum );

   CPPUNIT_ASSERT_EQUAL(true, frustrum.isInside( Sphere(Vector(10, 0, 0), 1)));
   CPPUNIT_ASSERT_EQUAL(true, frustrum.isInside( Sphere(Vector(100, 0, 0), 1)));
   CPPUNIT_ASSERT_EQUAL(true, frustrum.isInside( Sphere(Vector(50, 0, 0), 1)));

   // camera rotated
   rotY.setAxisAngle( Quad_0100, FastFloat::fromFloat( DEG2RAD( -90 ) ) );
   camera.accessLocalMtx().setRotation( rotY );
   camera.updateTransforms();
   camera.calculateFrustum( frustrum );

   CPPUNIT_ASSERT_EQUAL(true, frustrum.isInside( Sphere(Vector(-10, 0, 0), 1)));
   CPPUNIT_ASSERT_EQUAL(true, frustrum.isInside( Sphere(Vector(-100, 0, 0), 1)));
   CPPUNIT_ASSERT_EQUAL(true, frustrum.isInside( Sphere(Vector(-50, 0, 0), 1)));

}

///////////////////////////////////////////////////////////////////////////////

TEST(Camera, createRay)
{
   Camera camera( "camera", Camera::PT_PERSPECTIVE);
   Frustum frustum;
   camera.updateTransforms();
   camera.calculateFrustum( frustum );
   Vector expectedNormal;

   // a basic plane running straight through the screen's center
   Ray result;
   camera.createRay( 0, 0, result );
   COMPARE_VEC( Vector(0, 0, 0), result.origin );
   COMPARE_VEC( Vector(0, 0, 1), result.direction );

   // test the rays running through the frustum's clipping planes:
   // - bottom plane
   Vector planeNormal;
   frustum.planes[FP_BOTTOM].getNormal( planeNormal );
   expectedNormal.setCross( planeNormal, Quad_Neg_1000 );
   expectedNormal.normalize();

   camera.createRay( 0, -1, result );
   COMPARE_VEC(Vector(0, 0, 0), result.origin);
   COMPARE_VEC(expectedNormal, result.direction);

   // - top plane
   frustum.planes[FP_TOP].getNormal( planeNormal );
   expectedNormal.setCross( planeNormal, Quad_1000 );
   expectedNormal.normalize();

   camera.createRay( 0, 1, result );
   COMPARE_VEC(Vector(0, 0, 0), result.origin);
   COMPARE_VEC(expectedNormal, result.direction);

   // - left plane
   frustum.planes[FP_LEFT].getNormal( planeNormal );
   expectedNormal.setCross( Quad_Neg_0100, planeNormal );
   expectedNormal.normalize();

   camera.createRay( -1, 0, result );
   COMPARE_VEC(Vector(0, 0, 0), result.origin);
   COMPARE_VEC(expectedNormal, result.direction);

   // - right plane
   frustum.planes[FP_RIGHT].getNormal( planeNormal );
   expectedNormal.setCross( Quad_0100, planeNormal );
   expectedNormal.normalize();

   camera.createRay( 1, 0, result );
   COMPARE_VEC(Vector(0, 0, 0), result.origin);
   COMPARE_VEC(expectedNormal, result.direction);
}

///////////////////////////////////////////////////////////////////////////////

TEST( Camera, frustumAABB )
{
   Camera camera( "camera", Camera::PT_PERSPECTIVE );
   camera.setNearPlaneDimensions(10, 10);
   camera.setClippingPlanes(10, 100);
   camera.setFOV(90);

   AxisAlignedBox calculatedBB;

   // camera at the origin, pointing down the Z axis
   {
      camera.calculateFrustumAABB( calculatedBB );
      COMPARE_VEC( Vector( -100, -100, 10 ), calculatedBB.min );
      COMPARE_VEC( Vector( 100, 100, 100 ), calculatedBB.max );
   }

   // camera in an arbitrary position with an arbitrary orientation
   {
      Quaternion rotY;
      rotY.setAxisAngle( Quad_0100, FastFloat::fromFloat( DEG2RAD( 45 ) ) );
      camera.accessLocalMtx().setRotation( rotY );
      camera.accessLocalMtx().setPosition<3>( Vector( 10, 20, 30 ) );
      camera.updateTransforms();

      camera.calculateFrustumAABB( calculatedBB );
      COMPARE_VEC( Vector( -53.6396f, -80, -33.6396f ), calculatedBB.min );
      COMPARE_VEC( Vector( 151.42136f, 120, 171.42136f ), calculatedBB.max );
   }
}

///////////////////////////////////////////////////////////////////////////////
