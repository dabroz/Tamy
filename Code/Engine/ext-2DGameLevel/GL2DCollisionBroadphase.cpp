#include "ext-2DGameLevel\GL2DCollisionBroadphase.h"
#include "ext-2DGameLevel\GL2DBody.h"
#include "ext-2DGameLevel\GL2DBody.h"
#include "ext-2DGameLevel\GL2DShape.h"
#include "core\Sphere.h"
#include "core\MatrixUtils.h"
#include "core\Vector.h"
#include "core\ListUtils.h"


///////////////////////////////////////////////////////////////////////////////

GL2DCollisionBroadphase::GL2DCollisionBroadphase( )
: m_collisionsBuf( 128 )
{
}

///////////////////////////////////////////////////////////////////////////////

GL2DCollisionBroadphase::~GL2DCollisionBroadphase( )
{
}

///////////////////////////////////////////////////////////////////////////////

bool GL2DCollisionBroadphase::add( GL2DBody* body )
{
   if ( body->getShape( ) == NULL )
   {
      return false;
   }

   List< GL2DBody* >::iterator it = ListUtils::find( m_bodies, body );
   if ( it.isEnd( ) )
   {
      m_bodies.pushBack( body );

      // add the body to a proper list
      List< GL2DBody* >& subList = body->isMovable() ? m_movingBodies : m_staticBodies;
      List< GL2DBody* >::iterator itMB = ListUtils::find( subList, body );
      if ( itMB.isEnd( ) )
      {
         subList.pushBack( body );
      }

      // resize runtime buffers
      const uint bodiesCount = m_bodies.size( );
      m_collisionsBuf.resize( bodiesCount );

      return true;
   }
   return false;
}

///////////////////////////////////////////////////////////////////////////////

bool GL2DCollisionBroadphase::remove( GL2DBody* body )
{
   List< GL2DBody* >::iterator it = ListUtils::find( m_bodies, body );
   if ( !it.isEnd( ) )
   {
      it.markForRemoval( );

      // remove the body from the proper list
      List< GL2DBody* >::iterator itMB = ListUtils::find( body->isMovable( ) ? m_movingBodies : m_staticBodies, body );
      if ( !itMB.isEnd( ) )
      {
         itMB.markForRemoval( );
      }

      // resize runtime buffers
      const uint bodiesCount = m_bodies.size( );
      m_collisionsBuf.resize( bodiesCount );

      return true;
   }

   return false;
}

///////////////////////////////////////////////////////////////////////////////

void GL2DCollisionBroadphase::updateBodyState( GL2DBody* body )
{
   if ( body->isMovable( ) )
   {
      List< GL2DBody* >::iterator itMB = ListUtils::find( m_staticBodies, body );
      if ( !itMB.isEnd( ) )
      {
         itMB.markForRemoval( );
      }

      if ( ListUtils::find( m_movingBodies, body ).isEnd( ) )
      {
         m_movingBodies.pushBack( body );
      }
   }
   else
   {
      List< GL2DBody* >::iterator itMB = ListUtils::find( m_movingBodies, body );
      if ( !itMB.isEnd( ) )
      {
         itMB.markForRemoval( );
      }

      if ( ListUtils::find( m_staticBodies, body ).isEnd( ) )
      {
         m_staticBodies.pushBack( body );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void GL2DCollisionBroadphase::clear( )
{
   m_bodies.clear( );
   m_movingBodies.clear( );
   m_collisionsBuf.clear( );
}

///////////////////////////////////////////////////////////////////////////////

void GL2DCollisionBroadphase::detectCollisions( const FastFloat& deltaTime )
{
   m_collisionsBuf.clear( );

   // This method will sample sphere's movement with a specific granularity to find the first contact point approximation
   // It works best for small displacements of objects between consecutive frames.
   // When the distances are higher, use the adaptive algorithm

   // detect collisions between static bodies
   for ( List< GL2DBody* >::iterator itA = m_movingBodies.begin( ); !itA.isEnd( ); ++itA )
   {
      GL2DBody* bodyA = *itA;
      for ( List< GL2DBody* >::iterator itB = m_staticBodies.begin( ); !itB.isEnd( ); ++itB )
      {
         GL2DBody* bodyB = *itB;
         sampledCollisionDetection( bodyA, bodyB, deltaTime );
      }
   }

   // detect collisions between moving bodies
   for ( List< GL2DBody* >::iterator itA = m_movingBodies.begin( ); !itA.isEnd( ); ++itA )
   {
      GL2DBody* bodyA = *itA;
      List< GL2DBody* >::iterator itB = itA;
      ++itB;
      for ( ; !itB.isEnd( ); ++itB )
      {
         GL2DBody* bodyB = *itB;

         sampledCollisionDetection( bodyA, bodyB, deltaTime );
      }
   }

   // commit new body positions
}

///////////////////////////////////////////////////////////////////////////////

bool GL2DCollisionBroadphase::sampledCollisionDetection( GL2DBody* bodyA, GL2DBody* bodyB, const FastFloat& deltaTime )
{
   const FastFloat step = Float_1e_2; // accuracy

   // calculate displacements
   GL2DShape* shapeA = bodyA->getShape( );
   GL2DShape* shapeB = bodyB->getShape( );

   Sphere sphereA, sphereB;
   Vector displacementA, displacementB;

   const Matrix& bodyATransform = bodyA->getSimulationGlobalMtx( );
   shapeA->getBoundingSphere( sphereA );
   sphereA.transform( bodyATransform, sphereA );

   const Matrix& bodyBTransform = bodyB->getSimulationGlobalMtx( );
   shapeB->getBoundingSphere( sphereB );
   sphereB.transform( bodyBTransform, sphereB );

   displacementA.setSub( bodyA->getIntegratedTransform( ).position( ), bodyATransform.position( ) );
   displacementB.setSub( bodyB->getIntegratedTransform( ).position( ), bodyBTransform.position( ) );

   // 
   FastFloat displALen = displacementA.length( );
   FastFloat displBLen = displacementB.length( );
   const int stepsCountForA = ( int ) ( displALen / step ).getFloat( );
   const int stepsCountForB = ( int ) ( displBLen / step ).getFloat( );

   int stepsCount = max2( stepsCountForA, stepsCountForB );
   stepsCount = max2( 1, stepsCount );
   const FastFloat fStepsCount = FastFloat::fromInt( stepsCount );

   // adjust the steps displacements according to the new steps count
   Vector stepDispA, stepDispB;
   stepDispA.setDiv( displacementA, fStepsCount );
   stepDispB.setDiv( displacementB, fStepsCount );

   // calculate displacement proportions
   const FastFloat totalPenetrationDist = displALen + displBLen;
   FastFloat penetrationPartA, penetrationPartB;
   if ( totalPenetrationDist == Float_0 )
   {
      penetrationPartA = Float_1;
      penetrationPartB = Float_1;
   }
   else
   {
      penetrationPartA = displALen / totalPenetrationDist;
      penetrationPartB = displBLen / totalPenetrationDist;
   }


   Sphere tmpA( sphereA );
   Sphere tmpB( sphereB );

   FastFloat t = Float_0;
   const FastFloat dt = Float_1 / fStepsCount;
   GL2DCollisionPointsCollector::Info info( bodyA, bodyB );
   for ( int i = 0; i < stepsCount; ++i )
   {
      if ( tmpA.testIntersection( tmpB, info.m_contactPoints[0] ) )
      {
         // check a narrow phase collision
         MatrixUtils::lerp( bodyATransform, bodyA->getIntegratedTransform( ), t, info.m_transformA );
         MatrixUtils::lerp( bodyBTransform, bodyB->getIntegratedTransform( ), t, info.m_transformB );

         info.m_contactPointsCount = shapeA->detectNarrowphaseCollision( shapeB, info.m_transformA, info.m_transformB, info.m_contactPoints );
         if ( info.m_contactPointsCount > 0 )
         {
            info.m_manifoldA = m_collisionsBuf.getManifold( info.m_bodyA );
            info.m_manifoldB = m_collisionsBuf.getManifold( info.m_bodyB );
            info.m_penetrationPart[0] = penetrationPartA;
            info.m_penetrationPart[1] = penetrationPartB;
            info.m_timeRemaining = FastFloat::fromInt( stepsCount - i ) * dt * deltaTime;

            m_collisionsBuf.add( info );

            return true;
         }
      }

      tmpA.origin.add( stepDispA );
      tmpB.origin.add( stepDispB );
      t += dt;
   }

   return false;
}

///////////////////////////////////////////////////////////////////////////////
