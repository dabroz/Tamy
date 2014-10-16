#include "ext-2DGameLevel\GL2DCorridor.h"
#include "ext-2DGameLevel\GL2DProceduralLevel.h"
#include "ext-2DGameLevel\GL2DLevelVoxels.h"
#include "ext-2DGameLevel\GL2DRoom.h"
#include "ext-2DGameLevel\GL2DWallVoxel.h"
#include "ext-2DGameLevel\GL2DBounds.h"
#include "ext-2DGameLevel\GL2DVoxelizedItem.h"
#include "ext-2DGameLevel\GL2DRoomTheme.h"
#include "core-Renderer\StaticGeometryTree.h"
#include "core\GridUtils.h"
#include "core\List.h"


///////////////////////////////////////////////////////////////////////////////

GL2DCorridor::GL2DCorridor( const GL2DRoomTheme* theme )
   : m_theme( theme )
{
}

///////////////////////////////////////////////////////////////////////////////

void GL2DCorridor::translate( const Point& offset )
{
   m_start += offset;
   m_end += offset;

   for ( List< Point >::iterator it = m_pathPoints.begin(); !it.isEnd(); ++it )
   {
      Point& pt = *it;
      pt += offset;
   }
}

///////////////////////////////////////////////////////////////////////////////

void GL2DCorridor::getBounds( GL2DBounds& outBounds ) const
{
   Point offset( 1, 1 );
   for ( List< Point >::const_iterator it = m_pathPoints.begin(); !it.isEnd(); ++it )
   {
      const Point& pt = *it;
      outBounds.include( pt + offset );
   }
}

///////////////////////////////////////////////////////////////////////////////

void GL2DCorridor::drawStraightLine( const Point& start, const Point& end )
{
   m_start = start;
   m_end = end;

   m_pathPoints.clear();
   Point dir;
   dir.setAsDirection( end - start );
   for ( Point pt = m_start; pt != m_end; pt += dir )
   {
      m_pathPoints.pushBack( pt );
   }
}

///////////////////////////////////////////////////////////////////////////////

static uint levelCostFunc( const Grid< uint >& grid, const Point& pt )
{
   if ( pt.x < 0 || pt.y < 0 || pt.x >= ( int ) grid.width() || pt.y >= ( int ) grid.height() )
   {
      return GRID_CELLCOST_BLOCKED;
   }

   const uint cellType = grid.getValue( pt );
   if ( cellType == PF_ROOM_FLOOR_ID )
   {
      // rooms are considered obstacles when it comes to pathfinding
      return GRID_CELLCOST_BLOCKED;
   }

   if ( cellType == PF_CORRIDOR_FLOOR_ID )
   {
      // join the corridors wherever possible - give it a preferential treatment
      return 0;
   }

   // this is a hole - give it a slightly higher weight so that the algorithm chooses to join corridors
   // rather than create two one next to another
   return 10;
}

///////////////////////////////////////////////////////////////////////////////

void GL2DCorridor::voxelizeForPathfinding( Grid< uint >& outPathfindingGrid ) const
{
   for ( List< Point >::const_iterator it = m_pathPoints.begin(); !it.isEnd(); ++it )
   {
      const Point& pos = *it;
      int posAddr = outPathfindingGrid.calcAddr( pos );
      outPathfindingGrid[posAddr] = PF_CORRIDOR_FLOOR_ID;
   }

  /* GridSearchInfo< uint > searchInfo;
   searchInfo.m_start = m_start;
   searchInfo.m_end = m_end;
   searchInfo.m_traversalCostFunction = params.getCostFunction();
   searchInfo.m_cellCostFunction = &levelCostFunc;

   if ( GridUtils< uint >::aStar( outPathfindingGrid, searchInfo ) )
   {
      m_pathPoints.clear();

      for ( List< Point >::iterator it = searchInfo.m_pathPoints.begin(); !it.isEnd(); ++it )
      {
         const Point& pos = *it;
         int posAddr = outPathfindingGrid.calcAddr( pos );
         outPathfindingGrid[posAddr] = PF_CORRIDOR_FLOOR_ID;
      }

      m_pathPoints = searchInfo.m_pathPoints;
   }   */
}

///////////////////////////////////////////////////////////////////////////////

void GL2DCorridor::render( const Grid< uint >& occupationGrid, uint voxelsPerCell, StaticGeometryTree& staticGeometryBuilder ) const
{
   for ( List< Point >::const_iterator it = m_pathPoints.begin(); !it.isEnd(); ++it )
   {
      const Point& pos = *it;

      GL2DVoxelizedItem* item = getCorridorItem( pos, occupationGrid );
      if ( item )
      {
         item->render( pos * voxelsPerCell, staticGeometryBuilder );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

GL2DVoxelizedItem* GL2DCorridor::getCorridorItem( const Point& pos, const Grid< uint >& occupationGrid ) const
{
   const Point d[4] = {
      Point( -1, 0 ), // left
      Point( 0, -1 ), // top
      Point( 1, 0 ), // right
      Point( 0, 1 ), // bottom
   };

   enum {
      LEFT,
      TOP,
      RIGHT,
      BOTTOM
   };

   // scan the surrounding
   bool walls[4];
   for ( byte i = 0; i < 4; ++i )
   {
      const uint voxelId = occupationGrid.getValue( pos + d[i] );
      walls[i] = ( voxelId == PF_HOLE_ID );
   }

   // select an item:
   
   // dead ends
   {
      if ( walls[LEFT] && walls[TOP] && walls[RIGHT] )
      {
         GL2DVoxelizedItem* item = m_theme->getCoreItem( LVP_CORRIDOR_DEAD_END_LEFT_TOP_RIGHT );
         return item;
      }
      else if ( walls[TOP] && walls[RIGHT] && walls[BOTTOM] )
      {
         GL2DVoxelizedItem* item = m_theme->getCoreItem( LVP_CORRIDOR_DEAD_END_TOP_RIGHT_BOTTOM );
         return item;
      }
      else if ( walls[RIGHT] && walls[BOTTOM] && walls[LEFT] )
      {
         GL2DVoxelizedItem* item = m_theme->getCoreItem( LVP_CORRIDOR_DEAD_END_RIGHT_BOTTOM_LEFT );
         return item;
      }
      else if ( walls[BOTTOM] && walls[LEFT] && walls[TOP] )
      {
         GL2DVoxelizedItem* item = m_theme->getCoreItem( LVP_CORRIDOR_DEAD_END_BOTTOM_LEFT_TOP );
         return item;
      }
   }

   // corners
   {
      if ( walls[LEFT] && walls[TOP] )
      {
         GL2DVoxelizedItem* item = m_theme->getCoreItem( LVP_CORRIDOR_CORNER_TOP_LEFT );
         return item;
      }
      else if ( walls[TOP] && walls[RIGHT]  )
      {
         GL2DVoxelizedItem* item = m_theme->getCoreItem( LVP_CORRIDOR_CORNER_TOP_RIGHT );
         return item;
      }
      else if ( walls[RIGHT] && walls[BOTTOM])
      {
         GL2DVoxelizedItem* item = m_theme->getCoreItem( LVP_CORRIDOR_CORNER_BOTTOM_RIGHT );
         return item;
      }
      else if ( walls[BOTTOM] && walls[LEFT] )
      {
         GL2DVoxelizedItem* item = m_theme->getCoreItem( LVP_CORRIDOR_CORNER_BOTTOM_LEFT );
         return item;
      }
   }

   // two walled corridors
   {
      if ( walls[TOP] && walls[BOTTOM] )
      {
         GL2DVoxelizedItem* item = m_theme->getCoreItem( LVP_CORRIDOR_WALL_TOP_BOTTOM );
         return item;
      }
      else if ( walls[LEFT] && walls[RIGHT] )
      {
         GL2DVoxelizedItem* item = m_theme->getCoreItem( LVP_CORRIDOR_WALL_LEFT_RIGHT );
         return item;
      }
   }

   // one walled corridors
   {
      if ( walls[TOP] )
      {
         GL2DVoxelizedItem* item = m_theme->getCoreItem( LVP_CORRIDOR_WALL_TOP );
         return item;
      }
      else if ( walls[RIGHT] )
      {
         GL2DVoxelizedItem* item = m_theme->getCoreItem( LVP_CORRIDOR_WALL_RIGHT );
         return item;
      }
      else if ( walls[BOTTOM] )
      {
         GL2DVoxelizedItem* item = m_theme->getCoreItem( LVP_CORRIDOR_WALL_BOTTOM );
         return item;
      }
      else if ( walls[LEFT] )
      {
         GL2DVoxelizedItem* item = m_theme->getCoreItem( LVP_CORRIDOR_WALL_LEFT );
         return item;
      }
   }
   
   // a platform
   GL2DVoxelizedItem* item = m_theme->getCoreItem( LVP_CORRIDOR_PLATFORM );
   return item;

}

///////////////////////////////////////////////////////////////////////////////
