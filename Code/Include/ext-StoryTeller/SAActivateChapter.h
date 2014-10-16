/// @file   ext-StoryTeller/SAActivateChapter.h
/// @brief  story action that activates a chapter
#pragma once

#include "ext-StoryTeller\StoryAction.h"
#include "core\RuntimeData.h"
#include "core\Graph.h"


///////////////////////////////////////////////////////////////////////////////

class StoryChapter;
class SAVoidOutput;
class StoryNodeInstance;

typedef Graph< StoryAction* >     StoryChapterGraph;

///////////////////////////////////////////////////////////////////////////////

/**
 * Story action that activates a chapter.
 */
class SAActivateChapter : public StoryAction
{
   DECLARE_ALLOCATOR( SAActivateChapter, AM_DEFAULT );
   DECLARE_CLASS();

private:
   // static data
   StoryChapter*                                      m_chapter;

   // runtime data
   SAVoidOutput*                                      m_out;

   typedef List< StoryAction* >                       NodesList;

   TRuntimeVar< NodesList* >                          m_nodesToActivate;
   TRuntimeVar< NodesList* >                          m_activeNodes;
   TRuntimeVar< StoryChapterGraph* >                  m_graph;
   TRuntimeVar< StoryNodeInstance* >                  m_spawnedEnvironment;

public:
   /**
    * Constructor.
    *
    * @param chapter
    */
   SAActivateChapter( StoryChapter* chapter = NULL );

   /**
    * Copy constructor.
    */
   SAActivateChapter( const SAActivateChapter& rhs );

   // -------------------------------------------------------------------------
   // StoryAction implementation
   // -------------------------------------------------------------------------
   void onCreateLayout( StoryPlayer& runner ) const;
   void onDestroyLayout( StoryPlayer& runner ) const;
   void initialize( StoryPlayer& runner ) const;
   void deinitialize( StoryPlayer& runner ) const;
   bool execute( StoryPlayer& runner, List< StoryAction* >& outActionsToActivate ) const;

   // -------------------------------------------------------------------------
   // ReflectionObject implementation
   // -------------------------------------------------------------------------
   void onObjectLoaded();
};

///////////////////////////////////////////////////////////////////////////////
