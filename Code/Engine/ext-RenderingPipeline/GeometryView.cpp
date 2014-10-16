#include "ext-RenderingPipeline\GeometryView.h"
#include "ext-RenderingPipeline\GeometryStorage.h"
#include "core-Renderer\GeometryComponent.h"
#include "core-MVC\Entity.h"
#include "core-MVC\EntityUtils.h"
#include "core-MVC\EntitiesGroup.h"


///////////////////////////////////////////////////////////////////////////////

GeometryView::GeometryView( const AxisAlignedBox& sceneBB )
   : m_geometryStorage( new GeometryStorage() )
   , m_visibleGeometry( new GeometryArray( 1024 ) )
{
}

///////////////////////////////////////////////////////////////////////////////

GeometryView::~GeometryView()
{
   delete m_visibleGeometry;
   m_visibleGeometry = NULL;

   delete m_geometryStorage;
   m_geometryStorage = NULL;
}

///////////////////////////////////////////////////////////////////////////////

GeometryArray* GeometryView::queryGeometry( const RPBoundingVolume& volume )
{
   m_visibleGeometry->m_collection.clear();
   m_geometryStorage->query( volume, m_visibleGeometry->m_collection );
   m_geometryStorage->recalcSceneBounds();

   return m_visibleGeometry;
}

///////////////////////////////////////////////////////////////////////////////

void GeometryView::onNodeAdded( SceneNode* node )
{
   if ( node->isA< GeometryComponent >() )
   {
      GeometryComponent* renderable = static_cast< GeometryComponent* >( node );
      m_geometryStorage->insert( renderable );
   }
}

///////////////////////////////////////////////////////////////////////////////

void GeometryView::onNodeRemoved( SceneNode* node )
{
   if ( node->isA< GeometryComponent >() )
   {
      GeometryComponent* renderable = static_cast< GeometryComponent* >( node );
      
      // the opposite action to the one performed in onNodeAdded
      m_geometryStorage->remove( renderable );
   }
}

///////////////////////////////////////////////////////////////////////////////

void GeometryView::onAttachedToModel( Model& model )
{
   ModelView::onAttachedToModel( model );

   // populate the view with scene's contents
   model.pullStructure( this );
}

///////////////////////////////////////////////////////////////////////////////

void GeometryView::onDetachedFromModel( Model& model )
{
   ModelView::onDetachedFromModel( model );

   m_geometryStorage->clear();
}

///////////////////////////////////////////////////////////////////////////////

void GeometryView::onEntityAdded( Entity* entity )
{
   List< GeometryComponent* > renderables;
   EntityUtils::collectNodesByType< GeometryComponent >( entity, renderables );

   for ( List< GeometryComponent* >::iterator it = renderables.begin(); !it.isEnd(); ++it )
   {
      m_geometryStorage->insert( *it );
   }
}

///////////////////////////////////////////////////////////////////////////////

void GeometryView::onEntityRemoved( Entity* entity )
{
   List< GeometryComponent* > renderables;
   EntityUtils::collectNodesByType< GeometryComponent >( entity, renderables );

   for ( List< GeometryComponent* >::iterator it = renderables.begin(); !it.isEnd(); ++it )
   {
      m_geometryStorage->remove( *it );
   }
}

///////////////////////////////////////////////////////////////////////////////

void GeometryView::onAttachedToGroup( EntitiesGroup& group )
{
   EntitiesGroupView::onAttachedToGroup( group );

   // populate the view with scene's contents
   group.pullStructure( this );
}

///////////////////////////////////////////////////////////////////////////////

void GeometryView::onDetachedFromGroup( EntitiesGroup& group )
{
   EntitiesGroupView::onDetachedFromGroup( group );

   m_geometryStorage->clear();
}

///////////////////////////////////////////////////////////////////////////////
