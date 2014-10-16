#include "ext-RenderingPipeline\CustomComponentsView.h"

// custom components
#include "ext-RenderingPipeline\LocationRenderSettings.h"

// model
#include "core-MVC\Model.h"


///////////////////////////////////////////////////////////////////////////////

CustomComponentsView::CustomComponentsView()
   : m_locationSettings( NULL )
{
}

///////////////////////////////////////////////////////////////////////////////

CustomComponentsView::~CustomComponentsView()
{
   m_locationSettings = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void CustomComponentsView::query( const RPBoundingVolume& volume )
{
}

///////////////////////////////////////////////////////////////////////////////

void CustomComponentsView::onNodeAdded( SceneNode* node )
{
   if ( node->isA< LocationRenderSettings >() )
   {
      m_locationSettings = static_cast< LocationRenderSettings* >( node );
   }
}

///////////////////////////////////////////////////////////////////////////////

void CustomComponentsView::onNodeRemoved( SceneNode* node )
{
   if ( node == m_locationSettings )
   {
      m_locationSettings = NULL;
   }
}

///////////////////////////////////////////////////////////////////////////////

void CustomComponentsView::onAttachedToModel( Model& model )
{
   ModelView::onAttachedToModel( model );

   // populate the view with scene's contents
   model.pullStructure( this );
}

///////////////////////////////////////////////////////////////////////////////

void CustomComponentsView::resetContents( Model& scene )
{
   m_locationSettings = NULL;
}

///////////////////////////////////////////////////////////////////////////////
