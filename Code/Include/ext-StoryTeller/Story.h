/// @file   ext-StoryTeller/Story.h
/// @brief  a story resource
#ifndef _STORY_H
#define _STORY_H

#include "core\Resource.h"
#include "core\Array.h"
#include "core\List.h"


///////////////////////////////////////////////////////////////////////////////

class StoryChapter;
class StoryListener;

///////////////////////////////////////////////////////////////////////////////

class Story : public Resource
{
   DECLARE_ALLOCATOR( Story, AM_DEFAULT );
   DECLARE_RESOURCE();

public:
   // runtime data
   List< StoryListener* >              m_listeners;

private:
   // static data
   StoryChapter*                       m_root;

public:
   /**
    * Constructor.
    *
    * @param path
    */
   Story( const FilePath& path = FilePath() );
   ~Story();

   /**
    * Returns the root node of the story.
    */
   inline StoryChapter& getRoot() { return *m_root; }

   // -------------------------------------------------------------------------
   // Listeners management
   // -------------------------------------------------------------------------
   /**
    * Attaches a new listener.
    *
    * @param listener
    */
   void attachListener( StoryListener* listener );

   /**
    * Detaches a listener.
    *
    * @param listener
    */
   void detachListener( StoryListener* listener );

   /**
    * Pulls the story structure, feeding it to the listener.
    *
    * @param listener
    */
   void pullStructure( StoryListener* listener );

   // -------------------------------------------------------------------------
   // Resource implementation
   // -------------------------------------------------------------------------
   void replaceContents( Resource& rhs );
};

///////////////////////////////////////////////////////////////////////////////

#endif // _STORY_H
