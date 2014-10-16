#include "ext-StoryTeller\SACondition.h"
#include "ext-StoryTeller\StoryPlayer.h"
#include "ext-StoryTeller\StoryActionSockets.h"
#include "ext-StoryTeller\StoryCondition.h"
#include "core\ThreadSystem.h"
#include "core\Thread.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( SACondition );
   PARENT( StoryAction );
   PROPERTY_EDIT( "Condition", StoryCondition*, m_condition )
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

SACondition::SACondition( StoryCondition* condition )
   : m_condition( condition )
   , m_outTrue( NULL )
   , m_outFalse( NULL )
{
   ThreadSystem& threadSystem = TSingleton< ThreadSystem >::getInstance();
   if ( threadSystem.getCurrentThread().m_serializationInProgress == false )
   {
      defineInput( new SAVoidInput( "In" ) );

      m_outTrue = new SAVoidOutput( "True" );
      m_outFalse = new SAVoidOutput( "False" );

      std::vector< GBNodeOutput< StoryAction >* > outputs;
      outputs.push_back( m_outTrue );
      outputs.push_back( m_outFalse );

      defineOutputs( outputs );
   }
}

///////////////////////////////////////////////////////////////////////////////

SACondition::SACondition( const SACondition& rhs )
   : StoryAction( rhs )
   , m_condition( rhs.m_condition )
   , m_outTrue( NULL )
   , m_outFalse( NULL )
{
   m_outTrue = static_cast< SAVoidOutput* >( findOutput( "True" ) );
   m_outFalse = static_cast< SAVoidOutput* >( findOutput( "False" ) );
}

///////////////////////////////////////////////////////////////////////////////

SACondition::~SACondition()
{
   if ( m_condition )
   {
      m_condition->removeReference();
      m_condition = NULL;
   }
}

///////////////////////////////////////////////////////////////////////////////

void SACondition::onObjectLoaded()
{
   StoryAction::onObjectLoaded();

   m_outTrue = static_cast< SAVoidOutput* >( findOutput( "True" ) );
   m_outFalse = static_cast< SAVoidOutput* >( findOutput( "False" ) );
}

///////////////////////////////////////////////////////////////////////////////

void SACondition::onCreateLayout( StoryPlayer& player ) const
{
   if ( m_condition )
   {
      m_condition->createLayout( player );
   }
}

///////////////////////////////////////////////////////////////////////////////

void SACondition::initialize( StoryPlayer& player ) const
{
   if ( m_condition )
   {
      m_condition->initialize( player );
   }
}

///////////////////////////////////////////////////////////////////////////////

void SACondition::deinitialize( StoryPlayer& player ) const
{
   if ( m_condition )
   {
      m_condition->deinitialize( player );
   }
}

///////////////////////////////////////////////////////////////////////////////

bool SACondition::execute( StoryPlayer& runner, List< StoryAction* >& outActionsToActivate ) const
{
   if ( m_condition )
   {
      bool conditionResult = m_condition->evaluate( runner );
      if ( conditionResult )
      {
         m_outTrue->collectConnectedNodes( outActionsToActivate );
      }
      else
      {
         m_outFalse->collectConnectedNodes( outActionsToActivate );
      }
   }

   return true;
}

///////////////////////////////////////////////////////////////////////////////
