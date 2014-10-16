/// @file   TamyEditor/SBSpawnItem.h
/// @brief  story chapter block that represents an item spawning action
#pragma once

#include "ext-StoryTeller\SASpawnItem.h"
#include "GraphBlock.h"


///////////////////////////////////////////////////////////////////////////////

/**
 * Story chapter block that represents an item spawning action.
 */
class SBSpawnItem : public TGraphWidgetBlock< SASpawnItem, StoryAction >
{
   DECLARE_ALLOCATOR( SBSpawnItem, AM_DEFAULT );
   DECLARE_CLASS()

public:
   /**
    * Default constructor required by the RTTI system.
    */
   SBSpawnItem() : TGraphWidgetBlock< SASpawnItem, StoryAction >() {}

   /**
    * Parametrized constructor required by the generic factory.
    *
    * @param node    represented node
    */
   SBSpawnItem( SASpawnItem& node );
};

///////////////////////////////////////////////////////////////////////////////
