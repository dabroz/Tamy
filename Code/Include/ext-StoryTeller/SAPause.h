/// @file   ext-StoryTeller\SAPause.h
/// @brief  story action that pauses the flow until a condition is met
#pragma once

#include "ext-StoryTeller\StoryAction.h"


///////////////////////////////////////////////////////////////////////////////

class StoryCondition;
class SAVoidOutput;

///////////////////////////////////////////////////////////////////////////////

/**
 * Story action that pauses the flow until a condition is met.
 */
class SAPause : public StoryAction
{
   DECLARE_ALLOCATOR( SAPause, AM_DEFAULT );
   DECLARE_CLASS();

private:
   StoryCondition*      m_condition;
   SAVoidOutput*        m_out;

public:
   /**
    * Constructor.
    *
    * @param condition
    */
   SAPause( StoryCondition* condition = NULL );

   /**
    * Copy constructor.
    */
   SAPause( const SAPause& rhs );
   ~SAPause();

   // -------------------------------------------------------------------------
   // StoryAction implementation
   // -------------------------------------------------------------------------
   void onCreateLayout( StoryPlayer& player ) const;
   void initialize( StoryPlayer& player ) const;
   void deinitialize( StoryPlayer& player ) const;
   bool execute( StoryPlayer& runner, List< StoryAction* >& outActionsToActivate ) const;

   // -------------------------------------------------------------------------
   // ReflectionObject implementation
   // -------------------------------------------------------------------------
   void onObjectLoaded();
};

///////////////////////////////////////////////////////////////////////////////
