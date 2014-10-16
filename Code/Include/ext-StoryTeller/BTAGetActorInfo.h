/// @file   ext-StoryTeller\BTAGetActorInfo.h
/// @brief  gathers information about the specified actor - its world transform, velocity etc.
#pragma once

#include "core-AI\BehTreeAction.h"
#include "core\RuntimeData.h"


///////////////////////////////////////////////////////////////////////////////

class BehTreeVarReflectionObject;
class BehTreeVarVector;
class StoryActor;

///////////////////////////////////////////////////////////////////////////////

class BTAGetActorInfo : public BehTreeAction
{
   DECLARE_ALLOCATOR( BTAGetActorInfo, AM_DEFAULT );
   DECLARE_CLASS();

private:
   BehTreeVarReflectionObject*               m_actor;
   BehTreeVarVector*                         m_worldPos;
   BehTreeVarVector*                         m_velocity;

public:
   /**
    * Constructor.
    */
   BTAGetActorInfo();
   ~BTAGetActorInfo();

   // -------------------------------------------------------------------------
   // BehTreeAction implementation
   // -------------------------------------------------------------------------
   Result execute( BehaviorTreeRunner& runner ) const;
};

///////////////////////////////////////////////////////////////////////////////
