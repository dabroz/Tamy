/// @file   TamyEditor/StoryChapterLayout.h
/// @brief  story chapter layout
#pragma once

#include "GraphLayout.h"
#include "ext-StoryTeller\StoryChapter.h"


///////////////////////////////////////////////////////////////////////////////

class MaterialNode;
class GraphBlock;

///////////////////////////////////////////////////////////////////////////////

/**
 * Layout for a story chapter graph
 */
class StoryChapterLayout : public TGraphLayout< StoryAction >
{
public:
   /**
    * Constructor.
    *
    * @param graphBuilder
    */
   StoryChapterLayout( StoryChapterGraphBuilder& graphBuilder );

protected:
   // -------------------------------------------------------------------------
   // Resource implementation
   // -------------------------------------------------------------------------
   void initSocketsFactory( SocketsFactory& factory );
};

///////////////////////////////////////////////////////////////////////////////
