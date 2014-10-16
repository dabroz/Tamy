#include "ext-StoryTeller\SASpawnActor.h"
#include "ext-StoryTeller\StoryPlayer.h"
#include "ext-StoryTeller\StoryActionSockets.h"
#include "ext-StoryTeller\StoryActor.h"
#include "ext-StoryTeller\StoryNodeInstance.h"
#include "ext-StoryTeller\StoryLevelAnnotation.h"
#include "core-AI\BehaviorTreeRunner.h"
#include "core\ThreadSystem.h"
#include "core\Thread.h"
#include "core-MVC\Model.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( SASpawnActor );
   PARENT( StoryAction );
   PROPERTY_EDIT_REFERENCE( "Actor", StoryActor*, m_actor );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

SASpawnActor::SASpawnActor( StoryActor* actor )
   : StoryAction( "SASpawnActor" )
   , m_actor( actor )
   , m_spawnPoint( NULL )
   , m_out( NULL )
{
   registerReferences();

   ThreadSystem& threadSystem = TSingleton< ThreadSystem >::getInstance();
   if ( threadSystem.getCurrentThread().m_serializationInProgress == false )
   {
      std::vector< GBNodeInput< StoryAction >* > inputs;
      m_spawnPoint = new SAAnnotationInput( "SpawnPoint" );

      inputs.push_back( m_spawnPoint );
      inputs.push_back( new SAVoidInput( "In" ) );
      defineInputs( inputs );

      m_out = new SAVoidOutput( "Out" );
      defineOutput( m_out );
   }
}

///////////////////////////////////////////////////////////////////////////////

SASpawnActor::SASpawnActor( const SASpawnActor& rhs )
   : StoryAction( rhs )
   , m_actor( rhs.m_actor )
   , m_spawnPoint( NULL )
   , m_out( NULL )
{
   registerReferences();

   m_spawnPoint = static_cast< SAAnnotationInput* >( findInput( "SpawnPoint" ) );
   m_out = static_cast< SAVoidOutput* >( findOutput( "Out" ) );
}

///////////////////////////////////////////////////////////////////////////////

SASpawnActor::~SASpawnActor()
{
   m_actor = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void SASpawnActor::onObjectLoaded()
{
   StoryAction::onObjectLoaded();

   m_spawnPoint = static_cast< SAAnnotationInput* >( findInput( "SpawnPoint" ) );
   m_out = static_cast< SAVoidOutput* >( findOutput( "Out" ) );
}

///////////////////////////////////////////////////////////////////////////////

bool SASpawnActor::execute( StoryPlayer& runner, List< StoryAction* >& outActionsToActivate ) const
{
   if ( !m_actor )
   {
      return true;
   }

   // instantiate actor's avatar
   Model& gameWorld = runner.gameWorld();

   StoryNodeInstance* spawnedActor = m_actor->instantiate();
   gameWorld.addChild( spawnedActor );

   // use spawn point's position, if one's available
   if ( m_spawnPoint->isConnected() )
   {
      RuntimeDataBuffer& data = runner.data();
      StoryLevelAnnotation* annotation = m_spawnPoint->getValue( data );

      if ( annotation )
      {
         const Matrix& worldTransform = annotation->getGlobalMtx();
         spawnedActor->setLocalMtx( worldTransform );
      }
   }

   // exit 
   m_out->collectConnectedNodes( outActionsToActivate ); 

   return true;
}

///////////////////////////////////////////////////////////////////////////////
