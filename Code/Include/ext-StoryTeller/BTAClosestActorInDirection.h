/// @file   ext-StoryTeller\BTAClosestActorInDirection.h
/// @brief  grabs an actor that's located closest to this actor in the specified local space direction
#pragma once

#include "core-AI\BehTreeAction.h"
#include "core\RuntimeData.h"


///////////////////////////////////////////////////////////////////////////////

class BehTreeVarReflectionObject;
class StoryActor;

///////////////////////////////////////////////////////////////////////////////

class BTAClosestActorInDirection : public BehTreeAction
{
   DECLARE_ALLOCATOR( BTAClosestActorInDirection, AM_DEFAULT );
   DECLARE_CLASS();

private:
   float                                     m_facingLocalSpace;
   float                                     m_queryConeHalfWidth;
   std::string                               m_actorTag;
   float                                     m_maxRange;
   BehTreeVarReflectionObject*               m_actorFound;

public:
   /**
   * Constructor.
   */
   BTAClosestActorInDirection();
   ~BTAClosestActorInDirection();

   // -------------------------------------------------------------------------
   // BehTreeAction implementation
   // -------------------------------------------------------------------------
   Result execute( BehaviorTreeRunner& runner ) const;
};

///////////////////////////////////////////////////////////////////////////////
