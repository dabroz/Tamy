/// @file   TamyEditor/SBPause.h
/// @brief  story chapter block that represents a pause
#pragma once

#include "ext-StoryTeller\SAPause.h"
#include "GraphBlock.h"


///////////////////////////////////////////////////////////////////////////////

/**
 * Story chapter block that represents a pause.
 */
class SBPause : public TGraphWidgetBlock< SAPause, StoryAction >
{
   DECLARE_ALLOCATOR( SBPause, AM_DEFAULT );
   DECLARE_CLASS()

public:
   /**
    * Default constructor required by the RTTI system.
    */
   SBPause() : TGraphWidgetBlock< SAPause, StoryAction >() {}

   /**
    * Parametrized constructor required by the generic factory.
    *
    * @param node    represented node
    */
   SBPause( SAPause& node );
};

///////////////////////////////////////////////////////////////////////////////
