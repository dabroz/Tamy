/// @file   ext-2DGameLevel\GL2DRoom.h
/// @brief  representation of a room used by the procedural game level generator
#pragma once

#include "core\MemoryRouter.h"
#include "core\Point.h"
#include "core\Array.h"
#include "core\List.h"
#include "core\Grid.h"


///////////////////////////////////////////////////////////////////////////////

class GL2DBounds;
class Prefab;
struct GL2DWall;
class GL2DVoxelPrefabsMap;
class GL2DProceduralLevel;
class GL2DRoomArea;
class GL2DRoomTheme;
class StaticGeometryTree;
struct GL2DWallVoxel;
class GL2DVoxelizedItem;
enum GL2DVoxelRotation;

///////////////////////////////////////////////////////////////////////////////

class GL2DRoom
{
   DECLARE_ALLOCATOR( GL2DRoom, AM_DEFAULT );

public:
   const GL2DRoomTheme*                   m_theme;

   List< Prefab* >                        m_requiredFurniture;

   uint                                   m_width;       // number of areas horizontally
   uint                                   m_height;      // number of areas vertically

   Point                                  m_pos;         // position of the room in pathfinding coords

   Array< GL2DRoomArea* >                 m_areas;       // all room areas
   List< GL2DRoomArea* >                  m_connectivityPoint; // position of a connectivity point in pathfinding coords

   List< const GL2DRoom* >                m_connectedRooms; // other rooms connected to this one

private:
   struct LayoutInfo
   {
      Point          m_wallOrigin[4];
      Point          m_wallDir[4];
      uint           m_wallOffset[8];

      byte           m_wallIdx;
      byte           m_fromFront[4];

      LayoutInfo( uint width, uint height );
   };

public:
   /**
    * Constructor.
    *
    * @param roomTypeId
    * @param size
    * @param arrExitDirections
    */
   GL2DRoom( const GL2DRoomTheme* theme, const Point& size, const Array< GL2DVoxelRotation >& arrExitDirections );
   ~GL2DRoom();

   /**
    * Sets the position of the room.
    *
    * @param pos
    */
   void setPosition( const Point& pos );

   /**
    * Translates the room by the specified offset.
    *
    * @param offset
    */
   void translate( const Point& offset );

   /**
    * Checks if the room overlaps with any of the specified rooms.
    *
    * @param rooms
    */
   bool doesOverlap( const Array< GL2DRoom* >& rooms ) const;

   /**
    * Returns the bounds of the room, inflated if specified.
    *
    * @param outBounds
    * @param inflationRadius
    */
   void getBounds( GL2DBounds& outBounds, int inflationRadius = 0 ) const;

   // -------------------------------------------------------------------------
   // Exits
   // -------------------------------------------------------------------------

   /**
    * Returns a pair of two nearest exits that would connect the specified rooms.
    */
   float getNearestExits( const GL2DRoom* otherRoom, Point& outCorridorStartPos, Point& outCorridorEndPos ) const;

   /**
   * Creates a connection to another room.
   *
   * @param otherRoom
   */
   void connectToRoom( GL2DRoom* otherRoom );

   /**
    * Checks if the two rooms are connected with each other.
    *
    * @param otherRoom
    */
   bool isConnectedTo( const GL2DRoom* otherRoom ) const;


   /**
    * Returns the number of exits from this room.
    */
   inline uint getExitsCount() const {
      return m_connectivityPoint.size();
   }

   /**
    * Returns a number of rooms connected with this room.
    */
   inline uint getConnectedRoomsCount() const {
      return m_connectedRooms.size();
   }

   // -------------------------------------------------------------------------
   // Furniture
   // -------------------------------------------------------------------------

   /**
    * Adds a custom piece of furniture to the room.
    *
    * @param prefab
    */
   void addFurniture( Prefab* prefab );

   // -------------------------------------------------------------------------
   // Voxelization
   // -------------------------------------------------------------------------

   /**
    * Voxelizes the pathfinding representation of the room
    *
    * @param pathfindingGrid
    */
   void voxelizeForPathfinding( Grid< uint >& pathfindingGrid ) const;

   /**
    * Renders the room.
    *
    * @param voxelsPerUnit
    * @param staticGeometryBuilder
    */
   void render( uint voxelsPerUnit, StaticGeometryTree& staticGeometryBuilder ) const;

private:
   /**
    * Initializes the size of the room.
    */
   void setSize( byte width, byte height );

   /**
    * Generates a number of exits.
    */
   void generateExits( const Array< GL2DVoxelRotation >& arrExitDirections );

   /**
    * Calculates the address of a cell located at the specified position.
    */
   inline uint calcAreaAddr( const Point& pos ) const {
      return m_width * pos.y + pos.x;
   }

   /**
    * Populates the room with furniture during the initialization stage.
    */
   void populateWithFurniture();

   /**
    * Adds a prefab to the internal furniture layout.
    */
   void addToLayout( const Prefab* prefab, LayoutInfo& layout );
};

///////////////////////////////////////////////////////////////////////////////
