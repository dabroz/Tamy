/// @file   ext-StoryTeller/StoryCondition.h
/// @brief  base condition used in a story graph
#pragma once

#include "core\ReflectionObject.h"


///////////////////////////////////////////////////////////////////////////////

class StoryPlayer;

///////////////////////////////////////////////////////////////////////////////

/**
 * Base condition used in a story graph.
 */
class StoryCondition : public ReflectionObject
{
   DECLARE_ALLOCATOR( StoryCondition, AM_DEFAULT );
   DECLARE_CLASS();

public:
   virtual ~StoryCondition() {}

   /**
    * Initializes the runtime variables layout for this condition.
    *
    * @param player
    */
   virtual void createLayout( StoryPlayer& player ) const {}

   /**
    * Initializes the condition in the context of the specified runner.
    *
    * @param player
    */
   virtual void initialize( StoryPlayer& player ) const {}

   /**
    * Deinitializes the condition in the context of the specified runner.
    *
    * @param player
    */
   virtual void deinitialize( StoryPlayer& player ) const {}

   /**
    * Evaluates the condition.
    *
    * @param player
    * @return condition result
    */
   virtual bool evaluate( StoryPlayer& player ) const = 0;
};

///////////////////////////////////////////////////////////////////////////////
