/// @file   ext-2DGameLevel\GL2DRoomArea.h
/// @brief  room areas
#pragma once

#include "core\MemoryRouter.h"
#include "core\Point.h"
#include "core\Grid.h"
#include "ext-2DGameLevel\GL2DBounds.h"
#include "ext-2DGameLevel\GL2DLevelVoxels.h"


///////////////////////////////////////////////////////////////////////////////

class StaticGeometryTree;
class GL2DVoxelizedItem;
class Prefab;
class GL2DRoomTheme;
class StaticGeometryTree;

///////////////////////////////////////////////////////////////////////////////

class GL2DRoomArea
{
   DECLARE_ALLOCATOR( GL2DRoomArea, AM_DEFAULT );

private:
   Point                         m_offset;
   bool                          m_topWall;
   bool                          m_bottomWall;
   bool                          m_leftWall;
   bool                          m_rightWall;

   bool                          m_door;
   const GL2DVoxelizedItem*      m_geometryItem;
   GL2DVoxelizedItem*            m_decorationItem;

public:
   /**
    * Constructor.
    */
   GL2DRoomArea( const Point& offset, bool topWall, bool bottomWall, bool leftWall, bool rightWall );
   ~GL2DRoomArea();

   /**
    * Sets the base geometry item that represents this voxel.
    *
    * @param theme
    * @param door
    */
   void setGeometryItem( const GL2DRoomTheme* theme, bool door );

   /**
    * Returns the base geometry item of this area.
    */
   inline const GL2DVoxelizedItem* getGeometryItem() const {
      return m_geometryItem;
   }

   /**
    * Sets the decoration item of the voxel.
    *
    * @param itemPrefab
    * @return 'true' if the item was successfully added, 'false' otherwise
    */
   bool setDecorationItem( const Prefab* itemPrefab );

   /**
    * Returns the position of the connector ( in pathfinding coordinates )
    * the door area exposes.
    *
    * @param outPos
    */
   void getConnectorPos( Point& outPos ) const;

   /**
    * Renders the items located in the area.
    *
    * @param parentTranslation
    * @param voxelsPerUnit
    * @param staticGeometryBuilder
    */
   void render( const Point& parentTranslation, uint voxelsPerUnit, StaticGeometryTree& staticGeometryBuilder ) const;

private:
   void updateRotationForAlignment();
};

///////////////////////////////////////////////////////////////////////////////
