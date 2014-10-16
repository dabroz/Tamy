/// @file   ext-StoryTeller/StoryChapterStart.h
/// @brief  the first action in a story graph
#pragma once

#include "ext-StoryTeller\StoryAction.h"


///////////////////////////////////////////////////////////////////////////////

class SAVoidOutput;

///////////////////////////////////////////////////////////////////////////////

class StoryChapterStart : public StoryAction
{
   DECLARE_ALLOCATOR( StoryChapterStart, AM_DEFAULT );
   DECLARE_CLASS();

private:
   SAVoidOutput*           m_out;

public:
   /**
    * Constructor.
    */
   StoryChapterStart();

   /**
    * Copy constructor.
    */
   StoryChapterStart( const StoryChapterStart& rhs );

   // -------------------------------------------------------------------------
   // StoryAction implementation
   // -------------------------------------------------------------------------
   bool execute( StoryPlayer& runner, List< StoryAction* >& outActionsToActivate ) const;

   // -------------------------------------------------------------------------
   // ReflectionObject implementation
   // -------------------------------------------------------------------------
   void onObjectLoaded();
};

///////////////////////////////////////////////////////////////////////////////
