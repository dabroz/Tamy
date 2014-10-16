#include "ext-StoryTeller\StoryBehTreeContext.h"


///////////////////////////////////////////////////////////////////////////////

StoryBehTreeContext::StoryBehTreeContext( StoryNode& node, StoryNodeInstance* ownerInstance, StoryPlayer& player )
   : m_owner( node )
   , m_ownerInstance( ownerInstance )
   , m_player( player )
{
}

///////////////////////////////////////////////////////////////////////////////

StoryBehTreeContext::StoryBehTreeContext( const StoryBehTreeContext& rhs )
   : m_owner( rhs.m_owner )
   , m_ownerInstance( rhs.m_ownerInstance )
   , m_player( rhs.m_player )
{
}

///////////////////////////////////////////////////////////////////////////////
