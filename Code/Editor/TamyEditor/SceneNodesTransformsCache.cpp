#include "SceneNodesTransformsCache.h"
#include "core-MVC\Entity.h"
#include "core\ListUtils.h"


///////////////////////////////////////////////////////////////////////////////

SceneNodesTransformsCache::SceneNodesTransformsCache()
{
}

///////////////////////////////////////////////////////////////////////////////

SceneNodesTransformsCache::~SceneNodesTransformsCache()
{
}

///////////////////////////////////////////////////////////////////////////////

void SceneNodesTransformsCache::cacheTransforms( bool cache )
{
   if ( cache )
   {
      for ( List< SceneNodeTransformEntry >::iterator it = m_entities.begin(); !it.isEnd(); ++it )
      {
         SceneNodeTransformEntry& entry = *it;
         entry.m_cachedTransform = entry.m_entity->getLocalMtx();
      }
   }
   else
   {
      for ( List< SceneNodeTransformEntry >::iterator it = m_entities.begin(); !it.isEnd(); ++it )
      {
         SceneNodeTransformEntry& entry = *it;
         entry.m_entity->setLocalMtx( entry.m_cachedTransform );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void SceneNodesTransformsCache::onAttachedToModel( Model& model )
{
   ModelView::onAttachedToModel( model );
   model.pullStructure( this );
}

///////////////////////////////////////////////////////////////////////////////

void SceneNodesTransformsCache::onNodeAdded( SceneNode* node )
{
   if ( !node->isA< Entity >() )
   {
      return;
   }

   Entity* entity = static_cast< Entity* >( node );
   List< SceneNodeTransformEntry >::iterator it = ListUtils::find< SceneNodeTransformEntry, Entity*, SceneNodeTransformEntry >( m_entities, entity );
   if ( it.isEnd() )
   {
      m_entities.pushBack( SceneNodeTransformEntry( entity ) );
   }
}

///////////////////////////////////////////////////////////////////////////////

void SceneNodesTransformsCache::onNodeRemoved( SceneNode* node )
{
   if ( !node->isA< Entity >() )
   {
      return;
   }

   Entity* entity = static_cast< Entity* >( node );
   List< SceneNodeTransformEntry >::iterator it = ListUtils::find< SceneNodeTransformEntry, Entity*, SceneNodeTransformEntry >( m_entities, entity );
   if ( !it.isEnd() )
   {
      it.markForRemoval();
   }
}

///////////////////////////////////////////////////////////////////////////////

void SceneNodesTransformsCache::resetContents( Model& scene )
{
   m_entities.clear();
}

///////////////////////////////////////////////////////////////////////////////
