#include "ext-StoryTeller\SASpawnItem.h"
#include "ext-StoryTeller\StoryPlayer.h"
#include "ext-StoryTeller\StoryActionSockets.h"
#include "ext-StoryTeller\StoryItem.h"
#include "ext-StoryTeller\StoryNodeInstance.h"
#include "ext-StoryTeller\StoryLevelAnnotation.h"
#include "core\ThreadSystem.h"
#include "core\Thread.h"
#include "core-MVC\Model.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( SASpawnItem );
   PARENT( StoryAction );
   PROPERTY_EDIT_REFERENCE( "Item", StoryItem*, m_item );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

SASpawnItem::SASpawnItem( StoryItem* item )
   : StoryAction( "SASpawnItem" )
   , m_item( item )
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

SASpawnItem::SASpawnItem( const SASpawnItem& rhs )
   : StoryAction( rhs )
   , m_item( rhs.m_item )
   , m_out( NULL )
{
   registerReferences();

   m_spawnPoint = static_cast< SAAnnotationInput* >( findInput( "SpawnPoint" ) );
   m_out = static_cast< SAVoidOutput* >( findOutput( "Out" ) );
}

///////////////////////////////////////////////////////////////////////////////

SASpawnItem::~SASpawnItem()
{
   m_item = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void SASpawnItem::onObjectLoaded()
{
   StoryAction::onObjectLoaded();

   m_spawnPoint = static_cast< SAAnnotationInput* >( findInput( "SpawnPoint" ) );
   m_out = static_cast< SAVoidOutput* >( findOutput( "Out" ) );
}

///////////////////////////////////////////////////////////////////////////////

bool SASpawnItem::execute( StoryPlayer& runner, List< StoryAction* >& outActionsToActivate ) const
{
   if ( !m_item )
   {
      return true;
   }

   Model& gameWorld = runner.gameWorld();
      
   Entity* spawnedItem = m_item->instantiate();
   gameWorld.addChild( spawnedItem );

   // use spawn point's position, if one's available
   if ( m_spawnPoint->isConnected() )
   {
      RuntimeDataBuffer& data = runner.data();
      StoryLevelAnnotation* annotation = m_spawnPoint->getValue( data );

      if ( annotation )
      {
         const Matrix& worldTransform = annotation->getGlobalMtx();
         spawnedItem->setLocalMtx( worldTransform );
      }
   }

   // exit 
   m_out->collectConnectedNodes( outActionsToActivate ); 

   return true;
}

///////////////////////////////////////////////////////////////////////////////
