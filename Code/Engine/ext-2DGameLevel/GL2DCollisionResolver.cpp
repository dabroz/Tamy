#include "ext-2DGameLevel\GL2DCollisionResolver.h"
#include "ext-2DGameLevel\GL2DCollisionPointsCollector.h"
#include "ext-2DGameLevel\GL2DBody.h"


///////////////////////////////////////////////////////////////////////////////

void GL2DCollisionResolver::processCollisionResponse( GL2DCollisionPointsCollector& collisionsBuf )
{
   resolveVelocities( collisionsBuf );
   resolvePenetrations( collisionsBuf );
}

///////////////////////////////////////////////////////////////////////////////

void GL2DCollisionResolver::resolvePenetrations( GL2DCollisionPointsCollector& collisionsBuf )
{
   // We're gonna use a naive approach at first, simply iterating through every
   // body we have and resolving the deepest contacts.
   // If we wanted to take advantage of threads here, we could for instance
   // identify islands of contacts first and then have each processed on a separate thread.
   const uint manifoldsCount = collisionsBuf.m_manifoldsCount;

   // keep running until all penetrations have been resolved
   GL2DContactPointManifold* resolvedManifold = NULL;
   GL2DPenetrationSolution penetrationSolution;
   const FastFloat sign[] = { Float_Minus1, Float_1 };

   collisionsBuf.beginIteration( );
   for ( uint manifoldIdx = 0; manifoldIdx < manifoldsCount; ++manifoldIdx )
   {
      // go through all contacts and resolve penetrations
      collisionsBuf.findDeepestPenetrationManifold( resolvedManifold );
      if ( resolvedManifold == NULL )
      {
         // no more collisions to resolve
         return;
      }

      GL2DPenetrationContact& resolvedContactPt = resolvedManifold->m_penetrationContact;
      resolveSinglePenetration( resolvedContactPt, penetrationSolution );

      // update the time the resolved body still can use to move
      resolvedContactPt.m_body[0]->integrate( resolvedContactPt.m_timeRemaining, resolvedContactPt.m_collisionTransform );


      // resolving the penetration may have affected other penetrations ( body was moved, possibly into another one ),
      // so we need to relax the remaining penetrations
      for ( uint i = 0; i < manifoldsCount; ++i )
      {
         GL2DPenetrationContact& contactPt = collisionsBuf.m_manifolds[i].m_penetrationContact;

         if ( contactPt.m_body[1] == resolvedContactPt.m_body[0] && contactPt.m_body[0] == resolvedContactPt.m_body[1] )
         {
            // that's a counterpart to the resolved contact point - don't update it
            continue;
         }

         for ( byte a = 0; a < 2; ++a )
         {
            if ( contactPt.m_body[a] == resolvedContactPt.m_body[0] )
            {
               const FastFloat val = sign[a] * penetrationSolution.dot( contactPt.m_contactNormal );
               contactPt.m_penetrationDepth += val;
            }
         }
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void GL2DCollisionResolver::resolveSinglePenetration( GL2DPenetrationContact& contactPt, GL2DPenetrationSolution& outPenetrationSolution )
{
   // move the body away
   Matrix modifiedIntegratedTransform;
   modifiedIntegratedTransform = contactPt.m_collisionTransform;

   outPenetrationSolution.setMul( contactPt.m_contactNormal, contactPt.m_penetrationDepth );

   Vector newPos;
   newPos.setAdd( outPenetrationSolution, contactPt.m_collisionTransform.position( ) );
   modifiedIntegratedTransform.setPosition<3>( newPos );
   contactPt.m_collisionTransform = modifiedIntegratedTransform;
}

///////////////////////////////////////////////////////////////////////////////

void GL2DCollisionResolver::resolveVelocities( GL2DCollisionPointsCollector& collisionsBuf )
{
   // reset iteration indices
   const uint manifoldsCount = collisionsBuf.m_manifoldsCount;

   // keep running until all penetrations have been resolved
   GL2DContactPointManifold* resolvedManifold = NULL;
   GL2DPenetrationSolution velocitySolution;
   const FastFloat sign[] = { Float_1, Float_Minus1 };

   collisionsBuf.beginIteration( );
   for ( uint manifoldIdx = 0; manifoldIdx < manifoldsCount; ++manifoldIdx )
   {
      // go through all contacts and resolve velocities
      collisionsBuf.findTopDeltaVelocityManifold( resolvedManifold );
      if ( resolvedManifold == NULL )
      {
         // no more collisions to resolve
         return;
      }

      GL2DVelocityContact& resolvedContactPt = resolvedManifold->m_velocityContact;
      resolveSingleVelocity( resolvedContactPt, velocitySolution );

      // resolving the velocities may have affected other velocities ( body was moved, possibly into another one ),
      // so we need to relax the remaining ones
      for ( uint i = 0; i < manifoldsCount; ++i )
      {
         GL2DVelocityContact& contactPt = collisionsBuf.m_manifolds[i].m_velocityContact;

         if ( contactPt.m_body[1] == resolvedContactPt.m_body[0] && contactPt.m_body[0] == resolvedContactPt.m_body[1] )
         {
            // that's a counterpart to the resolved contact point - don't update it
            continue;
         }

         for ( byte a = 0; a < 2; ++a )
         {
            if ( contactPt.m_body[a] == resolvedContactPt.m_body[0] )
            {
               contactPt.m_contactVelocity[a].setMulAdd( velocitySolution, sign[a], contactPt.m_contactVelocity[a] );
               contactPt.calculateDesiredDeltaVelocity( );
            }
         }
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void GL2DCollisionResolver::resolveSingleVelocity( GL2DVelocityContact& contactPt, GL2DPenetrationSolution& outVelocitySolution )
{
   outVelocitySolution = contactPt.m_desiredDeltaVelocity;

   // Apply the changes
   Vector newLinearVelocity, newAngularVelocity;
   newLinearVelocity.setAdd( contactPt.m_body[0]->getLinearVelocity( ), outVelocitySolution );
   contactPt.m_body[0]->setLinearVelocity( newLinearVelocity );
}

///////////////////////////////////////////////////////////////////////////////
