#include "DebugEntitiesManager.h"
#include "core-Renderer.h"
#include "core.h"
#include "core-MVC\Model.h"
#include "core-AI\SkeletonComponent.h"
#include "ext-StoryTeller\StoryLevelAnnotation.h"

// debug representations
#include "DebugGrid.h"
#include "DRDirectionalLight.h"
#include "DRPointLight.h"
#include "DRSpotLight.h"
#include "DRGeometry.h"
#include "DRSkeletonEntity.h"
#include "DRStoryLevelAnnotation.h"



///////////////////////////////////////////////////////////////////////////////

BEGIN_ENUM( DebugFeature );
   ENUM_VAL( DF_Grid );
   ENUM_VAL( DF_BoundingBoxes );
   ENUM_VAL( DF_Shapes );
END_ENUM();

///////////////////////////////////////////////////////////////////////////////

DebugEntitiesManager::DebugEntitiesManager( Model& debugScene )
   : m_debugScene( debugScene )
   , m_debugGrid( new DebugGrid() )
   , m_debugViewEnabled( true )
{
   m_debugGrid->addReference();
   m_debugScene.addChild( m_debugGrid );

   // set debug features
   m_debugFeatureEnabled = new bool[MAX_DEBUG_FEATURES];
   memset( m_debugFeatureEnabled, 0, sizeof( bool ) * MAX_DEBUG_FEATURES );

   m_debugFeatureEnabled[DF_Grid] = true;
   m_debugFeatureEnabled[DF_Shapes] = true;

   // register debug representations
   m_representationsFactory = new GenericFactory< SceneNode, DebugGeometry >();
   m_representationsFactory->associate< DirectionalLight, DRDirectionalLight >();
   m_representationsFactory->associate< PointLight, DRPointLight >();
   m_representationsFactory->associate< SpotLight, DRSpotLight >();
   m_representationsFactory->associate< SkeletonComponent, DRSkeletonEntity >();
   m_representationsFactory->associate< StoryLevelAnnotation, DRStoryLevelAnnotation >();
   m_representationsFactory->associateAbstract< Entity, DRGeometry >();
}

///////////////////////////////////////////////////////////////////////////////

DebugEntitiesManager::~DebugEntitiesManager()
{
   m_debugScene.clear();
   m_representations.clear();
   m_representedNodes.clear();

   if ( m_debugGrid )
   {
      m_debugGrid->removeReference();
      m_debugGrid = NULL;
   }

   delete [] m_debugFeatureEnabled;
   m_debugFeatureEnabled = NULL;

   delete m_representationsFactory;
   m_representationsFactory = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void DebugEntitiesManager::onAttachedToModel( Model& model )
{
   ModelView::onAttachedToModel( model );

   model.pullStructure( this );
}

///////////////////////////////////////////////////////////////////////////////

bool DebugEntitiesManager::toggleDebugDisplay( DebugFeature flag )
{
   m_debugFeatureEnabled[flag] = !m_debugFeatureEnabled[flag];

   updateFeaturesVisibility();

   return m_debugFeatureEnabled[flag];
}

///////////////////////////////////////////////////////////////////////////////

void DebugEntitiesManager::setEnabled( bool enable )
{
   m_debugViewEnabled = enable;
   updateFeaturesVisibility();
}

///////////////////////////////////////////////////////////////////////////////

void DebugEntitiesManager::disableAll()
{
   memset( m_debugFeatureEnabled, 0, sizeof( bool ) * MAX_DEBUG_FEATURES );
   updateFeaturesVisibility();
}

///////////////////////////////////////////////////////////////////////////////

void DebugEntitiesManager::updateFeaturesVisibility()
{
   uint count = m_representations.size();
   for ( uint i = 0; i < count; ++i )
   {
      DebugGeometry* representation = m_representations[i];

      for ( uint featIdx = 0; featIdx < MAX_DEBUG_FEATURES; ++featIdx )
      {
         applyFeature( representation, (DebugFeature)featIdx );
      }
   }

   m_debugGrid->show( m_debugViewEnabled && m_debugFeatureEnabled[DF_Grid] );
}

///////////////////////////////////////////////////////////////////////////////

void DebugEntitiesManager::update( float timeElapsed )
{
   if ( !m_debugViewEnabled )
   {
      return;
   }
   uint count = m_representations.size();
   for ( uint i = 0; i < count; ++i )
   {
      const Matrix& globalMtx = m_representedNodes[i]->getGlobalMtx();
      m_representations[i]->setLocalMtx( globalMtx );
      m_representations[i]->updateDebugData( timeElapsed );
   }
}

///////////////////////////////////////////////////////////////////////////////

void DebugEntitiesManager::onNodeAdded( SceneNode* node )
{
   // check uniqueness
   uint count = m_representedNodes.size();
   for ( uint i = 0; i < count; ++i )
   {
      if ( m_representedNodes[i] == node )
      {
         // already got it
         return;
      }
   }

   DebugGeometry* representation = m_representationsFactory->create( *node );
   if ( representation )
   {
      m_debugScene.addChild( representation );

      m_representedNodes.push_back( node );
      m_representations.push_back( representation );

      // apply all debug features to it
      for ( uint i = 0; i < MAX_DEBUG_FEATURES; ++i )
      {
         applyFeature( representation, (DebugFeature)i );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void DebugEntitiesManager::onNodeRemoved( SceneNode* node )
{
   uint count = m_representedNodes.size();
   for ( uint i = 0; i < count; ++i )
   {
      if ( m_representedNodes[i] == node )
      {
         // found it
         m_representedNodes.erase( m_representedNodes.begin() + i );

         DebugGeometry* representation = m_representations[i];
         representation->remove();
         
         m_representations.erase( m_representations.begin() + i );

         return;
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void DebugEntitiesManager::onNodeChanged( SceneNode* node )
{
   // nothing to do here
}

///////////////////////////////////////////////////////////////////////////////

void DebugEntitiesManager::resetContents( Model& model )
{
   m_debugScene.clear();
   m_representations.clear();
   m_representedNodes.clear();
}

///////////////////////////////////////////////////////////////////////////////

void DebugEntitiesManager::applyFeature( DebugGeometry* representation, DebugFeature feature )
{
   bool enable = m_debugViewEnabled && m_debugFeatureEnabled[feature];

   switch( feature )
   {
   case DF_BoundingBoxes:
      {
         representation->enableBoundingBox( enable );
         break;
      }

   case DF_Shapes:
      {
         representation->enableDebugShape( enable );
         break;
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

DebugGeometry* DebugEntitiesManager::getRepresentation( SceneNode* node ) const
{
   uint count = m_representedNodes.size();
   for ( uint i = 0; i < count; ++i )
   {
      if ( m_representedNodes[i] == node )
      {
         return m_representations[i];
      }
   }

   return NULL;
}

///////////////////////////////////////////////////////////////////////////////
