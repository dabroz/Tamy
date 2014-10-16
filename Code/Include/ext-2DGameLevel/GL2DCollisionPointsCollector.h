/// @file   ext-2DGameLevel\GL2DCollisionPointsCollector.h
/// @brief  collision contact point description
#pragma once

#include "core\MemoryRouter.h"
#include "core\IntersectionResult.h"
#include "core\Matrix.h"
#include "core\Array.h"
#include "ext-2DGameLevel\Defines.h"


///////////////////////////////////////////////////////////////////////////////

class GL2DBody;

///////////////////////////////////////////////////////////////////////////////

struct GL2DPenetrationContact
{
   DECLARE_ALLOCATOR( GL2DPenetrationContact, AM_DEFAULT );

   GL2DBody*           m_body[2];
   Vector                     m_relativeContactPosition[2];

   Vector                     m_contactPoint;
   Vector                     m_contactNormal;
   FastFloat                  m_penetrationDepth;
   Matrix                     m_collisionTransform;
   FastFloat                  m_timeRemaining;
};

///////////////////////////////////////////////////////////////////////////////

struct GL2DVelocityContact
{
   DECLARE_ALLOCATOR( GL2DVelocityContact, AM_DEFAULT );

   GL2DBody*           m_body[2];
   Vector                     m_relativeContactPosition[2];

   Vector                     m_contactNormal;
   Vector                     m_contactVelocity[2];
   Vector                     m_desiredDeltaVelocity;
   FastFloat                  m_desiredDeltaVelocityMag;

   /**
   * Assignment operator.
   *
   * @param rhs
   */
   void operator=( const GL2DVelocityContact& rhs );

   /**
   * Calculates the desired change in velocity for the specified contact point.
   */
   void calculateDesiredDeltaVelocity( );

   /**
   * Calculates additional contact related data that will be used later on by the collision response system.
   */
   void calculateContactInternals( );

private:
   void calculateLocalVelocity( );

};

///////////////////////////////////////////////////////////////////////////////

struct GL2DContactPointManifold
{
   DECLARE_ALLOCATOR( GL2DContactPointManifold, AM_DEFAULT );

public:
   GL2DBody*           m_body;

   GL2DPenetrationContact     m_penetrationContact;
   GL2DVelocityContact        m_velocityContact;

public:
   GL2DContactPointManifold( )
   {
      m_body = NULL;
      m_penetrationContact.m_penetrationDepth = Float_Minus1;
      m_velocityContact.m_desiredDeltaVelocityMag = Float_Minus1;
   }

   /**
   * Initializes the manifold.
   *
   * @param body
   */
   inline void initialize( GL2DBody* body )
   {
      m_body = body;
      m_penetrationContact.m_penetrationDepth = Float_Minus1;
      m_velocityContact.m_desiredDeltaVelocityMag = Float_Minus1;
   }
};

///////////////////////////////////////////////////////////////////////////////

struct GL2DCollisionPointsCollector
{
   DECLARE_ALLOCATOR( GL2DCollisionPointsCollector, AM_DEFAULT );

public:
   struct Info
   {
      DECLARE_ALLOCATOR( Info, AM_DEFAULT );

      GL2DBody*           m_bodyA;
      GL2DBody*           m_bodyB;
      Matrix                     m_transformA;
      Matrix                     m_transformB;
      uint                       m_contactPointsCount;
      IntersectionResult         m_contactPoints[MAX_INTERSECTION_RESULTS];
      GL2DContactPointManifold*  m_manifoldA;
      GL2DContactPointManifold*  m_manifoldB;
      FastFloat                  m_penetrationPart[2];
      FastFloat                  m_timeRemaining;

      Info( GL2DBody* bodyA, GL2DBody* bodyB )
         : m_bodyA( bodyA )
         , m_bodyB( bodyB )
      {}
   };

public:
   Array< GL2DContactPointManifold >           m_manifolds;
   uint                                    m_manifoldsCount;

   Array< bool >                           m_manifoldProcessedFlag;

   /**
   * Constructor.
   *
   * @param maxManifolds
   */
   GL2DCollisionPointsCollector( uint maxManifolds );

   /**
   * Changes the number of possible manifolds.
   *
   * @param maxManifolds
   */
   void resize( uint maxManifolds );

   /**
   * Clears the manifold.
   */
   void clear( );

   /**
   * Adds a set of collision points.
   *
   * @param info
   */
   void add( const Info& info );

   /**
   * Creates or edits a contact manifold for the specified body
   *
   * @param body
   */
   GL2DContactPointManifold* getManifold( GL2DBody* body );

   /**
   * Creates or edits a contact manifold for the specified body
   *
   * @param body
   */
   GL2DContactPointManifold* getManifold( const GL2DBody* body );

   // -------------------------------------------------------------------------
   // Resolution process API
   // -------------------------------------------------------------------------

   /**
   * Initializes resolution process, cleaning up temporary buffers.
   */
   void beginIteration( );

   /**
   * Returns a contact manifold with the deepest penetration.
   *
   * @param outManifold
   */
   void findDeepestPenetrationManifold( GL2DContactPointManifold*& outManifold );

   /**
   * Returns a contact manifold with the highest velocity change value
   *
   * @param outManifold
   */
   void findTopDeltaVelocityManifold( GL2DContactPointManifold*& outManifold );
};

///////////////////////////////////////////////////////////////////////////////
