#include "core-TestFramework\TestFramework.h"
#include "core\Math.h"
#include "core-Renderer\Renderer.h"
#include "core-Renderer\RendererImplementation.h"
#include "core-Renderer\Camera.h"


///////////////////////////////////////////////////////////////////////////////

namespace // anonymous
{
   class RendererImplementationMock : public RendererImplementation
   {
   };
} // anonymous

///////////////////////////////////////////////////////////////////////////////

TEST( Frustum, frustumBoundingBox )
{   Camera camera( "camera", Camera::PT_PERSPECTIVE );
   camera.setNearPlaneDimensions(10, 10);
   camera.setClippingPlanes(10, 100);
   camera.setFOV(90);

   Frustum frustum;
   AxisAlignedBox frustumBounds;
   Transform cameraTransform;

   // identity matrix
   camera.updateTransforms();
   camera.calculateFrustum( frustum );
   frustum.calculateBoundingBox( frustumBounds );
   COMPARE_VEC( Vector( -100.0f, -100.0f, 10.0f ), frustumBounds.min );
   COMPARE_VEC( Vector(  100.0f,  100.0f, 100.0f ), frustumBounds.max );

   // changed position
   cameraTransform.m_translation.set( 10, 20, 30 );
   cameraTransform.toMatrix( camera.accessLocalMtx() );
   camera.updateTransforms();
   camera.calculateFrustum( frustum );
   frustum.calculateBoundingBox( frustumBounds );
   COMPARE_VEC( Vector( -90.0f, -80.0f, 40.0f ), frustumBounds.min );
   COMPARE_VEC( Vector(  110.0f,  120.0f, 130.0f ), frustumBounds.max );

   // rotated & translated camera
   cameraTransform.m_rotation.setAxisAngle( Vector_OY, FastFloat::fromFloat( DEG2RAD( 90 ) ) );
   cameraTransform.toMatrix( camera.accessLocalMtx() );
   camera.updateTransforms();
   camera.calculateFrustum( frustum );
   frustum.calculateBoundingBox( frustumBounds );
   COMPARE_VEC( Vector( 20.0f, -80.0f, -70.0f ), frustumBounds.min );
   COMPARE_VEC( Vector(  110.0f, 120.0f, 130.0f ), frustumBounds.max );

   // rotated camera
   cameraTransform.m_translation.set( 0, 0, 0 );
   cameraTransform.toMatrix( camera.accessLocalMtx() );
   camera.updateTransforms();
   camera.calculateFrustum( frustum );
   frustum.calculateBoundingBox( frustumBounds );
   COMPARE_VEC( Vector( 10.0f, -100.0f, -100.0f ), frustumBounds.min );
   COMPARE_VEC( Vector(  100.0f, 100.0f, 100.0f ), frustumBounds.max );
}

///////////////////////////////////////////////////////////////////////////////

TEST( Frustum, pointFrustumIntersection )
{
   Camera camera( "camera", Camera::PT_PERSPECTIVE );
   camera.setNearPlaneDimensions(10, 10);
   camera.setClippingPlanes(0.1f, 100);
   camera.setFOV(90);

   Frustum frustum;
   Vector ptVolume( Vector_ZERO );
   camera.updateTransforms();
   camera.calculateFrustum( frustum );

   ptVolume.set( 0, 0, 1 );
   CPPUNIT_ASSERT_EQUAL( true, frustum.isInside( ptVolume ) );

   ptVolume.set( 0, 0, 100.0f );
   CPPUNIT_ASSERT_EQUAL( true, frustum.isInside( ptVolume ) );

   ptVolume.set( -50, 50, 100.0f  );
   CPPUNIT_ASSERT_EQUAL( true, frustum.isInside( ptVolume ) );

   ptVolume.set( -5, -5, 10.0f  );
   CPPUNIT_ASSERT_EQUAL( true, frustum.isInside( ptVolume ) );

   ptVolume.set( -5, 5, 10.0f  );
   CPPUNIT_ASSERT_EQUAL( true, frustum.isInside( ptVolume ) );

   ptVolume.set( 5, -5, 10.0f  );
   CPPUNIT_ASSERT_EQUAL( true, frustum.isInside( ptVolume ) );

   ptVolume.set( 5, 5, 10.0f  );
   CPPUNIT_ASSERT_EQUAL( true, frustum.isInside( ptVolume ) );


   ptVolume.set( 0, 0, 0 );
   CPPUNIT_ASSERT_EQUAL( false, frustum.isInside( ptVolume ) );

   ptVolume.set( 0, 0, 101.0f );
   CPPUNIT_ASSERT_EQUAL( false, frustum.isInside( ptVolume ) );

   ptVolume.set( -15, -15, 10.0f  );
   CPPUNIT_ASSERT_EQUAL( false, frustum.isInside( ptVolume ) );

   ptVolume.set( -15, 15, 10.0f  );
   CPPUNIT_ASSERT_EQUAL( false, frustum.isInside( ptVolume ) );

   ptVolume.set( 15, -15, 10.0f  );
   CPPUNIT_ASSERT_EQUAL( false, frustum.isInside( ptVolume ) );

   ptVolume.set( 15, 15, 10.0f  );
   CPPUNIT_ASSERT_EQUAL( false, frustum.isInside( ptVolume ) );
}

///////////////////////////////////////////////////////////////////////////////
