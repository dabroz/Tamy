/// @file   TamyEditor/SBChapterStart.h
/// @brief  story chapter block representing the start of a graph
#pragma once

#include "ext-StoryTeller\StoryChapterStart.h"
#include "GraphBlock.h"


///////////////////////////////////////////////////////////////////////////////

/**
 * Story chapter block representing the start of a graph.
 */
class SBChapterStart : public TGraphWidgetBlock< StoryChapterStart, StoryAction >
{
   DECLARE_ALLOCATOR( SBChapterStart, AM_DEFAULT );
   DECLARE_CLASS()

public:
   /**
    * Default constructor required by the RTTI system.
    */
   SBChapterStart() : TGraphWidgetBlock< StoryChapterStart, StoryAction >() {}

   /**
    * Parametrized constructor required by the generic factory.
    *
    * @param node    represented node
    */
   SBChapterStart( StoryChapterStart& node );
};

///////////////////////////////////////////////////////////////////////////////
