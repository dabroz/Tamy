/// @file   ext-StoryTeller\StoryAction.h
/// @brief  a base story graph action
#pragma once

#include "core\ReflectionObject.h"
#include "core\GraphBuilderNode.h"
#include "core\List.h"


///////////////////////////////////////////////////////////////////////////////

class StoryPlayer;

///////////////////////////////////////////////////////////////////////////////

/**
 * A base story graph action.
 */
class StoryAction : public GraphBuilderNode< StoryAction >
{
   DECLARE_ALLOCATOR( StoryAction, AM_DEFAULT );
   DECLARE_CLASS();

public:
   /**
    * Constructor.
    *
    * @param name
    */
   StoryAction( const char* name = "" );

   /**
    * Copy constructor.
    */
   StoryAction( const StoryAction& rhs );
   virtual ~StoryAction();

   /**
    * Initializes the runtime variables layout for this action.
    *
    * @param player
    */
   void createLayout( StoryPlayer& player ) const;

   /**
    * Destroys the runtime variables layout for this action.
    *
    * @param player
    */
   void destroyLayout( StoryPlayer& player ) const;

   /**
    * Initializes the story in the context of the specified player.
    *
    * @param player
    */
   virtual void initialize( StoryPlayer& player ) const {}

   /**
    * Deinitializes the story in the context of the specified player.
    *
    * @param player
    */
   virtual void deinitialize( StoryPlayer& player ) const {}

   /**
    * Executes the object's functionality.
    *
    * @param player
    * @param outActionsToActivate   a list of actions to activate after this one
    * @return  'true' if the node should be deactivated, 'false' if we want it to keep on running
    */
   virtual bool execute( StoryPlayer& player, List< StoryAction* >& outActionsToActivate ) const = 0;

protected:
   /**
    * Called so that the node could initialize its layout.
    *
    * @param player
    */
   virtual void onCreateLayout( StoryPlayer& player ) const {}

   /**
    * Called so that the node could deinitialize its layout.
    *
    * @param player
    */
   virtual void onDestroyLayout( StoryPlayer& player ) const {}
};

///////////////////////////////////////////////////////////////////////////////
