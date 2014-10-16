#include "ext-RenderingPipeline\GeometryStorage.h"
#include "ext-RenderingPipeline\RPDataProxies.h"
#include "core-Renderer\GeometryComponent.h"
#include "core-MVC\Entity.h"
#include "core-MVC\EntityUtils.h"
#include "core\ListUtils.h"


///////////////////////////////////////////////////////////////////////////////

void GeometryStorage::query( const RPBoundingVolume& volume, Array< GeometryComponent* >& outVisibleElems ) const
{
   query( volume, false, outVisibleElems );
}

///////////////////////////////////////////////////////////////////////////////

void GeometryStorage::queryForShadowMapping( const RPBoundingVolume& volume, Array< GeometryComponent* >& outVisibleElems ) const
{
   query( volume, true, outVisibleElems );
}

///////////////////////////////////////////////////////////////////////////////

void GeometryStorage::query( const RPBoundingVolume& volume, bool shadowCasters, Array< GeometryComponent* >& outVisibleElems ) const
{
   // preallocate certain size in order to avoid frequent reallocations
   {
      // at this moment we're making an assumption that all scene elements will be visible
      uint newSize = outVisibleElems.size() + m_renderables.size();
      outVisibleElems.allocate( newSize );
   }

   for ( List< GeometryComponent* >::const_iterator it = m_renderables.begin(); !it.isEnd(); ++it )
   {
      GeometryComponent* renderable = *it;

      // check the flags
      if ( shadowCasters && !renderable->isShadowcaster() )
      {
         continue;
      }
      
      const AxisAlignedBox& boundsWorldSpace = renderable->getBoundingVolume();
      if ( volume.testIntersection( boundsWorldSpace ) )
      {
         outVisibleElems.push_back( renderable );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void GeometryStorage::insert( GeometryComponent* renderable )
{
   List< GeometryComponent* >::iterator it = ListUtils::find( m_renderables, renderable );
   if ( it.isEnd() )
   {
      m_renderables.pushBack( renderable );
   }
}

///////////////////////////////////////////////////////////////////////////////

void GeometryStorage::remove( GeometryComponent* renderable )
{
   List< GeometryComponent* >::iterator it = ListUtils::find( m_renderables, renderable );
   if ( !it.isEnd() )
   {
      it.markForRemoval();
   }
}

///////////////////////////////////////////////////////////////////////////////

void GeometryStorage::clear()
{
   m_renderables.clear();
}

///////////////////////////////////////////////////////////////////////////////

void GeometryStorage::getSceneBounds( AxisAlignedBox& outBounds )
{
   outBounds = m_sceneBounds;
}

///////////////////////////////////////////////////////////////////////////////

void GeometryStorage::recalcSceneBounds()
{
   m_sceneBounds.reset();

   for ( List< GeometryComponent* >::iterator it = m_renderables.begin(); !it.isEnd(); ++it )
   {
      GeometryComponent* renderable = *it;

      const AxisAlignedBox& boundsWorldSpace = renderable->getBoundingVolume();
      m_sceneBounds.add( boundsWorldSpace, m_sceneBounds );
   }
}

///////////////////////////////////////////////////////////////////////////////
