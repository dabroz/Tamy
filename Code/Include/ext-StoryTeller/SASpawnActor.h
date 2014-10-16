/// @file   ext-StoryTeller/SASpawnActor.h
/// @brief  this action spawns a story actor
#pragma once

#include "ext-StoryTeller\StoryAction.h"


///////////////////////////////////////////////////////////////////////////////

class StoryActor;
class SAVoidOutput;
class SAAnnotationInput;

///////////////////////////////////////////////////////////////////////////////

/**
 * Story action that spawns an actor.
 */
class SASpawnActor : public StoryAction
{
   DECLARE_ALLOCATOR( SASpawnActor, AM_DEFAULT );
   DECLARE_CLASS();

private:
   // static data
   StoryActor*             m_actor;

   // runtime data
   SAAnnotationInput*      m_spawnPoint;
   SAVoidOutput*           m_out;

public:
   /**
    * Constructor.
    *
    * @param item
    */
   SASpawnActor( StoryActor* actor = NULL );

   /**
   * Copy constructor.
   *
   * @param rhs
   */
   SASpawnActor( const SASpawnActor& rhs );
   ~SASpawnActor();

   // -------------------------------------------------------------------------
   // StoryAction implementation
   // -------------------------------------------------------------------------
   bool execute( StoryPlayer& runner, List< StoryAction* >& outActionsToActivate ) const;

   // -------------------------------------------------------------------------
   // ReflectionObject implementation
   // -------------------------------------------------------------------------
   void onObjectLoaded();
};

///////////////////////////////////////////////////////////////////////////////
