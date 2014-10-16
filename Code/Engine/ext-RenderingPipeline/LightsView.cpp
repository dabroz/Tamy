#include "ext-RenderingPipeline\LightsView.h"
#include "core-Renderer\Light.h"
#include "core-MVC\Model.h"


///////////////////////////////////////////////////////////////////////////////

LightsView::LightsView( const AxisAlignedBox& sceneBB )
   : m_lightsStorage( new LinearStorage< Light >() )
   , m_visibleLights( new LightsArray( 128 ) )
{
}

///////////////////////////////////////////////////////////////////////////////

LightsView::~LightsView()
{
   delete m_visibleLights;
   m_visibleLights = NULL;

   delete m_lightsStorage;
   m_lightsStorage = NULL;
}

///////////////////////////////////////////////////////////////////////////////

LightsArray* LightsView::queryLights( const RPBoundingVolume& volume )
{
   m_visibleLights->m_collection.clear();
   m_lightsStorage->query( volume, m_visibleLights->m_collection );
   m_lightsStorage->recalcSceneBounds();

   return m_visibleLights;
}

///////////////////////////////////////////////////////////////////////////////

void LightsView::onNodeAdded( SceneNode* node )
{
   if ( node->isA< Light >() )
   {
      m_lightsStorage->insert( *static_cast< Light* >( node ) );
   }
}

///////////////////////////////////////////////////////////////////////////////

void LightsView::onNodeRemoved( SceneNode* node )
{
   if ( node->isA< Light >() )
   {
      m_lightsStorage->remove( *static_cast< Light* >( node ) );
   }
}

///////////////////////////////////////////////////////////////////////////////

void LightsView::onAttachedToModel( Model& model )
{
   ModelView::onAttachedToModel( model );

   // populate the view with scene's contents
   model.pullStructure( this );
}

///////////////////////////////////////////////////////////////////////////////

void LightsView::resetContents( Model& scene )
{
   m_lightsStorage->clear();
}

///////////////////////////////////////////////////////////////////////////////
