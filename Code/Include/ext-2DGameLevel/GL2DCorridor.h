/// @file   ext-2DGameLevel\GL2DCorridor.h
/// @brief  a corridor representation
#pragma once

#include "core\MemoryRouter.h"
#include "core\Point.h"
#include "core\Grid.h"
#include "core\List.h"


///////////////////////////////////////////////////////////////////////////////

class GL2DProceduralLevel;
class GL2DRoomTheme;
struct GL2DWallVoxel;
class GL2DRoom;
class GL2DBounds;
class StaticGeometryTree;
class GL2DVoxelizedItem;

///////////////////////////////////////////////////////////////////////////////

class GL2DCorridor
{
   DECLARE_ALLOCATOR( GL2DCorridor, AM_DEFAULT );

public:
   const GL2DRoomTheme*          m_theme;

   // a list of path points in pathfinding coordinates
   Point                         m_start;
   Point                         m_end;
   List< Point >                 m_pathPoints;

public:
   /**
    * Constructor.
    *
    * @param theme
    */
   GL2DCorridor(  const GL2DRoomTheme* theme );

   /**
    * Gives the corridor a different theme.
    *
    * @param theme
    */
   inline void setTheme( const GL2DRoomTheme* theme ) {
      m_theme = theme;
   }

   /**
    * Returns the theme the room is currently using.
    */
   inline const GL2DRoomTheme* getTheme() const {
      return m_theme;
   }

   /**
    * Translates the corridor.
    *
    * @param offset
    */
   void translate( const Point& offset );

   /**
    * Retrieves the bounds of the corridor
    *
    * @param outBounds
    */
   void getBounds( GL2DBounds& outBounds ) const;

   /**
    * A straight line corridor.
    *
    * @param start
    * @param end
    */
   void drawStraightLine( const Point& start, const Point& end );

   /**
    * Voxelizes the corridor for pathfinding ( in low resolution ).
    *
    * @param outPathfindingGrid
    */
   void voxelizeForPathfinding( Grid< uint >& outPathfindingGrid ) const;

   /**
    * Renders the corridor
    *
    * @param cccupationGrid
    * @param voxelsPerCell
    * @param staticGeometryBuilder
    */
   void render( const Grid< uint >& cccupationGrid, uint voxelsPerCell, StaticGeometryTree& staticGeometryBuilder ) const;

private:
   GL2DVoxelizedItem* getCorridorItem( const Point& pos, const Grid< uint >& occupationGrid ) const;
};

///////////////////////////////////////////////////////////////////////////////
