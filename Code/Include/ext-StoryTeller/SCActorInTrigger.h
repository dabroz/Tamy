/// @file   ext-StoryTeller\SCActorInTrigger.h
/// @brief  a condition that tests whether the specified agent enters the specified trigger
#pragma once

#include "ext-StoryTeller\StoryCondition.h"
#include "core\RuntimeData.h"


///////////////////////////////////////////////////////////////////////////////

class StoryActor;
class SITrigger;

///////////////////////////////////////////////////////////////////////////////


class SCActorInTrigger : public StoryCondition
{
   DECLARE_ALLOCATOR( SCActorInTrigger, AM_DEFAULT );
   DECLARE_CLASS();

private:
   // static data
   StoryActor*                m_actor;
   SITrigger*                 m_trigger;

public:
   /**
    * Constructor.
    *
    * @param numSeconds    how long should the condition hold the flow
    */
   SCActorInTrigger();

   // -------------------------------------------------------------------------
   // StoryCondition implementation
   // -------------------------------------------------------------------------
   bool evaluate( StoryPlayer& player ) const;

};

///////////////////////////////////////////////////////////////////////////////

