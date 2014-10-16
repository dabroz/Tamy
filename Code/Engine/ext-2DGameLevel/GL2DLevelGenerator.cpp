#include "ext-2DGameLevel\GL2DLevelGenerator.h"
#include "ext-2DGameLevel\GL2DProceduralLevel.h"
#include "ext-2DGameLevel\GL2DLevelVoxels.h"

// utils
#include "ext-2DGameLevel\GL2DLevelGenUtils.h"
#include "ext-2DGameLevel\GL2DVoxelPrefabsMap.h"
#include "ext-2DGameLevel\GL2DVoxelizedItem.h"

// rooms
#include "ext-2DGameLevel\GL2DRoom.h"
#include "ext-2DGameLevel\GL2DRoomArea.h"
#include "ext-2DGameLevel\GL2DCorridor.h"
#include "ext-2DGameLevel\GL2DBounds.h"
#include "ext-2DGameLevel\GL2DWallVoxel.h"

// layouts
#include "ext-2DGameLevel\GL2DRoomLayout.h"

// level rendering
#include "core-MVC\Entity.h"
#include "core-Renderer\StaticGeometryTree.h"
#include "core-Renderer\RenderSystem.h"


///////////////////////////////////////////////////////////////////////////////

GL2DLevelGenerator::GL2DLevelGenerator( GL2DVoxelPrefabsMap& prefabsMap, GL2DProceduralLevel& params )
   : m_prefabsMap( prefabsMap )
   , m_params( params )
{
}

///////////////////////////////////////////////////////////////////////////////

GL2DLevelGenerator::~GL2DLevelGenerator()
{
   reset();
}

///////////////////////////////////////////////////////////////////////////////

void GL2DLevelGenerator::addRoom( GL2DRoom* room )
{
   m_rooms.push_back( room );
}

///////////////////////////////////////////////////////////////////////////////

void GL2DLevelGenerator::addLayout( GL2DRoomLayout* layout )
{
   m_layouts.pushBack( layout );
}

///////////////////////////////////////////////////////////////////////////////

void GL2DLevelGenerator::addCorridor( GL2DCorridor* corridor )
{
   m_corridors.pushBack( corridor );
}

///////////////////////////////////////////////////////////////////////////////

void GL2DLevelGenerator::reset()
{
   m_rooms.clear();
   m_corridors.clear();

   for ( List< GL2DRoomLayout* >::iterator it = m_layouts.begin(); !it.isEnd(); ++it )
   {
      GL2DRoomLayout* layout = *it;
      delete layout;
   }
   m_layouts.clear();
}

///////////////////////////////////////////////////////////////////////////////

Entity* GL2DLevelGenerator::renderLevel()
{
   m_rooms.clear();
   m_corridors.clear();

   // build a common array of rooms
   for ( List< GL2DRoomLayout* >::iterator it = m_layouts.begin(); !it.isEnd(); ++it )
   {
      GL2DRoomLayout* layout = *it;
      layout->build( *this );
   }

   if ( m_rooms.empty() )
   {
      return NULL;
   }

  /* // generate the connections between the rooms
   Graph< GL2DRoom* > roomsGraph;
   GL2DLevelGenUtils::buildGraphOfRooms( rooms, roomsGraph );
   
   GL2DBounds sceneBounds;
   GL2DLevelGenUtils::calculateSceneBounds( rooms, sceneBounds );

   // TODO: leave the corridors out for a bit
   
   // generate corridors
   {
      Point extents;
      sceneBounds.getExtents( extents );
      Grid< uint >* pathfindingGrid = new Grid< uint >( extents.x, extents.y );

      const uint roomsCount = rooms.size();
      for ( uint i = 0; i < roomsCount; ++i )
      {
         GL2DRoom* room = rooms[i];
         room->voxelizeForPathfinding( *pathfindingGrid );
      }

      // ...plot the corridors
      for ( List< GL2DCorridor* >::iterator it = m_corridors.begin(); !it.isEnd(); ++it )
      {
         GL2DCorridor* corridor = *it;
         corridor->voxelizeForPathfinding( m_params, *pathfindingGrid );
      }

      // cleanup
      delete pathfindingGrid;
   }
   */

   GL2DBounds sceneBounds;
   for ( List< GL2DRoomLayout* >::iterator it = m_layouts.begin(); !it.isEnd(); ++it )
   {
      GL2DRoomLayout* layout = *it;
      layout->getBounds( sceneBounds );
   }

   // voxelize the level
   Point boundsMin, boundsMax;
   sceneBounds.get( boundsMin, boundsMax );
   Grid< uint > occupationGrid( boundsMax.x, boundsMax.y );
   {
      const uint roomsCount = m_rooms.size();
      for ( byte i = 0; i < roomsCount; ++i )
      {
         GL2DRoom* room = m_rooms[i];
         room->voxelizeForPathfinding( occupationGrid );
      }

      for ( List< GL2DCorridor* >::iterator it = m_corridors.begin(); !it.isEnd(); ++it )
      {
         GL2DCorridor* corridor = *it;
         corridor->voxelizeForPathfinding( occupationGrid );
      }
   }

   // TODO: make sure that no room lies on the negative side of the origin - otherwise the grid based operations won't work

   // render the level
   Entity* proceduralGeometry = NULL;
   {

      // render the level
      Point extents;
      sceneBounds.getExtents( extents );
      AxisAlignedBox bounds( Vector_ZERO, Vector( (float)( extents.x * m_params.m_voxelsPerCell ), (float)( extents.y * m_params.m_voxelsPerCell ), 0.0f ) );
      StaticGeometryTree staticGeometryBuilder( bounds );


      const uint roomsCount = m_rooms.size();
      for ( byte i = 0; i < roomsCount; ++i )
      {
         GL2DRoom* room = m_rooms[i];
         room->render( m_params.m_voxelsPerCell, staticGeometryBuilder );
      }

      for ( List< GL2DCorridor* >::iterator it = m_corridors.begin(); !it.isEnd(); ++it )
      {
         GL2DCorridor* corridor = *it;
         corridor->render( occupationGrid, m_params.m_voxelsPerCell, staticGeometryBuilder );
      }

      // build the geometry
      proceduralGeometry = staticGeometryBuilder.build( m_params.m_geometryDeploymentDir, "ProceduralLevel" );

      // This operation could potentially generate some render commands related to 
      // rebuilding the geometry of the existing triangle meshes - so we need to
      // force-flush the rendering thread
      RenderSystem& renderSystem = TSingleton< RenderSystem >::getInstance();
      renderSystem.flush();
   }

   return proceduralGeometry;
}

///////////////////////////////////////////////////////////////////////////////
