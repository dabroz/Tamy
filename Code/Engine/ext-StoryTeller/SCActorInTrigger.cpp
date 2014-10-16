#include "ext-StoryTeller\SCActorInTrigger.h"
#include "ext-StoryTeller\StoryActor.h"
#include "ext-StoryTeller\SITrigger.h"
#include "ext-StoryTeller\StoryNodeInstance.h"
#include "ext-StoryTeller\StoryPlayer.h"
#include "core\LocalList.h"
#include "core\AxisAlignedBox.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( SCActorInTrigger );
   PARENT( StoryCondition );
   PROPERTY_EDIT_REFERENCE( "Actor", StoryActor*, m_actor );
   PROPERTY_EDIT_REFERENCE( "Trigger", SITrigger*, m_trigger );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

SCActorInTrigger::SCActorInTrigger()
   : m_actor( NULL )
   , m_trigger( NULL )
{
   registerReferences();
}

///////////////////////////////////////////////////////////////////////////////

bool SCActorInTrigger::evaluate( StoryPlayer& player ) const
{
   if ( !m_actor || !m_trigger )
   {
      return false;
   }

   LocalList< StoryNodeInstance* > triggerInstances;
   player.collectInstances( m_trigger, triggerInstances );

   LocalList< StoryNodeInstance* > actorInstances;
   player.collectInstances( m_actor, actorInstances );

   // check if any of the actor instances intersects with any of the trigger instances
   for ( LocalList< StoryNodeInstance* >::iterator triggerIt = triggerInstances.begin(); !triggerIt.isEnd(); ++triggerIt )
   {
      StoryNodeInstance* triggerInstance = *triggerIt;
      const AxisAlignedBox& triggerBoundsWorldSpace = triggerInstance->getBoundingVolume();

      for ( LocalList< StoryNodeInstance* >::iterator actorIt = actorInstances.begin(); !actorIt.isEnd(); ++actorIt )
      {
         StoryNodeInstance* actorInstance = *actorIt;
         const AxisAlignedBox& actorBoundsWorldSpace = actorInstance->getBoundingVolume();

         if ( triggerBoundsWorldSpace.testIntersection( actorBoundsWorldSpace ) )
         {
            // we found an intersection
            return true;
         }
      }
   }

   return false;
}

///////////////////////////////////////////////////////////////////////////////
