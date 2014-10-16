/// @file   ext-RenderingPipeline/GeometryStorage.h
/// @brief  spatial geometry storage
#pragma once

#include "core\MemoryRouter.h"
#include "core\Array.h"
#include "core\List.h"
#include "core\AxisAlignedBox.h"


///////////////////////////////////////////////////////////////////////////////

class GeometryComponent;
class Entity;
struct AxisAlignedBox;
class RPBoundingVolume;

///////////////////////////////////////////////////////////////////////////////

class GeometryStorage
{
   DECLARE_ALLOCATOR( GeometryStorage, AM_ALIGNED_16 );

private:
   List< GeometryComponent* >          m_renderables;
   AxisAlignedBox                      m_sceneBounds;

public:
   /**
    * Recalculates scene bounds.
    */
   void recalcSceneBounds();

   /**
    * Queries the storage contents, returning only the elements contained in the specified bounding volume
    *
    * @param volume
    * @param outVisibleElems
    */
   void query( const RPBoundingVolume& volume, Array< GeometryComponent* >& outVisibleElems) const;

   /**
   * Queries the geometry that casts shadows
   *
   * @param volume
   * @param outVisibleElems
   */
   void queryForShadowMapping( const RPBoundingVolume& volume, Array< GeometryComponent* >& outVisibleElems ) const;

   /**
    * Inserts a entity.
    *
    * @param renderable.
    */
   void insert( GeometryComponent* renderable );

   /**
    * Removes a renderable.
    *
    * @param renderable.
    */
   void remove( GeometryComponent* renderable );

   /**
    * Clears the storage
    */
   void clear();
   
   /**
    * Returns the bounds around all geometry this storage contains.
    *
    * @param outBounds
    */
   void getSceneBounds( AxisAlignedBox& outBounds );

private:
   void query( const RPBoundingVolume& volume, bool shadowCasters, Array< GeometryComponent* >& outVisibleElems ) const;

};

///////////////////////////////////////////////////////////////////////////////
