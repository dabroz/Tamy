/// @file   TamyEditor/UnconstrainedMotionController.h
/// @brief  spaceship-like movement for the node
#pragma once

#include "core\MemoryRouter.h"


///////////////////////////////////////////////////////////////////////////////

class Camera;
struct Vector;
struct EulerAngles;
struct Matrix;

///////////////////////////////////////////////////////////////////////////////

class TamyCameraController
{
   DECLARE_ALLOCATOR( TamyCameraController, AM_DEFAULT );

protected:
   Camera&    m_controlledNode;

public:
   /**
    * Constructor.
    *
    * @param controlledNode
    */
   TamyCameraController( Camera& controlledNode );
   virtual ~TamyCameraController() {}

   /**
    * Moves the camera in the specified direction.
    */
   virtual void move( const Vector& transVec ) = 0;

   /**
   * 2D rotation around global Y axis
   */
   virtual void rotate( float pitch, float yaw ) = 0;

   /**
    * Returns the controlled camera.
    */
   Camera& getCamera() const {
      return m_controlledNode;
   }

   /**
    * Teleports the camera to the specified position.
    *
    * @param vec
    */
   void setPosition( const Vector& vec );

   /**
    * Returns the camera's right vector.
    *
    * @param outVec
    */
   void getRightVec( Vector& outVec ) const;

   /**
   * Returns the camera's up vector.
   *
   * @param outVec
   */
   void getUpVec( Vector& outVec ) const;

   /**
   * Returns the camera's look vector.
   *
   * @param outVec
   */
   void getLookVec( Vector& outVec ) const;

   /**
   * Returns the camera's position.
   *
   * @param outVec
   */
   void getPosition( Vector& outVec ) const;

protected:
   void regenerateVectors( Vector& rightVec, Vector& upVec, Vector& lookVec ) const;

   void calcRotationMtx( float pitch, float yaw, const Matrix& currRotationMtx, Matrix& outMtx );
};

///////////////////////////////////////////////////////////////////////////////

/**
 * This class is responsible for controlling the transformation
 * of a node.
 * All the methods operate in the GLOBAL COORDINATE SYSTEM,
 * (as opposed to the node's local coordinate system),
 * allowing the user of this class to take control of the
 * node and move it around the scene
 */
class UnconstrainedMotionController : public TamyCameraController
{
   DECLARE_ALLOCATOR( UnconstrainedMotionController, AM_DEFAULT );

public:
   /**
    * Constructor.
    *
    * @param controlledNode
    */
   UnconstrainedMotionController( Camera& controlledNode );

   // -------------------------------------------------------------------------
   // TamyCameraController implementation
   // -------------------------------------------------------------------------
   void move( const Vector& transVec );
   void rotate( float pitch, float yaw );

};

///////////////////////////////////////////////////////////////////////////////

class OrbitingMotionController : public TamyCameraController
{
   DECLARE_ALLOCATOR( OrbitingMotionController, AM_DEFAULT );

public:
   /**
   * Constructor.
   *
   * @param controlledNode
   */
   OrbitingMotionController( Camera& controlledNode );

   // -------------------------------------------------------------------------
   // TamyCameraController implementation
   // -------------------------------------------------------------------------
   void move( const Vector& transVec );
   void rotate( float pitch, float yaw );
};

///////////////////////////////////////////////////////////////////////////////
