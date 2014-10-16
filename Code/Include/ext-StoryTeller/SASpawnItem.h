/// @file   ext-StoryTeller/SASpawnItem.h
/// @brief  story action that spawns an item
#pragma once

#include "ext-StoryTeller\StoryAction.h"


///////////////////////////////////////////////////////////////////////////////

class StoryItem;
class SAVoidOutput;
class SAAnnotationInput;

///////////////////////////////////////////////////////////////////////////////

/**
 * Story action that spawns an item.
 */
class SASpawnItem : public StoryAction
{
   DECLARE_ALLOCATOR( SASpawnItem, AM_DEFAULT );
   DECLARE_CLASS();

private:
   // static data
   StoryItem*              m_item;

   // runtime data
   SAAnnotationInput*      m_spawnPoint;
   SAVoidOutput*           m_out;

public:
   /**
    * Constructor.
    *
    * @param item
    */
   SASpawnItem( StoryItem* item = NULL );

   /**
    * Copy constructor.
    *
    * @param rhs
    */
   SASpawnItem( const SASpawnItem& rhs );
   ~SASpawnItem();

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
