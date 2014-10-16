/// @file   ext-2DGameLevel/GL2DCollisionBroadphase.h
/// @brief  this structure quickly determines which objects in the 2D game level collide with each other
#pragma once

#include "core\MemoryRouter.h"
#include "core\List.h"
#include "core\Array.h"
#include "ext-2DGameLevel\GL2DCollisionPointsCollector.h"


///////////////////////////////////////////////////////////////////////////////

struct Ray;
struct RayCastCollector;
struct IntersectionResult;
struct Sphere;
struct Vector;

///////////////////////////////////////////////////////////////////////////////

class GL2DCollisionBroadphase
{
   DECLARE_ALLOCATOR( GL2DCollisionBroadphase, AM_DEFAULT );

public:
   GL2DCollisionPointsCollector    m_collisionsBuf;

   List< GL2DBody* >               m_movingBodies;
   List< GL2DBody* >               m_staticBodies;
   List< GL2DBody* >               m_bodies;

public:
   /**
   * Constructor.
   */
   GL2DCollisionBroadphase( );
   ~GL2DCollisionBroadphase( );

   /**
   * Adds a new body to the broad phase.
   *
   * @param body
   * @return  'true' if the body was added
   */
   bool add( GL2DBody* body );

   /**
   * Removes a physical body from the broad phase.
   *
   * @param body
   * @return  'true' if the body was removed
   */
   bool remove( GL2DBody* body );

   /**
   * Called when a body becomes active/inactive.
   *
   * @param body
   */
   void updateBodyState( GL2DBody* body );

   /**
   * Clears the contents of the broad phase.
   */
   void clear( );

   /**
   * Runs the collision detection algorithm.
   *
   * @param deltaTime
   */
   void detectCollisions( const FastFloat& deltaTime );

private:
   /**
   * This collision detection method will sample sphere's movement with a specific granularity to find the first contact point approximation
   * It works best for small displacements of objects between consecutive frames.
   * When the distances are higher, use the adaptive algorithm.
   *
   * Bodies need to be constant - they don't change in result to collision detection
   *
   * @param bodyA
   * @param bodyB
   * @param deltaTime
   */
   bool sampledCollisionDetection( GL2DBody* bodyA, GL2DBody* bodyB, const FastFloat& deltaTime );
};

///////////////////////////////////////////////////////////////////////////////
