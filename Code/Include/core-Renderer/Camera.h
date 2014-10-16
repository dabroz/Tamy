/// @file   core-Renderer/Camera.h
/// @brief  3D camera
#pragma once

#include "core\MemoryRouter.h"
#include "core-MVC\Entity.h"
#include "core\Frustum.h"
#include "core\Matrix.h"


///////////////////////////////////////////////////////////////////////////////

struct Frustum;
struct Ray;

///////////////////////////////////////////////////////////////////////////////

/**
 * A camera is a kind of Node that allows to set 
 * camera specific stuff and represents a camera that watches the scene
 */
class Camera : public Entity
{
   DECLARE_ALLOCATOR( Camera, AM_ALIGNED_16 );

public:
   enum ProjectionType
   {
      PT_PERSPECTIVE,
      PT_ORTHO
   };

private:
   ProjectionType       m_projectionType;

   float                m_fov;
   float                m_aspectRatio;
   float                m_nearZPlane;
   float                m_farZPlane;
   float                m_nearPlaneWidth;
   float                m_nearPlaneHeight;

   Matrix               m_mtxView;
   Matrix               m_mtx3DProjection;

   bool                 m_mtxProjectionDirty;

public:
   /**
    * Constructor.
    *
    * @param name       camera name
    */
   Camera( const char* name, ProjectionType projectionType );
   ~Camera();

   /**
    * Sets a new projection type ( with perspective, orthogonal etc. )
    *
    * @param projectionType
    */
   void setProjection( ProjectionType projectionType );

   /**
    * Retrieves current view matrix.
    */
   const Matrix& getViewMtx() const;

   /**
    * Retrieves current projection matrix.
    */
   const Matrix& getProjectionMtx() const;

   /**
    * Sets new near plane dimensions
    *
    * @param width
    * @param height
    */
   void setNearPlaneDimensions( float width, float height );

   /**
    * Returns the dimensions of the new near plane
    *
    * @param outWidth
    * @param outHeight
    */
   void getNearPlaneDimensions( float& outWidth, float& outHeight ) const;

   /**
    * Retrieves the width of the near plane.
    */
   inline float getNearPlaneWidth() const { return m_nearPlaneWidth; }

   /**
    * Retrieves the height of the near plane.
    */
   inline float getNearPlaneHeight() const { return m_nearPlaneHeight; }

   /**
    * Retrieves near plane aspect ration.
    */
   inline float getAspectRatio() const {return m_aspectRatio;}

   /**
    * Sets the camera clipping planes.
    *
    * @param nearZPlane
    * @param farZPlane
    */
   void setClippingPlanes( float nearZPlane, float farZPlane );

   /**
    * Returns the camera clipping planes.
    *
    * @param outNearZPlane
    * @param outFarZPlane
    */
   void getClippingPlanes( float& outNearZPlane, float& outFarZPlane ) const;
   inline float getNearClippingPlane() const {return m_nearZPlane;}
   inline float getFarClippingPlane() const {return m_farZPlane;}


   /**
    * Sets the camera so that it looks at the specified node from
    * the specified distance.
    *
    * @param node
    * @param distance      ( must be >0 )
    */
   void lookAt( Entity& node, const FastFloat& distance, const Vector& upVec = Quad_0010 );

   /**
    * Sets the fov of the camera (angle specified in degrees)
    */
   void setFOV(float angle);

   /**
    * Returns the current FOV angle of the camera (in degrees)
    */
   float getFOV() const;

   /**
    * The method returns the current camera frustum (in global coordinates)
    * The planes in the frustum ARE NORMALIZED
    *
    * @param outFrustum
    */
   void calculateFrustum( Frustum& outFrustum );

   /**
    * Calculates an axis-aligned bounding box around the current view frustum.
    *
    * @param outAABB
    */
   void calculateFrustumAABB( AxisAlignedBox& outAABB ) const;

   /**
    * Calculates an axis-aligned bounding box around the current view frustum.
    *
    * @param nearZ
    * @param farZ
    * @param outAABB
    */
   void calculateFrustumAABBFromRange( float nearZ, float farZ, AxisAlignedBox& outAABB ) const;

   /**
    * The method constructs a ray running from the camera global position
    * through the position on the viewport specified by the method params.
    *
    * The method params should be specified in the viewport coordinates (range <-1, 1>)
    *
    * @param viewportX
    * @param viewportY
    * @param outRay
    */
   void createRay( float viewportX, float viewportY, Ray& outRay );

   /**
    * Called when the rendering viewport gets resized.
    *
    * @param width
    * @param height
    */
   void onViewportResized( uint width, uint height );

   // -------------------------------------------------------------------------
   // Entity implementation
   // -------------------------------------------------------------------------
   void updateTransforms();
   
private:
   void updateViewMtx();
   void updateProjectionMtx();
};

///////////////////////////////////////////////////////////////////////////////
