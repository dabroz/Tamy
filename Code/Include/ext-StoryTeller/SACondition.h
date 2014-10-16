/// @file   ext-StoryTeller/SACondition.h
/// @brief  story action that represents a base condition
#pragma once

#include "ext-StoryTeller\StoryAction.h"


///////////////////////////////////////////////////////////////////////////////

class StoryCondition;
class SAVoidOutput;

///////////////////////////////////////////////////////////////////////////////

/**
 * Story action that represents a base condition.
 */
class SACondition : public StoryAction
{
   DECLARE_ALLOCATOR( SACondition, AM_DEFAULT );
   DECLARE_CLASS();

private:
   StoryCondition*      m_condition;
   SAVoidOutput*        m_outTrue;
   SAVoidOutput*        m_outFalse;

public:
   /**
    * Constructor.
    *
    * @param condition
    */
   SACondition( StoryCondition* condition = NULL );

   /**
    * Copy constructor.
    */
   SACondition( const SACondition& rhs );
   ~SACondition();

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
