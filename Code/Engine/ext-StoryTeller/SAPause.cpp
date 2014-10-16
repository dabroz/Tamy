#include "ext-StoryTeller\SAPause.h"
#include "ext-StoryTeller\StoryPlayer.h"
#include "ext-StoryTeller\StoryActionSockets.h"
#include "ext-StoryTeller\StoryCondition.h"
#include "core\ThreadSystem.h"
#include "core\Thread.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( SAPause );
   PARENT( StoryAction );
   PROPERTY_EDIT( "Condition", StoryCondition*, m_condition )
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

SAPause::SAPause( StoryCondition* condition )
   : m_condition( condition )
   , m_out( NULL )
{
   if ( !IS_BEING_SERIALIZED() )
   {
      defineInput( new SAVoidInput( "In" ) );

      m_out = new SAVoidOutput( "Out" );
      defineOutput( m_out );
   }
}

///////////////////////////////////////////////////////////////////////////////

SAPause::SAPause( const SAPause& rhs )
   : StoryAction( rhs )
   , m_condition( rhs.m_condition )
   , m_out( NULL )
{
   m_out = static_cast< SAVoidOutput* >( findOutput( "Out" ) );
}

///////////////////////////////////////////////////////////////////////////////

SAPause::~SAPause()
{
   if ( m_condition )
   {
      m_condition->removeReference();
      m_condition = NULL;
   }
}

///////////////////////////////////////////////////////////////////////////////

void SAPause::onObjectLoaded()
{
   StoryAction::onObjectLoaded();

   m_out = static_cast< SAVoidOutput* >( findOutput( "Out" ) );
}

///////////////////////////////////////////////////////////////////////////////

void SAPause::onCreateLayout( StoryPlayer& player ) const
{
   if ( m_condition )
   {
      m_condition->createLayout( player );
   }
}

///////////////////////////////////////////////////////////////////////////////

void SAPause::initialize( StoryPlayer& player ) const
{
   if ( m_condition )
   {
      m_condition->initialize( player );
   }
}

///////////////////////////////////////////////////////////////////////////////

void SAPause::deinitialize( StoryPlayer& player ) const
{
   if ( m_condition )
   {
      m_condition->deinitialize( player );
   }
}

///////////////////////////////////////////////////////////////////////////////

bool SAPause::execute( StoryPlayer& runner, List< StoryAction* >& outActionsToActivate ) const
{
   if ( !m_condition )
   {
      // no condition - exit immediately
      m_out->collectConnectedNodes( outActionsToActivate );
      return true;
   }
      
   bool conditionResult = m_condition->evaluate( runner );
   if ( conditionResult )
   {
      // condition evaluated positively - let the flow through
      m_out->collectConnectedNodes( outActionsToActivate );
      return true;
   }
   else
   {
      return false;
   }
}

///////////////////////////////////////////////////////////////////////////////
