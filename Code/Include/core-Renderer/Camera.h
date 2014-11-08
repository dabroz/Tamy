/// @file   core-Renderer/Camera.h
/// @brief  3D camera
#pragma once

#include "core\MemoryRouter.h"
#include "core-MVC\Transformable.h"
#include "core\Frustum.h"
#include "core\Matrix.h"


///////////////////////////////////////////////////////////////////////////////

struct Frustum;
struct Ray;
class Entity;

///////////////////////////////////////////////////////////////////////////////

/**
 * A camera is a kind of Node that allows to set 
 * camera specific stuff and represents a camera that watches the scene
 */
class Camera : public Transformable
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

   Matrix               m_localMtx;
   Matrix               m_globalMtx;
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
   // Transforms management implementation
   // -------------------------------------------------------------------------

   /**
   * This is the matrix that describes the node's position in relation
   * to the position of its parent.
   * It the node doesn't have a parent, this one will be equal
   * to the global matrix
   */
   const Matrix& getLocalMtx() const {
      return m_localMtx;
   }

   /**
   * Assigns the node a new local matrix.
   */
   void setLocalMtx( const Matrix& localMtx );

   /**
   * The method allows to access the matrix of the node directly,
   * skipping the setters.
   * Manipulating the matrix in this way is sometimes necessary
   * as various libs manipulate pointers to matrices.
   * Not to worry - the global matrix will always remain in sync
   */
   Matrix& accessLocalMtx() {
      return m_localMtx;
   }

   /**
    * This is the matrix that describes the node's absolute world position
    * (unlike the local matrix which describes the position relative to node's
    * parent).
    */
   inline const Matrix& getGlobalMtx() const {
      return m_globalMtx;
   }

   /*
    * A group of accessors to the local coordinate system vectors
    */
   void setRightVec( const Vector& vec );
   void setUpVec( const Vector& vec );
   void setLookVec( const Vector& vec );
   void setPosition( const Vector& vec );
   void getRightVec( Vector& outRightVec ) const;
   void getUpVec( Vector& outUpVec ) const;
   void getLookVec( Vector& outLookVec ) const;
   void getPosition( Vector& outPos ) const;

   /*
    * Returns the global coordinate system vectors.
    *
    * @param outRightVec
    * @param outUpVec
    * @param outLookVec
    * @Param outPos
    */
   void getGlobalVectors( Vector& outRightVec, Vector& outUpVec, Vector& outLookVec, Vector& outPos ) const;
   
   // -------------------------------------------------------------------------
   // Transformable implementation
   // -------------------------------------------------------------------------
   void updateTransforms() override;

private:
   void updateViewMtx();
   void updateProjectionMtx();
};

///////////////////////////////////////////////////////////////////////////////
