/// @file   ext-2DGameLevel\GL2DBody.h
/// @brief  a body that resides in a 2D game level
#pragma once

#include "core\MemoryRouter.h"
#include "core-MVC\Component.h"
#include "core\Vector.h"


///////////////////////////////////////////////////////////////////////////////

class GL2DWorld;
class GL2DCollisionsResolver;
class GL2DShape;
struct GL2DCollisionContactDesc;
struct RaycastResult;
struct Ray;

///////////////////////////////////////////////////////////////////////////////

/**
* A body that resides in a 2D game level.
*
* The architecture allows the body to be simulated by many different GameLevel2DWorlds,
* and it's the world that maintains the state of the body - thus all methods
* that affect the state of runtime simulation require you to specify the instance
* of world that simulates the body so that the body can access that set of simulation data
* dedicated to it.
*/
class GL2DBody : public Component
{
   DECLARE_ALLOCATOR( GL2DBody, AM_DEFAULT );
   DECLARE_CLASS( );

private:
   // static data
   bool                          m_isMovable;
   GL2DShape*                      m_shape;

   // runtime data
   Matrix                        m_simulationGlobalMatrix;
   Vector                        m_linearVelocity;
   Vector                        m_angularVelocity;

   Matrix                        m_newIntegratedTransform;

public:
   /**
    * Default constructor.
    *
    * @param name
    */
   GL2DBody( const char* name = "GL2DBody" );

   /**
    * Copy constructor.
    */
   GL2DBody( const GL2DBody& rhs );
   ~GL2DBody( );

   /**
   * Returns the shape of the body.
   */
   inline GL2DShape* getShape( ) const {
      return m_shape;
   }

   /**
    * Changes the shape of the body.
    *
    * @param shape
    */
   void setShape( GL2DShape* shape );

   /**
    * Tells if the body can move around.
    */
   inline bool isMovable() const {
      return m_isMovable;
   }

   /**
    * Defines if the body can move or not.
    *
    * @param movable
    */
   void setMovable( bool movable );

   // -------------------------------------------------------------------------
   // Runtime API
   // -------------------------------------------------------------------------

   /**
   * Initializes the body when the simulation starts playing.
   *
   * @param world
   */
   void initialize( GL2DWorld& world );

   /**
    * Deinitializes the body when the simulation stops playing.
    *
    * @param world
    */
   void deinitialize( GL2DWorld& world );

   /**
    * Sets the linear velocity of the body
    *
    * @param vel
    */
   inline void setLinearVelocity( const Vector& vel ) {
      if ( m_isMovable )
      {
         m_linearVelocity = vel;
      }
   }

   /**
    * Returns the linear velocity the body is traveling with.
    */
   inline const Vector& getLinearVelocity( ) const {
      return m_linearVelocity;
   }

   /**
    * Sets the angular velocity of the body
    *
    * @param vel
    */
   inline void setAngularVelocity( const Vector& vel ) {
      m_angularVelocity = vel;
   }

   /**
    * Returns the angular velocity the body is traveling with.
    */
   inline const Vector& getAngularVelocity( ) const {
      return m_angularVelocity;
   }

   /**
    * Returns a transform that was put together during forces integration step and
    * describes the new position and orientation of the body.
    */
   inline const Matrix& getIntegratedTransform( ) const {
      return m_newIntegratedTransform;
   }

   /**
    * Sets the new integrated transform that will later on be used to set the new transformation
    * of parent entity.
    */
   inline void setIntegratedTransform( const Matrix& transform ) {
      m_newIntegratedTransform = transform;
   }

   /**
    * Returns the global matrix of the object used to mark the transform
    * of the body during the simulation frame.
    */
   inline const Matrix& getSimulationGlobalMtx( ) const {
      return m_simulationGlobalMatrix;
   }

   /**
    * Checks if the body collides with the specified ray.
    *
    * @param ray
    * @param transform
    * @param outResult
    * @return                       'true' if a collision takes place, 'false' otherwise
    */
   bool rayCast( const Ray& ray, const Matrix& transform, RaycastResult& outResult ) const;

   // -------------------------------------------------------------------------
   // Component implementation
   // -------------------------------------------------------------------------
   void updateTransforms( );

   // -------------------------------------------------------------------------
   // ReflectionObject implementation
   // -------------------------------------------------------------------------
   void onPrePropertyChanged( ReflectionProperty& property );

protected:
   // -------------------------------------------------------------------------
   // PhysicsWorld API
   // -------------------------------------------------------------------------
   friend class GL2DWorld;
   friend class GL2DCollisionResolver;

   /**
   * Simulates body movement.
   *
   * @param timeElapsed
   * @param entityMtx
   */
   void integrate( const FastFloat& timeElapsed, const Matrix& entityMtx );

   /**
   * Initializes the global matrix used during the simulation frame.
   */
   inline void initializeSimulationGlobalMtx( )
   {
      m_simulationGlobalMatrix = getGlobalMtx( );
   }

   /**
   * After every iteration in a single simulation frame, the new integrated matrix is
   * committed to the simulation global matrix, so that it can serve as the basis
   * for further movement analysis in the next iteration.
   */
   inline void updateSimulationGlobalMtx( )
   {
      m_simulationGlobalMatrix = m_newIntegratedTransform;
   }
};

///////////////////////////////////////////////////////////////////////////////
