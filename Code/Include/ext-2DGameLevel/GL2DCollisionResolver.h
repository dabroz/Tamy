/// @file   ext-2DGameLevel\GL2DCollisionsResolver.h
/// @brief  collisions resolver
#pragma once

#include "core\MemoryRouter.h"
#include "core\Vector.h"
#include "core\Array.h"


///////////////////////////////////////////////////////////////////////////////

struct GL2DCollisionPointsCollector;
struct GL2DContactPointManifold;
typedef Vector GL2DPenetrationSolution;
struct GL2DPenetrationContact;
struct GL2DVelocityContact;

///////////////////////////////////////////////////////////////////////////////

class GL2DCollisionResolver
{
   DECLARE_ALLOCATOR( GL2DCollisionResolver, AM_DEFAULT );

public:
   /**
   * Processes responses to collisions.
   *
   * @param collisionsBuf
   */
   static void processCollisionResponse( GL2DCollisionPointsCollector& collisionsBuf );

private:
   static void resolvePenetrations( GL2DCollisionPointsCollector& collisionsBuf );
   static void resolveSinglePenetration( GL2DPenetrationContact& contactPt, GL2DPenetrationSolution& outPenetrationSolution );
   static void resolveVelocities( GL2DCollisionPointsCollector& collisionsBuf );
   static void resolveSingleVelocity( GL2DVelocityContact& resolvedContactPt, GL2DPenetrationSolution& outVelocitySolution );
};

///////////////////////////////////////////////////////////////////////////////
