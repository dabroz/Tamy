/// @file   ext-2DGameLevel\GL2DWorld.h
/// @brief  a 2d game level simulation world
#pragma once

#include "core\RaycastResult.h"
#include "core-MVC\ModelView.h"
#include "core\MemoryRouter.h"
#include "core\List.h"
#include "core\Array.h"
#include "core\Math.h"


///////////////////////////////////////////////////////////////////////////////

class GL2DListener;
class GL2DBody;
class GL2DCollisionBroadphase;
class RuntimeDataBuffer;
struct GL2DRayCastCollector;
struct Ray;

///////////////////////////////////////////////////////////////////////////////

/**
 * A 2d game level simulation world.
 */
class GL2DWorld : public ModelView
{
   DECLARE_ALLOCATOR( GL2DWorld, AM_DEFAULT );

private:
   List< GL2DListener* >           m_listeners;

   RuntimeDataBuffer*              m_data;
   List < GL2DBody* >              m_bodies;

   // runtime params
   bool                            m_isPlaying;

   GL2DCollisionBroadphase*        m_broadphase;

public:
   /**
    * Constructor.
    */
   GL2DWorld( );
   ~GL2DWorld( );

   /**
    * Enables/disables physics simulation.
    *
    * @param enable
    */
   void enableSimulation( bool enable );

   /**
    * Tells if the simulation should be updated or not.
    */
   inline bool isRunningSimulation( ) const {
      return m_isPlaying;
   }

   /**
    * Runs a ray cast query int the world
    */
   void rayCast( const Ray& ray, GL2DRayCastCollector& outResults ) const;

   /**
    * Updates the simulation.
    *
    * @param timneElapsed
    */
   void tickSimulation( float timeElapsed );

   // -------------------------------------------------------------------------
   // ModelView implementation
   // -------------------------------------------------------------------------
   void onAttachedToModel( Model& model );
   void onNodeAdded( SceneNode* node );
   void onNodeRemoved( SceneNode* node );
   void onNodeChanged( SceneNode* node );
   void resetContents( Model& scene );

   // -------------------------------------------------------------------------
   // Listeners management
   // -------------------------------------------------------------------------
   /**
   * Attaches a new listener.
   *
   * @param listener
   */
   void attachListener( GL2DListener* listener );

   /**
   * Detaches a listener.
   *
   * @param listener
   */
   void detachListener( GL2DListener* listener );

   // -------------------------------------------------------------------------
   // Runtime data accessors
   // -------------------------------------------------------------------------
   inline RuntimeDataBuffer& data( ) const {
      return *m_data;
   }

private:
   void processBodyAddition( SceneNode* node );

   void singleSimulationTick( const FastFloat& deltaTime );
};

///////////////////////////////////////////////////////////////////////////////

/**
* Structure used to store the results of a ray-cast query.
*/
struct GL2DRayCastCollector
{
   DECLARE_ALLOCATOR( GL2DRayCastCollector, AM_DEFAULT );

   List< RaycastResult >      m_hitsDesc;
   uint                       m_resultsLeft;

   GL2DRayCastCollector( uint maxResults = 8 ) : m_resultsLeft( maxResults ) {}
};

///////////////////////////////////////////////////////////////////////////////
