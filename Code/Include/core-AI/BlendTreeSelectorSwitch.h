/// @file   core-AI/BlendTreeSelectorSwitch.h
/// @brief  classes that tell a BlendTreeSelector which node to activate
#pragma once

#include "core\ReflectionObject.h"


///////////////////////////////////////////////////////////////////////////////

class BlendTreePlayer;

///////////////////////////////////////////////////////////////////////////////

/**
 * A base class of a hierarchy of classes that tell a BlendTreeSelector which node to activate.
 */
class BlendTreeSelectorSwitch : public ReflectionObject
{
   DECLARE_ALLOCATOR( BlendTreeSelectorSwitch, AM_DEFAULT );
   DECLARE_CLASS();

public:
   virtual ~BlendTreeSelectorSwitch() {}

   /**
    * Initializes switch's runtime context.
    *
    * @param player
    */
   virtual void initializeLayout( BlendTreePlayer* player ) {}

   /**
    * Deinitializes switch's runtime context.
    *
    * @param player
    */
   virtual void deinitializeLayout( BlendTreePlayer* player ) {}

   /**
    * Returns the index of a node the selector should switch to.
    *
    * @param player
    */
   virtual int getNodeIdx( BlendTreePlayer* player ) const { return -1; }
};

///////////////////////////////////////////////////////////////////////////////
