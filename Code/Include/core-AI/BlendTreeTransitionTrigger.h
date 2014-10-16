/// @file   core-AI/BlendTreeTransitionTrigger.h
/// @brief  a blend tree transition trigger interface
#pragma once

#include "core\ReflectionObject.h"


/////////////////////////////////////////////////////////////////////////////////////////

class BlendTreeNode;
class BlendTreePlayer;

/////////////////////////////////////////////////////////////////////////////////////////

/**
 * A transition trigger is responsible for checking when a transition can be activated.
 */
class BlendTreeTransitionTrigger : public ReflectionObject
{
   DECLARE_ALLOCATOR( BlendTreeTransitionTrigger, AM_DEFAULT );
   DECLARE_CLASS();

public:
   /**
    * Constructor.
    *
    * @param name
    */
   BlendTreeTransitionTrigger();
   virtual ~BlendTreeTransitionTrigger();

   /**
    * Initializes the trigger's runtime data layout.
    *
    * @param player
    */
   virtual void initializeLayout( BlendTreePlayer* player ) const {}

   /**
    * Deinitializes the trigger's runtime data layout.
    *
    * @param player
    */
   virtual void deinitializeLayout( BlendTreePlayer* player ) const {}

   /**
    * Tests the condition.
    *
    * @param player
    *
    * @return              'true' if the transition can be activated, 'false' otherwise
    */
   virtual bool testActivation( BlendTreePlayer* player ) const { return false; }
};

/////////////////////////////////////////////////////////////////////////////////////////
