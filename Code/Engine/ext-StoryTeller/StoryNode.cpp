#include "ext-StoryTeller\StoryNode.h"
#include "ext-StoryTeller\Story.h"
#include "ext-StoryTeller\StoryListener.h"
#include "core\List.h"
#include "core\Assert.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_ABSTRACT_OBJECT( StoryNode );
   PARENT( ReflectionObject );
   PROPERTY( Story*, m_story );
   PROPERTY_EDIT( "Object name", std::string, m_name );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

StoryNode::StoryNode()
   : m_story( NULL )
   , m_name( "New object" )
{
}

///////////////////////////////////////////////////////////////////////////////

void StoryNode::setHostStory( Story* story )
{
   ASSERT_MSG( ( story != NULL && m_story == NULL ) || ( story == NULL && m_story != NULL ) , "This node is already a part of a story" );

   m_story = story;
   onHostStorySet( story );
}

///////////////////////////////////////////////////////////////////////////////

void StoryNode::setName( const char* nodeName )
{
   if ( nodeName )
   {
      m_name = nodeName;
   }
   else
   {
      m_name = "";
   }

   // notify listeners
   if ( m_story )
   {
      for ( List< StoryListener* >::iterator it = m_story->m_listeners.begin(); !it.isEnd(); ++it )
      {
         StoryListener* listener = *it;
         listener->onNodeChanged( this );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void StoryNode::onPropertyChanged( ReflectionProperty& property )
{
   if ( !m_story )
   {
      return;
   }
   // notify listeners
   for ( List< StoryListener* >::iterator it = m_story->m_listeners.begin(); !it.isEnd(); ++it )
   {
      StoryListener* listener = *it;
      listener->onNodeChanged( this );
   }
}

///////////////////////////////////////////////////////////////////////////////
