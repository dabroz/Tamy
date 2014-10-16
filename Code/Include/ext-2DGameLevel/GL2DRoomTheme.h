/// @file   ext-2DGameLevel\GL2DRoomTheme.h
/// @brief  graphics theme of a room
#pragma once

#include "core\MemoryRouter.h"
#include "core\Array.h"
#include "core\List.h"
#include <string>


///////////////////////////////////////////////////////////////////////////////

class Prefab;
class GL2DVoxelizedItem;

///////////////////////////////////////////////////////////////////////////////

enum CoreVoxelItemsIds
{
   LVP_FLOOR,

   LVP_WALL_TOP,
   LVP_WALL_RIGHT,
   LVP_WALL_BOTTOM,
   LVP_WALL_LEFT,

   LVP_WALL_CORNER_TOP_LEFT,
   LVP_WALL_CORNER_TOP_RIGHT,
   LVP_WALL_CORNER_BOTTOM_RIGHT,
   LVP_WALL_CORNER_BOTTOM_LEFT,

   LVP_DOOR_TOP,
   LVP_DOOR_RIGHT,
   LVP_DOOR_BOTTOM,
   LVP_DOOR_LEFT,

   LVP_DOOR_CORNER_TOP_LEFT,
   LVP_DOOR_CORNER_TOP_RIGHT,
   LVP_DOOR_CORNER_BOTTOM_RIGHT,
   LVP_DOOR_CORNER_BOTTOM_LEFT,

   LVP_CORRIDOR_PLATFORM,

   LVP_CORRIDOR_WALL_TOP_BOTTOM,
   LVP_CORRIDOR_WALL_LEFT_RIGHT,

   LVP_CORRIDOR_DEAD_END_LEFT_TOP_RIGHT,
   LVP_CORRIDOR_DEAD_END_TOP_RIGHT_BOTTOM,
   LVP_CORRIDOR_DEAD_END_RIGHT_BOTTOM_LEFT, 
   LVP_CORRIDOR_DEAD_END_BOTTOM_LEFT_TOP,

   LVP_CORRIDOR_CORNER_TOP_LEFT,
   LVP_CORRIDOR_CORNER_TOP_RIGHT,
   LVP_CORRIDOR_CORNER_BOTTOM_RIGHT,
   LVP_CORRIDOR_CORNER_BOTTOM_LEFT,

   LVP_CORRIDOR_WALL_TOP,
   LVP_CORRIDOR_WALL_RIGHT, 
   LVP_CORRIDOR_WALL_BOTTOM,
   LVP_CORRIDOR_WALL_LEFT,

   LVP_ITEMS_COUNT
};

///////////////////////////////////////////////////////////////////////////////

class GL2DRoomTheme
{
   DECLARE_ALLOCATOR( GL2DRoomTheme, AM_DEFAULT );

public:
   std::string                               m_name;

private:
   Array< GL2DVoxelizedItem* >               m_coreItems;
   Array< const Prefab* >                    m_items;

public:
   /**
    * Constructor.
    *
    * @param name
    */
   GL2DRoomTheme( const std::string& name );
   ~GL2DRoomTheme();

   /**
    * Adds a prefab to the theme.
    *
    * @param prefab
    */
   void addCorePrefab( const Prefab* prefab );
   
   /**
    * Adds a prefab to the theme.
    *
    * @param prefab
    */
   void addDecorativePrefab( const Prefab* prefab );

   /**
    * Returns a shared item that corresponds to the specified index.
    *
    * @param index
    */
   GL2DVoxelizedItem* getCoreItem( CoreVoxelItemsIds index ) const;

   /**
    * Returns a random furniture prefab.
    */
   const Prefab* getRandomFurniturePrefab() const;
};

///////////////////////////////////////////////////////////////////////////////
