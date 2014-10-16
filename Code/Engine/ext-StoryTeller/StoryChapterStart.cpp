#include "ext-StoryTeller\StoryChapterStart.h"
#include "ext-StoryTeller\StoryPlayer.h"
#include "ext-StoryTeller\StoryActionSockets.h"
#include "core\ThreadSystem.h"
#include "core\Thread.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( StoryChapterStart );
   PARENT( StoryAction );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

StoryChapterStart::StoryChapterStart()
   : m_out( NULL )
{
   ThreadSystem& threadSystem = TSingleton< ThreadSystem >::getInstance();
   if ( threadSystem.getCurrentThread().m_serializationInProgress == false )
   {
      m_out = new SAVoidOutput( "Out" );
      defineOutput( m_out );
   }
}

///////////////////////////////////////////////////////////////////////////////

StoryChapterStart::StoryChapterStart( const StoryChapterStart& rhs )
   : StoryAction( rhs )
   , m_out( NULL )
{
   m_out = static_cast< SAVoidOutput* >( findOutput( "Out" ) );
}

///////////////////////////////////////////////////////////////////////////////

void StoryChapterStart::onObjectLoaded()
{
   StoryAction::onObjectLoaded();

   m_out = static_cast< SAVoidOutput* >( findOutput( "Out" ) );
}

///////////////////////////////////////////////////////////////////////////////

bool StoryChapterStart::execute( StoryPlayer& runner, List< StoryAction* >& outActionsToActivate ) const
{
   m_out->collectConnectedNodes( outActionsToActivate ); 
   return true;
}

///////////////////////////////////////////////////////////////////////////////
