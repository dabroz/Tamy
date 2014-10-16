#include "ext-2DGameLevel\GL2DWorld.h"
#include "ext-2DGameLevel\GL2DSystem.h"
#include "ext-2DGameLevel\GL2DListener.h"
#include "ext-2DGameLevel\GL2DBody.h"
#include "ext-2DGameLevel\GL2DCollisionBroadphase.h"
#include "ext-2DGameLevel\GL2DCollisionResolver.h"

#include "core-MVC\Entity.h"
#include "core-MVC\Model.h"
#include "core\ListUtils.h"
#include "core\RaycastResult.h"
#include "core\RuntimeData.h"


///////////////////////////////////////////////////////////////////////////////

GL2DWorld::GL2DWorld( )
   : m_isPlaying( false )
{
   m_data = new RuntimeDataBuffer( );
   m_broadphase = new GL2DCollisionBroadphase( );

   GL2DSystem& physicsSys = TSingleton< GL2DSystem >::getInstance( );
   physicsSys.registerWorld( this );
}

///////////////////////////////////////////////////////////////////////////////

GL2DWorld::~GL2DWorld( )
{
   GL2DSystem& physicsSys = TSingleton< GL2DSystem >::getInstance( );
   physicsSys.unregisterWorld( this );

   delete m_broadphase;
   m_broadphase = NULL;

   delete m_data;
   m_data = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void GL2DWorld::enableSimulation( bool enable )
{
   m_isPlaying = enable;

   // initialize/deinitialize registered bodies
   if ( m_isPlaying )
   {
      for ( List< GL2DBody* >::iterator it = m_bodies.begin( ); !it.isEnd( ); ++it )
      {
         GL2DBody* body = *it;
         body->initialize( *this );
      }
   }
   else
   {
      for ( List< GL2DBody* >::iterator it = m_bodies.begin( ); !it.isEnd( ); ++it )
      {
         GL2DBody* body = *it;
         body->deinitialize( *this );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void GL2DWorld::tickSimulation( float timeElapsed )
{
   const FastFloat deltaTime = FastFloat::fromFloat( timeElapsed );

   // initialize global matrices used during the simulation
   for ( List< GL2DBody* >::iterator it = m_bodies.begin( ); !it.isEnd( ); ++it )
   {
      GL2DBody* body = *it;
      body->initializeSimulationGlobalMtx( );
   }

   // how many times should we run the simulation to get the most accurate solution
   uint iterationsCount = ( uint ) ( timeElapsed / 0.1f );
   FastFloat totalTime = Float_0;
   for ( uint i = 0; i < iterationsCount; ++i, totalTime += Float_1e_1 )
   {
      singleSimulationTick( Float_1e_1 );
   }

   // run the remainder of the simulation
   singleSimulationTick( deltaTime - totalTime );

   // commit the matrices
   for ( List< GL2DBody* >::iterator it = m_bodies.begin( ); !it.isEnd( ); ++it )
   {
      GL2DBody* body = *it;
      if ( !body->isMovable() )
      {
         continue;
      }
      Entity* hostEntity = body->getParent( );

      // physical bodies move in world space, so we need to take additional
      // step in order to transform this transformation to the local space of each body
      Matrix invParentEntityGlobalMtx;
      invParentEntityGlobalMtx.setInverse( hostEntity->getParentWorldMtx( ) );

      const Matrix& newWorldMtx = body->getIntegratedTransform( );
      hostEntity->accessLocalMtx( ).setMul( newWorldMtx, invParentEntityGlobalMtx );
   }
}

///////////////////////////////////////////////////////////////////////////////

void GL2DWorld::singleSimulationTick( const FastFloat& deltaTime )
{
   for ( List< GL2DBody* >::iterator it = m_bodies.begin( ); !it.isEnd( ); ++it )
   {
      GL2DBody* body = *it;
      body->integrate( deltaTime, body->getSimulationGlobalMtx() );
   }

   // detect collisions
   m_broadphase->detectCollisions( deltaTime );

   // notify listeners about collisions
   for ( uint i = 0; i < m_broadphase->m_collisionsBuf.m_manifoldsCount; ++i )
   {
      GL2DContactPointManifold& manifold = m_broadphase->m_collisionsBuf.m_manifolds[i];
      for ( List< GL2DListener* >::iterator it = m_listeners.begin( ); !it.isEnd( ); ++it )
      {
         GL2DListener* listener = *it;
         listener->onCollision( manifold );
      }
   }
   
   GL2DCollisionResolver::processCollisionResponse( m_broadphase->m_collisionsBuf );
   
   // commit the simulation matrices
   for ( List< GL2DBody* >::iterator it = m_bodies.begin( ); !it.isEnd( ); ++it )
   {
      GL2DBody* body = *it;
      if ( body->isMovable( ) )
      {
         body->updateSimulationGlobalMtx( );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void GL2DWorld::rayCast( const Ray& ray, GL2DRayCastCollector& outResults ) const
{
   RaycastResult tmpResult;

   for ( List< GL2DBody* >::const_iterator it = m_bodies.begin( ); !it.isEnd( ) && outResults.m_resultsLeft > 0; ++it )
   {
      GL2DBody* body = *it;
      const Matrix& bodyTransform = body->getGlobalMtx( );
      if ( body->rayCast( ray, bodyTransform, tmpResult ) )
      {
         outResults.m_hitsDesc.pushBack( tmpResult );
         // TODO: outResults.m_hitsDesc.back().m_obstacle = body;

         --outResults.m_resultsLeft;
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void GL2DWorld::onAttachedToModel( Model& model )
{
   ModelView::onAttachedToModel( model );

   model.pullStructure( this );
}

///////////////////////////////////////////////////////////////////////////////

void GL2DWorld::onNodeAdded( SceneNode* node )
{
   processBodyAddition( node );
}

///////////////////////////////////////////////////////////////////////////////

void GL2DWorld::onNodeRemoved( SceneNode* node )
{
   if ( node->isA< GL2DBody >( ) )
   {
      GL2DBody* body = static_cast< GL2DBody* >( node );
      bool bodyRemoved = m_broadphase->remove( body );
      if ( bodyRemoved )
      {
         // deinitialize the body, if the simulation is going
         if ( m_isPlaying )
         {
            body->deinitialize( *this );
         }

         List< GL2DBody* >::iterator it = ListUtils::find( m_bodies, body );
         ASSERT( !it.isEnd( ) );
         it.markForRemoval( );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void GL2DWorld::onNodeChanged( SceneNode* node )
{
   processBodyAddition( node );
}

///////////////////////////////////////////////////////////////////////////////

void GL2DWorld::processBodyAddition( SceneNode* node )
{
   if ( !node->isA< GL2DBody >( ) )
   {
      return;
   }

   GL2DBody* body = static_cast< GL2DBody* >( node );
   bool bodyAdded = m_broadphase->add( body );

   if ( bodyAdded )
   {
      // initialize the body, if the simulation is going
      if ( m_isPlaying )
      {
         body->initialize( *this );
      }
      m_bodies.pushBack( body );
   }
   else
   {
      // the body has already been added to broadphase etc - we just need to update its state
      // ( this method's called from onNodeChanged )
      m_broadphase->updateBodyState( body );
   }
}

///////////////////////////////////////////////////////////////////////////////

void GL2DWorld::resetContents( Model& scene )
{
   m_bodies.clear( );
}

///////////////////////////////////////////////////////////////////////////////

void GL2DWorld::attachListener( GL2DListener* listener )
{
   List< GL2DListener* >::iterator it = ListUtils::find( m_listeners, listener );
   if ( it.isEnd( ) )
   {
      m_listeners.pushBack( listener );
   }
}

///////////////////////////////////////////////////////////////////////////////

void GL2DWorld::detachListener( GL2DListener* listener )
{
   List< GL2DListener* >::iterator it = ListUtils::find( m_listeners, listener );
   if ( !it.isEnd( ) )
   {
      it.markForRemoval( );
   }
}

///////////////////////////////////////////////////////////////////////////////
