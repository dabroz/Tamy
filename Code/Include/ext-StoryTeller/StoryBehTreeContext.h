/// @file   ext-StoryTeller/StoryBehTreeContext.h
/// @brief  execution context for behavior trees in a running story
#pragma once

#include "core\MemoryRouter.h"
#include "core\Vector.h"


///////////////////////////////////////////////////////////////////////////////

class StoryNode;
class StoryNodeInstance;
class StoryPlayer;

///////////////////////////////////////////////////////////////////////////////

/**
 * Execution context for behavior trees in a running story.
 */
struct StoryBehTreeContext
{
   DECLARE_ALLOCATOR( StoryBehTreeContext, AM_DEFAULT );

   // node that owns the execution context
   StoryNode&              m_owner;

   // instance of the story node
   StoryNodeInstance*      m_ownerInstance;
 
   // instance of a StoryPlayer running this story
   StoryPlayer&            m_player;

   /**
    * Constructor.
    *
    * @param node
    * @param ownerInstance
    * @param player
    */
   StoryBehTreeContext( StoryNode& node, StoryNodeInstance* ownerInstance, StoryPlayer& player );

   /**
    * Copy constructor.
    *
    * @param rhs
    */
   StoryBehTreeContext( const StoryBehTreeContext& rhs );
};

///////////////////////////////////////////////////////////////////////////////
