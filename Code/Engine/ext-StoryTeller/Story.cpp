#include "ext-StoryTeller\Story.h"
#include "ext-StoryTeller\StoryChapter.h"
#include "core\ThreadSystem.h"
#include "core\Thread.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_RESOURCE( Story, tst, AM_BINARY );
   PROPERTY( StoryChapter*, m_root );
END_RESOURCE();

///////////////////////////////////////////////////////////////////////////////

Story::Story( const FilePath& path )
   : Resource( path )
   , m_root( NULL )
{
   Thread& currThread = TSingleton< ThreadSystem >::getInstance().getCurrentThread();
   if ( !currThread.m_serializationInProgress )
   {
      m_root = new StoryChapter();
      m_root->setName( "Root" );
      m_root->setHostStory( this );    
   }
}

///////////////////////////////////////////////////////////////////////////////

Story::~Story()
{
   m_listeners.clear();

   m_root->removeReference();
   m_root = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void Story::replaceContents( Resource& rhs )
{
   Story& rhsStory = static_cast< Story& >( rhs );
   m_root = rhsStory.m_root;
   if ( m_root )
   {
      m_root->addReference();
   }
}

///////////////////////////////////////////////////////////////////////////////

void Story::attachListener( StoryListener* listener )
{
   // check for duplicates
   for ( List< StoryListener* >::iterator it = m_listeners.begin(); !it.isEnd(); ++it )
   {
      StoryListener* checkedListener = *it;
      if ( checkedListener == listener )
      {
         // it's a duplicate
         return;
      }
   }

   m_listeners.pushBack( listener );
}

///////////////////////////////////////////////////////////////////////////////

void Story::detachListener( StoryListener* listener )
{
   for ( List< StoryListener* >::iterator it = m_listeners.begin(); !it.isEnd(); ++it )
   {
      StoryListener* checkedListener = *it;
      if ( checkedListener == listener )
      {
         it.markForRemoval();
         return;
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void Story::pullStructure( StoryListener* listener )
{
   m_root->pullStructure( listener );
}

///////////////////////////////////////////////////////////////////////////////
