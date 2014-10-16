/// @file   TamyEditor/SBCondition.h
/// @brief  story chapter block that represents a condition
#pragma once

#include "ext-StoryTeller\SACondition.h"
#include "GraphBlock.h"


///////////////////////////////////////////////////////////////////////////////

/**
 * Story chapter block that represents a condition.
 */
class SBCondition : public TGraphWidgetBlock< SACondition, StoryAction >
{
   DECLARE_ALLOCATOR( SBCondition, AM_DEFAULT );
   DECLARE_CLASS()

public:
   /**
    * Default constructor required by the RTTI system.
    */
   SBCondition() : TGraphWidgetBlock< SACondition, StoryAction >() {}

   /**
    * Parametrized constructor required by the generic factory.
    *
    * @param node    represented node
    */
   SBCondition( SACondition& node );
};

///////////////////////////////////////////////////////////////////////////////
