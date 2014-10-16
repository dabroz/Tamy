#include "core-Renderer\RenderStateChangeTracker.h"
#include "core-Renderer\RenderStateCommand.h"


///////////////////////////////////////////////////////////////////////////////

RenderStateChangeTracker::RenderStateChangeTracker( Renderer& renderer )
   : m_renderer( renderer )
{
   m_chainTail = NULL;
}

///////////////////////////////////////////////////////////////////////////////

RenderStateChangeTracker::~RenderStateChangeTracker()
{
   for ( RenderStateCommand* command = m_chainTail; command != NULL; )
   {
      RenderStateCommand* currCommand = command;
      command = command->m_prev;

      currCommand->commit( m_renderer );
      delete currCommand;
   }
}
///////////////////////////////////////////////////////////////////////////////

void RenderStateChangeTracker::commit( RenderStateCommand* command )
{
   command->m_prev = m_chainTail;
   m_chainTail = command;
   command->commit( m_renderer );
}

///////////////////////////////////////////////////////////////////////////////
