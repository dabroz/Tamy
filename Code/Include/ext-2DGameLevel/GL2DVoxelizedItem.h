/// @file   ext-2DGameLevel\GL2DVoxelizedItem.h
/// @brief  a single item that can be voxelized and then rendered using an assigned prefab
#pragma once

#include "core\MemoryRouter.h"
#include "core\Point.h"


///////////////////////////////////////////////////////////////////////////////

class Prefab;
class StaticGeometryTree;
enum GL2DVoxelRotation;
class GL2DBounds;

///////////////////////////////////////////////////////////////////////////////

class GL2DVoxelizedItem
{
   DECLARE_ALLOCATOR( GL2DVoxelizedItem, AM_DEFAULT );

public:
   const Prefab*        m_prefab;
   
   GL2DVoxelRotation    m_rotation;

public:
   /**
    * Constructor.
    *
    * @param prefab
    * @param rotation
    */
   GL2DVoxelizedItem( const Prefab* prefab, GL2DVoxelRotation rotation );

   /**
    * Copy constructor.
    *
    * @param rhs
    */
   GL2DVoxelizedItem( const GL2DVoxelizedItem& rhs );

   /**
   * Sets the rotation of the item.
   *
   * @param rotation
   */
   inline void setRotation( GL2DVoxelRotation rotation ) {
      m_rotation = rotation;
   }

   /**
    * Returns the rotation of the item.
    */
   inline GL2DVoxelRotation getRotation() const {
      return m_rotation;
   }

   /**
    * Renders the voxel
    *
    * @param pos              ( in world space )
    * @param staticGeometry
    */
   void render( const Point& pos, StaticGeometryTree& staticGeometry ) const;
};

///////////////////////////////////////////////////////////////////////////////
