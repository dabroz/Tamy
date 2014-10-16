/// @file   ext-StoryTeller\BTAInputCondition.h
/// @brief  a conditional node the evaluation of which depends on a specific key being pressed
#pragma once

#include "core-AI\BehTreeCondition.h"


///////////////////////////////////////////////////////////////////////////////

class BTAInputCondition : public BehTreeCondition
{
   DECLARE_ALLOCATOR( BTAInputCondition, AM_DEFAULT );
   DECLARE_CLASS();

private:

public:
   /**
    * Constructor.
    */
   BTAInputCondition();
   ~BTAInputCondition();

   // -------------------------------------------------------------------------
   // BehTreeAction implementation
   // -------------------------------------------------------------------------
   int evaluate( BehaviorTreeRunner& runner ) const;
};

///////////////////////////////////////////////////////////////////////////////
