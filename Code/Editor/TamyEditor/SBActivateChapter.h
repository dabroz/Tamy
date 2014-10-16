/// @file   TamyEditor/SBActivateChapter.h
/// @brief  story chapter block that represents a chapter activation action
#pragma once

#include "ext-StoryTeller\SAActivateChapter.h"
#include "GraphBlock.h"


///////////////////////////////////////////////////////////////////////////////

/**
 * Story chapter block that represents a chapter activation action.
 */
class SBActivateChapter : public TGraphWidgetBlock< SAActivateChapter, StoryAction >
{
   DECLARE_ALLOCATOR( SBActivateChapter, AM_DEFAULT );
   DECLARE_CLASS()

public:
   /**
    * Default constructor required by the RTTI system.
    */
   SBActivateChapter() : TGraphWidgetBlock< SAActivateChapter, StoryAction >() {}

   /**
    * Parametrized constructor required by the generic factory.
    *
    * @param node    represented node
    */
   SBActivateChapter( SAActivateChapter& node );
};

///////////////////////////////////////////////////////////////////////////////
