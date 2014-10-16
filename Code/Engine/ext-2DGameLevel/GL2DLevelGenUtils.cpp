#include "ext-2DGameLevel\GL2DLevelGenUtils.h"
#include "ext-2DGameLevel\GL2DProceduralLevel.h"
#include "core\GridUtils.h"
#include "core\GraphUtils.h"
#include "core\List.h"
#include "core\LocalList.h"
#include "core\ListUtils.h"
#include "core\BestResultsList.h"

// voxel definitions
#include "ext-2DGameLevel\GL2DLevelVoxels.h"
#include "ext-2DGameLevel\GL2DVoxelPrefabsMap.h"

// rooms 
#include "ext-2DGameLevel\GL2DRoom.h"
#include "ext-2DGameLevel\GL2DRoomArea.h"
#include "ext-2DGameLevel\GL2DRoomTheme.h"
#include "ext-2DGameLevel\GL2DCorridor.h"
#include "ext-2DGameLevel\GL2DBounds.h"
#include "ext-2DGameLevel\GL2DWallVoxel.h"


///////////////////////////////////////////////////////////////////////////////

uint GL2DLevelGenUtils::s_cohesionThreshold = 2;

///////////////////////////////////////////////////////////////////////////////

void GL2DLevelGenUtils::calcGalaxiesEscape( GL2DRoom* room, const Array< GL2DRoom* >& rooms, Point& outVelocity )
{
   outVelocity.set( 0, 0 );

   GL2DBounds roomBounds, otherRoomBounds;
   room->getBounds( roomBounds, 1 );


   Point centerPos;
   const uint count = rooms.size();
   uint includedNeighborsCount = 0;
   for ( uint i = 0; i < count; ++i )
   {
      GL2DRoom* otherRoom = rooms[i];

      otherRoom->getBounds( otherRoomBounds, 1 );
      otherRoomBounds.getCenter( centerPos );

      // cohesion should only kick in when the rooms are really far apart, and not touching with their sides
      uint distance = roomBounds.distanceTo( otherRoomBounds );
      if ( distance <= s_cohesionThreshold )
      {
         outVelocity += centerPos;
         ++includedNeighborsCount;
      }
   }

   if ( includedNeighborsCount > 1 )
   {
      outVelocity /= includedNeighborsCount;
   }

   roomBounds.getCenter( centerPos );
   outVelocity = centerPos - outVelocity;

   // normalize the velocity
   Vector v( ( float ) outVelocity.x, ( float ) outVelocity.y, 0 );
   if ( v.lengthSq() > Float_1e_3 )
   {
      v.normalize();
      outVelocity.x = ( int ) round( v[0] );
      outVelocity.y = ( int ) round( v[1] );
   }
}

///////////////////////////////////////////////////////////////////////////////

void GL2DLevelGenUtils::distributeRooms( const Array< GL2DRoom* >& rooms )
{
   const uint roomsCount = rooms.size();

   // set initial room positions so that the distribution algorithm may work properly
   int horizRoomsCount = (int)sqrt( roomsCount );
   int vertRoomsCount = roomsCount / horizRoomsCount + 1;

   uint roomIdx = 0;
   Point roomPos;
   for ( roomPos.y = 0; roomPos.y < vertRoomsCount && roomIdx < roomsCount; ++roomPos.y )
   {
      for ( roomPos.x = 0; roomPos.x < horizRoomsCount && roomIdx < roomsCount; ++roomPos.x, ++roomIdx )
      {
         GL2DRoom* room = rooms[roomIdx];
         room->setPosition( roomPos );
      }
   }


   // move the rooms away from the center until the don't overlap any more
   float relativeMovement = 1000000.0f;
   uint iterationsCount = 0;
   Point separationVel;
   while ( relativeMovement > 0.f )
   {
      // calculate the steering behaviors
      relativeMovement = 0.0f;
      for ( uint i = 0; i < roomsCount; ++i )
      {
         GL2DRoom* room = rooms[i];
         GL2DLevelGenUtils::calcGalaxiesEscape( room, rooms, separationVel );

         // move the room
         room->translate( separationVel );

         relativeMovement += separationVel.length();
      }
      relativeMovement /= ( float )roomsCount;
      ++iterationsCount;
   }
}

///////////////////////////////////////////////////////////////////////////////

void GL2DLevelGenUtils::calculateSceneBounds( const Array< GL2DRoom* >& rooms, GL2DBounds& outSceneBounds )
{
   const uint roomsCount = rooms.size();
   GL2DBounds roomBounds;
   for ( uint i = 0; i < roomsCount; ++i )
   {
      GL2DRoom* room = rooms[i];
      if ( room )
      {
         room->getBounds( roomBounds, 1 );
         outSceneBounds.add( roomBounds );
      }
   }

   // move all rooms so that the scene starts at the origin
   Point minBoundOffset, maxBoundOffset;
   outSceneBounds.get( minBoundOffset, maxBoundOffset );
   minBoundOffset *= -1;
   for ( uint i = 0; i < roomsCount; ++i )
   {
      GL2DRoom* room = rooms[i];
      if ( room )
      {
         room->translate( minBoundOffset );
      }
   }

   // update the scene bounds as well to reflect that change
   outSceneBounds.translate( minBoundOffset );
}

///////////////////////////////////////////////////////////////////////////////

static int roomGraphCostFunc( const Graph< GL2DRoom* >& graph, int start, int end )
{
   const GL2DRoom* startRoom = graph.getNode( start );
   const GL2DRoom* endRoom = graph.getNode( end );

   float dist = startRoom->m_pos.distanceTo( endRoom->m_pos );
   return ( int ) round( dist );
}

///////////////////////////////////////////////////////////////////////////////

void GL2DLevelGenUtils::generateCorridor( const Graph< GL2DRoom* >& roomsGraph, GL2DRoom* room1, GL2DRoom* room2, List< GL2DCorridor* >& outCorridors, List< GL2DRoom* >& outSequenceOfRooms )
{
  /* // run an A* search between two room that are farthest away from each other
   GraphSearchInfo< GL2DRoom* > searchParams;
   searchParams.m_start = roomsGraph.getNodeIdx( room1 );
   searchParams.m_end = roomsGraph.getNodeIdx( room2 );
   searchParams.m_costFunc = &roomGraphCostFunc;
   searchParams.m_distanceCostFunc = &roomGraphCostFunc;

   List< GL2DRoom* > sequenceOfRooms;
   GraphUtils< GL2DRoom* >::aStar( roomsGraph, searchParams, sequenceOfRooms );

   // generate connections between those rooms
   GL2DRoom* startRoom = NULL;
   Point corridorStartPos, corridorEndPos;
   for ( List< GL2DRoom* >::iterator it = sequenceOfRooms.begin(); !it.isEnd(); ++it )
   {
      GL2DRoom* endRoom = *it;
      outSequenceOfRooms.pushBack( endRoom );

      if ( startRoom )
      {
         outCorridors.pushBack( new GL2DCorridor( startRoom, endRoom ) );
      }

      startRoom = endRoom;
   }*/
}

///////////////////////////////////////////////////////////////////////////////

namespace
{
   struct RoomEntry
   {
      DECLARE_ALLOCATOR( RoomEntry, AM_DEFAULT );

      GL2DRoom*            m_room;
      uint                 m_depth;

      RoomEntry( GL2DRoom* room, uint depth )
         : m_room( room )
         , m_depth( depth )
      {}

   };
}

void GL2DLevelGenUtils::generateRandomSecondaryCorridors( const Graph< GL2DRoom* >& roomsGraph, const List< GL2DRoom* >& sequenceOfRooms, const GL2DProceduralLevel& params, List< GL2DCorridor >& outCorridors )
{
 /*  // now let's broaden our path to the specified level - so using the connectivity graph,
   // let's add new rooms that connect to each of the room on our critical path,
   // and select some random connection between them up until the specified depth   
   for ( List< GL2DRoom* >::const_iterator it = sequenceOfRooms.begin(); !it.isEnd(); ++it )
   {
      GL2DRoom* startRoom = *it;

      LocalList< RoomEntry > roomsToAnalyze;
      roomsToAnalyze.pushBack( RoomEntry( startRoom, 0 ) );
      while ( !roomsToAnalyze.empty() )
      {
         const RoomEntry& entry = roomsToAnalyze.front();

         // and, if we haven't reached the maximum depth of the spanning tree yet,
         // add for further analysis
         if ( entry.m_depth >= params.m_maxSideCorridorsDepth )
         {
            roomsToAnalyze.popFront();
            continue;
         }

         int nodeIdx = roomsGraph.getNodeIdx( entry.m_room );
         const Graph< GL2DRoom* >::EdgeIndices& edges = roomsGraph.getEdges( nodeIdx );

         const uint edgesCount = edges.size();
         uint childrenExpanded = 0;
         for ( uint edgeIdx = 0; edgeIdx < edgesCount && childrenExpanded < params.m_maxExpandedChildrenCount; ++edgeIdx )
         {
            const int nextRoomIdx = roomsGraph.getEdge( edges[edgeIdx] );
            GL2DRoom* endRoom = roomsGraph.getNode( nextRoomIdx );

            if ( entry.m_room->isConnectedTo( endRoom ) == false )
            {
               // connect the rooms if they're not connected
               outCorridors.pushBack( GL2DCorridor( startRoom, endRoom ) );

               ++childrenExpanded;

               roomsToAnalyze.pushBack( RoomEntry( endRoom, entry.m_depth + 1 ) );
            }
         }

         // remove the analyzed entry
         roomsToAnalyze.popFront();
      }
   }*/
}

///////////////////////////////////////////////////////////////////////////////

void GL2DLevelGenUtils::buildGraphOfRooms( const Array< GL2DRoom* >& rooms, Graph< GL2DRoom* >& outRoomsGraph )
{
   // add all room indices
   const uint roomsCount = rooms.size();
   for ( uint i = 0; i < roomsCount; ++i )
   {
      outRoomsGraph.addNode( rooms[i] );
   }

   // connect rooms into a rough graph, where every room establishes 4 connections with its neighbors.
   // That should be enough to connect every room in a mesh
   Point exit1, exit2;
   GL2DBounds bounds;
   BestResultsList< float, uint > exitsList;
   const uint exitsCount = 4;
   for ( uint i = 0; i < roomsCount; ++i )
   {
      GL2DRoom* room = rooms[i];

      // how many exist do we want to generate ( a room may have between 1-4 exist, no more, no less )
      exitsList.init( exitsCount );

      for ( uint j = 0; j < roomsCount; ++j )
      {
         if ( i == j )
         {
            // that's the same room as the analyzed one
            continue;
         }

         GL2DRoom* otherRoom = rooms[j];
         const float dist = room->getNearestExits( otherRoom, exit1, exit2 );
         exitsList.insert( dist, j );
      }

      // connect the rooms
      for ( BestResultsList< float, uint >::const_iterator it = exitsList.begin(); !it.isEnd(); ++it )
      {
         uint endRoomIdx = *it;
         outRoomsGraph.connect( i, endRoomIdx );
         outRoomsGraph.connect( endRoomIdx, i );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void GL2DLevelGenUtils::removeUnconnectedRooms( Array< GL2DRoom* >& rooms )
{
   const uint count = rooms.size();
   LocalList< GL2DRoom* > connectedRooms;
   for ( uint i = 0; i < count; ++i )
   {
      GL2DRoom* room = rooms[i];
      if ( room->getConnectedRoomsCount() > 0 )
      {
         connectedRooms.pushBack( room );
      }
   }

   rooms.clear();
   rooms.resize( connectedRooms.size() );
   uint roomIdx = 0;
   for ( LocalList< GL2DRoom* >::iterator it = connectedRooms.begin(); !it.isEnd(); ++it, ++roomIdx )
   {
      GL2DRoom* room = *it;
      rooms[roomIdx] = room;
   }
}

///////////////////////////////////////////////////////////////////////////////

void GL2DLevelGenUtils::getBoundsAroundOccupiedSpace( const Grid< uint >& grid, GL2DBounds& outSceneBounds )
{
   const int w = grid.width();
   const int h = grid.height();
   
   Point minPt, maxPt, tmpPos;

   // left margin
   bool allHoles = true;
   for ( minPt.x = 0; minPt.x < w && allHoles; ++minPt.x )
   {
      tmpPos.x = minPt.x;
      for ( tmpPos.y = 0; tmpPos.y < h; ++tmpPos.y )
      {
         if ( grid.getValue( tmpPos ) != NULL )
         {
            allHoles = false;
            --minPt.x;
            break;
         }
      }
   }

   // right margin
   allHoles = true;
   for ( maxPt.x = w - 1; maxPt.x >= 0 && allHoles; --maxPt.x )
   {
      tmpPos.x = maxPt.x;
      for ( tmpPos.y = 0; tmpPos.y < h; ++tmpPos.y )
      {
         if ( grid.getValue( tmpPos ) != NULL )
         {
            allHoles = false;
            ++maxPt.x;
            break;
         }
      }
   }

   // top margin
   allHoles = true;
   for ( minPt.y = 0; minPt.y < h && allHoles; ++minPt.y )
   {
      tmpPos.y = minPt.y;
      for ( tmpPos.x = 0; tmpPos.x < w; ++tmpPos.x )
      {
         if ( grid.getValue( tmpPos ) != NULL )
         {
            allHoles = false;
            --minPt.y;
            break;
         }
      }
   }

   // right margin
   allHoles = true;
   for ( maxPt.y = h - 1; maxPt.y >= 0 && allHoles; --maxPt.y )
   {
      tmpPos.y = maxPt.y;
      for ( tmpPos.x = 0; tmpPos.x < w; ++tmpPos.x )
      {
         if ( grid.getValue( tmpPos ) != NULL )
         {
            allHoles = false;
            ++maxPt.y;
            break;
         }
      }
   }

   // set the new bounds
   outSceneBounds.set( minPt, maxPt );
}

///////////////////////////////////////////////////////////////////////////////

