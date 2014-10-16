/// @file   ext-StoryTeller/SCWaitForAMoment.h
/// @brief  this condition pauses the flow for the specified period of time
#pragma once

#include "ext-StoryTeller\StoryCondition.h"
#include "core\RuntimeData.h"


///////////////////////////////////////////////////////////////////////////////

/**
 * This condition pauses the flow for the specified period of time.
 */
class SCWaitForAMoment : public StoryCondition
{
   DECLARE_ALLOCATOR( SCWaitForAMoment, AM_DEFAULT );
   DECLARE_CLASS();

private:
   float                      m_numSeconds;

   TRuntimeVar< float >       m_timeElapsed;

public:
   /**
    * Constructor.
    *
    * @param numSeconds    how long should the condition hold the flow
    */
   SCWaitForAMoment( float numSeconds = 5 );

   // -------------------------------------------------------------------------
   // StoryCondition implementation
   // -------------------------------------------------------------------------
   void createLayout( StoryPlayer& player ) const;
   void initialize( StoryPlayer& player ) const;
   void deinitialize( StoryPlayer& player ) const;
   bool evaluate( StoryPlayer& player ) const;
};

///////////////////////////////////////////////////////////////////////////////
