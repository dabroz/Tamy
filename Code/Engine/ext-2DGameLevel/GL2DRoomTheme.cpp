#include "ext-2DGameLevel\GL2DRoomTheme.h"
#include "ext-2DGameLevel\GL2DVoxelizedItem.h"
#include "ext-2DGameLevel\GL2DLevelVoxels.h"

// prefabs and entities
#include "core-MVC\Prefab.h"
#include "core-MVC\Entity.h"
#include "core-MVC\EntityUtils.h"

// utils
#include "core\ListUtils.h"


///////////////////////////////////////////////////////////////////////////////

GL2DRoomTheme::GL2DRoomTheme( const std::string& name )
   : m_name( name )
   , m_coreItems( LVP_ITEMS_COUNT )
{
   m_coreItems.resize( LVP_ITEMS_COUNT, NULL );
}

///////////////////////////////////////////////////////////////////////////////

GL2DRoomTheme::~GL2DRoomTheme()
{
   const uint count = m_coreItems.size();
   for ( uint i = 0; i < count; ++i )
   {
      delete m_coreItems[i];
   }
   m_coreItems.clear();
   m_items.clear();
}

///////////////////////////////////////////////////////////////////////////////

void GL2DRoomTheme::addCorePrefab( const Prefab* prefab )
{
   std::string prefabFileName = prefab->getFilePath().extractNodeName();

   if ( prefabFileName.find( "RoomFloor" ) != std::string::npos )
   {
      m_coreItems[LVP_FLOOR] = new GL2DVoxelizedItem( prefab, VRot_0 );
   }
   else if ( prefabFileName.find( "WallStraight" ) != std::string::npos )
   {
      m_coreItems[LVP_WALL_TOP] = new GL2DVoxelizedItem( prefab, VRot_0 );
      m_coreItems[LVP_WALL_RIGHT] = new GL2DVoxelizedItem( prefab, VRot_90 );
      m_coreItems[LVP_WALL_BOTTOM] = new GL2DVoxelizedItem( prefab, VRot_180 );
      m_coreItems[LVP_WALL_LEFT] = new GL2DVoxelizedItem( prefab, VRot_270 );
   }
   else if ( prefabFileName.find( "WallCorner" ) != std::string::npos )
   {
      m_coreItems[LVP_WALL_CORNER_TOP_LEFT] = new GL2DVoxelizedItem( prefab, VRot_0 );
      m_coreItems[LVP_WALL_CORNER_TOP_RIGHT] = new GL2DVoxelizedItem( prefab, VRot_90 );
      m_coreItems[LVP_WALL_CORNER_BOTTOM_RIGHT] = new GL2DVoxelizedItem( prefab, VRot_180 );
      m_coreItems[LVP_WALL_CORNER_BOTTOM_LEFT] = new GL2DVoxelizedItem( prefab, VRot_270 );
   }
   else if ( prefabFileName.find( "DoorStraight" ) != std::string::npos )
   {
      m_coreItems[LVP_DOOR_TOP] = new GL2DVoxelizedItem( prefab, VRot_0 );
      m_coreItems[LVP_DOOR_RIGHT] = new GL2DVoxelizedItem( prefab, VRot_90 );
      m_coreItems[LVP_DOOR_BOTTOM] = new GL2DVoxelizedItem( prefab, VRot_180 );
      m_coreItems[LVP_DOOR_LEFT] = new GL2DVoxelizedItem( prefab, VRot_270 );
   }
   else if ( prefabFileName.find( "DoorCorner" ) != std::string::npos )
   {
      m_coreItems[LVP_DOOR_CORNER_TOP_LEFT] = new GL2DVoxelizedItem( prefab, VRot_0 );
      m_coreItems[LVP_DOOR_CORNER_TOP_RIGHT] = new GL2DVoxelizedItem( prefab, VRot_90 );
      m_coreItems[LVP_DOOR_CORNER_BOTTOM_RIGHT] = new GL2DVoxelizedItem( prefab, VRot_180 );
      m_coreItems[LVP_DOOR_CORNER_BOTTOM_LEFT] = new GL2DVoxelizedItem( prefab, VRot_270 );
   }
   else if ( prefabFileName.find( "CorridorFloor" ) != std::string::npos )
   {
      m_coreItems[LVP_CORRIDOR_PLATFORM] = new GL2DVoxelizedItem( prefab, VRot_0 );
   }
   else if ( prefabFileName.find( "CorridorStraightWallTwoSides" ) != std::string::npos )
   {
      m_coreItems[LVP_CORRIDOR_WALL_TOP_BOTTOM] = new GL2DVoxelizedItem( prefab, VRot_0 );
      m_coreItems[LVP_CORRIDOR_WALL_LEFT_RIGHT] = new GL2DVoxelizedItem( prefab, VRot_90 );
   }
   else if ( prefabFileName.find( "CorridorStraightWallOneSide" ) != std::string::npos )
   {
      m_coreItems[LVP_CORRIDOR_WALL_TOP] = new GL2DVoxelizedItem( prefab, VRot_0 );
      m_coreItems[LVP_CORRIDOR_WALL_RIGHT] = new GL2DVoxelizedItem( prefab, VRot_90 );
      m_coreItems[LVP_CORRIDOR_WALL_BOTTOM] = new GL2DVoxelizedItem( prefab, VRot_180 );
      m_coreItems[LVP_CORRIDOR_WALL_LEFT] = new GL2DVoxelizedItem( prefab, VRot_270 );
   }
   else if ( prefabFileName.find( "CorridorDeadEnd" ) != std::string::npos )
   {
      m_coreItems[LVP_CORRIDOR_DEAD_END_LEFT_TOP_RIGHT] = new GL2DVoxelizedItem( prefab, VRot_0 );
      m_coreItems[LVP_CORRIDOR_DEAD_END_TOP_RIGHT_BOTTOM] = new GL2DVoxelizedItem( prefab, VRot_90 );
      m_coreItems[LVP_CORRIDOR_DEAD_END_RIGHT_BOTTOM_LEFT] = new GL2DVoxelizedItem( prefab, VRot_180 );
      m_coreItems[LVP_CORRIDOR_DEAD_END_BOTTOM_LEFT_TOP] = new GL2DVoxelizedItem( prefab, VRot_270 );
   }
   else if ( prefabFileName.find( "CorridorCorner" ) != std::string::npos )
   {
      m_coreItems[LVP_CORRIDOR_CORNER_TOP_LEFT] = new GL2DVoxelizedItem( prefab, VRot_0 );
      m_coreItems[LVP_CORRIDOR_CORNER_TOP_RIGHT] = new GL2DVoxelizedItem( prefab, VRot_90 );
      m_coreItems[LVP_CORRIDOR_CORNER_BOTTOM_RIGHT] = new GL2DVoxelizedItem( prefab, VRot_180 );
      m_coreItems[LVP_CORRIDOR_CORNER_BOTTOM_LEFT] = new GL2DVoxelizedItem( prefab, VRot_270 );
   }
}

///////////////////////////////////////////////////////////////////////////////

void GL2DRoomTheme::addDecorativePrefab( const Prefab* prefab )
{
   // check for duplicate entries
   const uint count = m_items.size();
   for ( uint i = 0; i < count; ++i )
   {
      if ( m_items[i] == prefab )
      {
         return;
      }
   }

   m_items.push_back( prefab );
}

///////////////////////////////////////////////////////////////////////////////

GL2DVoxelizedItem* GL2DRoomTheme::getCoreItem( CoreVoxelItemsIds index ) const
{
   return m_coreItems[index];
}

///////////////////////////////////////////////////////////////////////////////

const Prefab* GL2DRoomTheme::getRandomFurniturePrefab() const
{
   const uint itemsCount = m_items.size();
   if ( itemsCount == 0 )
   {
      return NULL;
   }

   const uint prefabIdx = rand() % itemsCount;
   return m_items[prefabIdx];
}

///////////////////////////////////////////////////////////////////////////////
