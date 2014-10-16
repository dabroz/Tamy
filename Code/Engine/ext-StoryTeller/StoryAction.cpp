#include "ext-StoryTeller\StoryAction.h"
#include "ext-StoryTeller\StoryPlayer.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_ABSTRACT_OBJECT( StoryAction );
   PARENT( ReflectionObject );
   PROPERTY( std::string, m_name );
   PROPERTY( InputsMap, m_inputs );
   PROPERTY( OutputsMap, m_outputs );
   PROPERTY( Vector, m_position );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

StoryAction::StoryAction( const char* name )
   : GraphBuilderNode< StoryAction >( name )
{
}

///////////////////////////////////////////////////////////////////////////////

StoryAction::StoryAction( const StoryAction& rhs )
   : GraphBuilderNode< StoryAction >( rhs )
{
}

///////////////////////////////////////////////////////////////////////////////

StoryAction::~StoryAction()
{
}

///////////////////////////////////////////////////////////////////////////////

void StoryAction::createLayout( StoryPlayer& player ) const
{
   for ( OutputsMap::const_iterator it = m_outputs.begin(); it != m_outputs.end(); ++it )
   {
      ( *it )->createLayout( player.data() );
   }

   onCreateLayout( player );
}

///////////////////////////////////////////////////////////////////////////////

void StoryAction::destroyLayout( StoryPlayer& player ) const
{
   onDestroyLayout( player );

   for ( OutputsMap::const_iterator it = m_outputs.begin(); it != m_outputs.end(); ++it )
   {
      ( *it )->destroyLayout( player.data() );
   }
}

///////////////////////////////////////////////////////////////////////////////
