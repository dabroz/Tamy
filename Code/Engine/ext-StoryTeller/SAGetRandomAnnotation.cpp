#include "ext-StoryTeller\SAGetRandomAnnotation.h"
#include "ext-StoryTeller\StoryPlayer.h"
#include "ext-StoryTeller\StoryActionSockets.h"
#include "ext-StoryTeller\StoryActor.h"
#include "ext-StoryTeller\StoryNodeInstance.h"
#include "ext-StoryTeller\StoryLevelAnnotation.h"
#include "core-AI\BehaviorTreeRunner.h"
#include "core\ThreadSystem.h"
#include "core\Thread.h"
#include "core-MVC\Model.h"
#include "core-MVC\EntityUtils.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( SAGetRandomAnnotation );
   PARENT( StoryAction );
   PROPERTY_EDIT( "Spawn point", bool, m_spawnPoint );
   PROPERTY_EDIT( "Tag", std::string, m_annotationName );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

SAGetRandomAnnotation::SAGetRandomAnnotation()
   : StoryAction( "SAGetRandomAnnotation" )
   , m_out( NULL )
   , m_spawnPoint( false )
{
   ThreadSystem& threadSystem = TSingleton< ThreadSystem >::getInstance();
   if ( threadSystem.getCurrentThread().m_serializationInProgress == false )
   {
      defineInput( new SAVoidInput( "In" ) );

      m_out = new SAAnnotationOutput( "Out" );
      defineOutput( m_out );
   }
}

///////////////////////////////////////////////////////////////////////////////

SAGetRandomAnnotation::SAGetRandomAnnotation( const SAGetRandomAnnotation& rhs )
   : StoryAction( rhs )
   , m_out( NULL )
   , m_spawnPoint( rhs.m_spawnPoint )
   , m_annotationName( rhs.m_annotationName )
{
   m_out = static_cast< SAAnnotationOutput* >( findOutput( "Out" ) );
}

///////////////////////////////////////////////////////////////////////////////

SAGetRandomAnnotation::~SAGetRandomAnnotation()
{
}

///////////////////////////////////////////////////////////////////////////////

void SAGetRandomAnnotation::onObjectLoaded()
{
   m_out = static_cast< SAAnnotationOutput* >( findOutput( "Out" ) );
}

///////////////////////////////////////////////////////////////////////////////

bool SAGetRandomAnnotation::execute( StoryPlayer& runner, List< StoryAction* >& outActionsToActivate ) const
{
   // collect all annotations
   Model& world = runner.gameWorld();
   Array< StoryLevelAnnotation* > annotations;
   EntityUtils::collectNodesByType< StoryLevelAnnotation >( world.getRoot(), annotations );

   // filter out unnecessary annotations
   const uint count = annotations.size();
   Array< StoryLevelAnnotation* > matchingAnnotations( count );
   for ( uint i = 0; i < count; ++i )
   {
      StoryLevelAnnotation* annotation = annotations[i];
      if ( annotation->m_spawnPoint != m_spawnPoint )
      {
         continue;
      }

      if ( m_annotationName.length() > 0 )
      {
         // the name of the annotation was specified - do a partial match and see if it fits
         const std::string spawnPointName = annotation->getParent()->getSceneNodeName();
         if ( spawnPointName.find( m_annotationName ) == std::string::npos )
         {
            continue;
         }
      }

      // we have a match
      matchingAnnotations.push_back( annotation );
   }

   // select an annotation at random
   if ( !matchingAnnotations.empty() )
   {
      const uint matchingAnnotationsCount = matchingAnnotations.size();
      const uint idx = rand() % matchingAnnotationsCount;

      RuntimeDataBuffer& data = runner.data();
      m_out->setValue( data, matchingAnnotations[idx] );
   }
   else
   {
      RuntimeDataBuffer& data = runner.data();
      m_out->setValue( data, NULL );
   }

   // exit
   m_out->collectConnectedNodes( outActionsToActivate );

   return true;
}

///////////////////////////////////////////////////////////////////////////////
