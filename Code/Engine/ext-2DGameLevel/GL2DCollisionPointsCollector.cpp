#include "ext-2DGameLevel\GL2DCollisionPointsCollector.h"
#include "ext-2DGameLevel\GL2DBody.h"
#include "core\Assert.h"


///////////////////////////////////////////////////////////////////////////////

void GL2DVelocityContact::operator=( const GL2DVelocityContact& rhs )
{
   memcpy( this, &rhs, sizeof( GL2DVelocityContact ) );
}

///////////////////////////////////////////////////////////////////////////////

void GL2DVelocityContact::calculateContactInternals( )
{
   calculateLocalVelocity( );
   calculateDesiredDeltaVelocity( );
}

///////////////////////////////////////////////////////////////////////////////

void GL2DVelocityContact::calculateLocalVelocity( )
{
   // Work out the velocity of the contact point.
   m_contactVelocity[0] = m_body[0]->getLinearVelocity( );
   m_contactVelocity[1] = m_body[1]->getLinearVelocity( );
}

///////////////////////////////////////////////////////////////////////////////

void GL2DVelocityContact::calculateDesiredDeltaVelocity( )
{
   if ( !m_body[0]->isMovable( ) )
   {
      m_desiredDeltaVelocity.setZero( );
      m_desiredDeltaVelocityMag = Float_0;
      return;
   }

   // project the velocity along the obstacle's edge
   const FastFloat impactSpeed = m_contactVelocity[0].dot( m_contactNormal );
   m_desiredDeltaVelocity.setMulAdd( m_contactNormal, -impactSpeed, m_contactVelocity[0] );
   m_desiredDeltaVelocity.sub( m_contactVelocity[0] ); // cancel previous velocity

   m_desiredDeltaVelocityMag = m_desiredDeltaVelocity.lengthSq();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

GL2DCollisionPointsCollector::GL2DCollisionPointsCollector( uint maxManifolds )
: m_manifolds( maxManifolds )
, m_manifoldsCount( 0 )
{
   m_manifolds.resizeWithoutInitializing( maxManifolds );
}

///////////////////////////////////////////////////////////////////////////////

void GL2DCollisionPointsCollector::resize( uint maxManifolds )
{
   m_manifolds.resizeWithoutInitializing( maxManifolds );
}

///////////////////////////////////////////////////////////////////////////////

void GL2DCollisionPointsCollector::clear( )
{
   m_manifoldsCount = 0;
}

///////////////////////////////////////////////////////////////////////////////

void GL2DCollisionPointsCollector::add( const Info& info )
{
   // Find the deepest penetration point ( or points ). If there's more than one, use them all
   // to figure out one common point
   FastFloat deepestPenetration[] = { Float_Minus1, Float_Minus1 };
   FastFloat shallowestPenetration = FastFloat::fromFloat( FLT_MAX );
   for ( uint i = 0; i < info.m_contactPointsCount; ++i )
   {
      const IntersectionResult& srcPt = info.m_contactPoints[i];

      if ( info.m_manifoldA &&  srcPt.m_penetrationDepth > Float_0 )
      {
         if ( deepestPenetration[0] < srcPt.m_penetrationDepth )
         {
            deepestPenetration[0] = srcPt.m_penetrationDepth;
         }

         if ( shallowestPenetration > srcPt.m_penetrationDepth )
         {
            shallowestPenetration = srcPt.m_penetrationDepth;
         }
      }

      if ( info.m_manifoldB && srcPt.m_penetrationDepth > Float_0 && deepestPenetration[1] < srcPt.m_penetrationDepth )
      {
         deepestPenetration[1] = srcPt.m_penetrationDepth;
      }
   }
   if ( deepestPenetration[0] > Float_0 )
   {
      deepestPenetration[0] *= info.m_penetrationPart[0];
   }
   if ( deepestPenetration[1] > Float_0 )
   {
      deepestPenetration[1] *= info.m_penetrationPart[1];
   }


   Vector contactPoint = Vector_ZERO;
   Vector contactNormal = Vector_ZERO;
   FastFloat totalWeight = Float_0;
   for ( uint i = 0; i < info.m_contactPointsCount; ++i )
   {
      const IntersectionResult& srcPt = info.m_contactPoints[i];
      totalWeight += Float_1;
      contactPoint.add( srcPt.m_contactPoint );
      contactNormal.add( srcPt.m_contactNormal );
   }

   contactPoint.div( totalWeight );
   contactNormal.div( totalWeight );

   Vector negContactNormal, relPositionA, relPositionB;
   negContactNormal.setMul( contactNormal, Float_Minus1 );

   relPositionA.setSub( contactPoint, info.m_transformA.position( ) );
   relPositionB.setSub( contactPoint, info.m_transformB.position( ) );

   // add penetration contacts
   {
      if ( info.m_manifoldA && deepestPenetration[0] > Float_0 )
      {
         {
            GL2DPenetrationContact& newContact = info.m_manifoldA->m_penetrationContact;
            newContact.m_body[0] = info.m_bodyA;
            newContact.m_body[1] = info.m_bodyB;
            newContact.m_collisionTransform = info.m_transformA;
            newContact.m_contactPoint = contactPoint;
            newContact.m_contactNormal = contactNormal;
            newContact.m_penetrationDepth = deepestPenetration[0];
            newContact.m_relativeContactPosition[0] = relPositionA;
            newContact.m_relativeContactPosition[1] = relPositionB;
            newContact.m_timeRemaining = info.m_timeRemaining;
         }

         {
            GL2DVelocityContact& newContact = info.m_manifoldA->m_velocityContact;
            newContact.m_body[0] = info.m_bodyA;
            newContact.m_body[1] = info.m_bodyB;
            newContact.m_contactNormal = contactNormal;
            newContact.m_relativeContactPosition[0] = relPositionA;
            newContact.m_relativeContactPosition[1] = relPositionB;
            newContact.calculateContactInternals( );
         }
      }
      else
      {
         {
            GL2DPenetrationContact& newContact = info.m_manifoldA->m_penetrationContact;
            newContact.m_body[0] = NULL;
            newContact.m_body[1] = NULL;
            newContact.m_penetrationDepth = Float_0;
         }
         {
            GL2DVelocityContact& newContact = info.m_manifoldA->m_velocityContact;
            newContact.m_body[0] = NULL;
            newContact.m_body[1] = NULL;
         }
      }

      if ( info.m_manifoldB && deepestPenetration[1] > Float_0 )
      {
         {
            GL2DPenetrationContact& newContact = info.m_manifoldB->m_penetrationContact;

            newContact.m_body[0] = info.m_bodyB;
            newContact.m_body[1] = info.m_bodyA;
            newContact.m_collisionTransform = info.m_transformB;
            newContact.m_contactPoint = contactPoint;
            newContact.m_contactNormal = negContactNormal;
            newContact.m_penetrationDepth = deepestPenetration[1];
            newContact.m_relativeContactPosition[0] = relPositionB;
            newContact.m_relativeContactPosition[1] = relPositionA;
            newContact.m_timeRemaining = info.m_timeRemaining;
         }

         {
            GL2DVelocityContact& newContact = info.m_manifoldB->m_velocityContact;
            newContact.m_body[0] = info.m_bodyB;
            newContact.m_body[1] = info.m_bodyA;
            newContact.m_contactNormal = negContactNormal;
            newContact.m_relativeContactPosition[0] = relPositionB;
            newContact.m_relativeContactPosition[1] = relPositionA;
            newContact.calculateContactInternals( );
         }
      }
      else
      {
         {
            GL2DPenetrationContact& newContact = info.m_manifoldB->m_penetrationContact;
            newContact.m_body[0] = NULL;
            newContact.m_body[1] = NULL;
            newContact.m_penetrationDepth = Float_0;
         }
         {
            GL2DVelocityContact& newContact = info.m_manifoldB->m_velocityContact;
            newContact.m_body[0] = NULL;
            newContact.m_body[1] = NULL;
         }
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

GL2DContactPointManifold* GL2DCollisionPointsCollector::getManifold( GL2DBody* body )
{
   if ( !body->isMovable( ) )
   {
      // we generate contacts for moving bodies only
      return NULL;
   }

   GL2DContactPointManifold* manifold = NULL;
   for ( uint i = 0; i < m_manifoldsCount; ++i )
   {
      manifold = &m_manifolds[i];
      if ( manifold->m_body == body )
      {
         return manifold;
      }
   }

   // if we got this far, it means we're allocating a new manifold, 'cause this body hasn't been registered before
   manifold = &m_manifolds[m_manifoldsCount];
   manifold->initialize( body );
   ++m_manifoldsCount;

   ASSERT( manifold );
   return manifold;
}

///////////////////////////////////////////////////////////////////////////////

GL2DContactPointManifold* GL2DCollisionPointsCollector::getManifold( const GL2DBody* body )
{
   if ( !body->isMovable( ) )
   {
      // we generate contacts for moving bodies only
      return NULL;
   }

   GL2DContactPointManifold* manifold = NULL;
   for ( uint i = 0; i < m_manifoldsCount; ++i )
   {
      manifold = &m_manifolds[i];
      if ( manifold->m_body == body )
      {
         return manifold;
      }
   }
   return NULL;
}

///////////////////////////////////////////////////////////////////////////////

void GL2DCollisionPointsCollector::beginIteration( )
{
   m_manifoldProcessedFlag.resize( m_manifoldsCount, false );
   m_manifoldProcessedFlag.broadcastValue( false );
}

///////////////////////////////////////////////////////////////////////////////

void GL2DCollisionPointsCollector::findDeepestPenetrationManifold( GL2DContactPointManifold*& outManifold )
{
   outManifold = NULL;

   // go through all contacts and resolve penetrations
   FastFloat deepestPenetration = Float_0;
   uint manifoldIdx = 0;
   for ( uint i = 0; i < m_manifoldsCount; ++i )
   {
      GL2DContactPointManifold& manifold = m_manifolds[i];

      if ( m_manifoldProcessedFlag[i] )
      {
         // we've already resolved this one in this iteration
         continue;
      }

      if ( manifold.m_penetrationContact.m_penetrationDepth > deepestPenetration )
      {
         deepestPenetration = manifold.m_penetrationContact.m_penetrationDepth;
         outManifold = &manifold;
         manifoldIdx = i;
      }
   }

   if ( outManifold )
   {
      // mark the manifold as resolved in this iteration
      m_manifoldProcessedFlag[manifoldIdx] = true;
   }
}

///////////////////////////////////////////////////////////////////////////////

void GL2DCollisionPointsCollector::findTopDeltaVelocityManifold( GL2DContactPointManifold*& outManifold )
{
   outManifold = NULL;

   // go through all contacts and resolve velocities
   FastFloat topDeltaVelocity = Float_Minus1;
   uint manifoldIdx = 0;
   for ( uint i = 0; i < m_manifoldsCount; ++i )
   {
      GL2DContactPointManifold& manifold = m_manifolds[i];

      if ( m_manifoldProcessedFlag[i] )
      {
         // we've already resolved this one in this iteration
         continue;
      }

      if ( manifold.m_velocityContact.m_desiredDeltaVelocityMag > topDeltaVelocity )
      {
         topDeltaVelocity = manifold.m_velocityContact.m_desiredDeltaVelocityMag;
         outManifold = &manifold;
         manifoldIdx = i;
      }
   }

   if ( outManifold )
   {
      // mark the manifold as resolved in this iteration
      m_manifoldProcessedFlag[manifoldIdx] = true;
   }
}

///////////////////////////////////////////////////////////////////////////////
