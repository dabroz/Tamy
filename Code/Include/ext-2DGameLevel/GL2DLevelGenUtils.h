/// @file   ext-2DGameLevel\GL2DLevelGenUtils.h
/// @brief  voxel based level generation utility
#pragma once

#include "core\MemoryRouter.h"
#include "core\Array.h"
#include "core\List.h"
#include "core\Point.h"
#include "core\Grid.h"
#include "core\Graph.h"


///////////////////////////////////////////////////////////////////////////////

class GL2DBounds;
class GL2DRoom;
class GL2DProceduralLevel;
class GL2DVoxelPrefabsMap;
class Prefab;
class FilePath;
class Entity;
class GL2DCorridor;
struct GL2DWallVoxel;
class GL2DVoxelizedItem;

///////////////////////////////////////////////////////////////////////////////

class GL2DLevelGenUtils
{
public:
   static uint s_cohesionThreshold;

public:
   /**
    * Distributes the rooms randomly on the level
    *
    * @param rooms
    */
   static void distributeRooms( const Array< GL2DRoom* >& rooms );

   /**
    * Calculates the bounds around the rooms.
    *
    * @param rooms
    * @param outSceneBounds
    */
   static void calculateSceneBounds( const Array< GL2DRoom* >& rooms, GL2DBounds& outSceneBounds );

   /**
    * Creates a graph of rooms that can be used to find paths connecting them.
    *
    * @param rooms
    * @param outRoomsGraph
    */
   static void buildGraphOfRooms( const Array< GL2DRoom* >& rooms, Graph< GL2DRoom* >& outRoomsGraph );

   /**
    * Connects specified rooms with corridors on a 'which room is closest to which' basis.
    *
    * @param roomsGraph
    * @param room1
    * @param room2
    * @param outCorridors
    * @param outSequenceOfRooms
    */
   static void generateCorridor( const Graph< GL2DRoom* >& roomsGraph, GL2DRoom* room1, GL2DRoom* room2, List< GL2DCorridor* >& outCorridors, List< GL2DRoom* >& outSequenceOfRooms );

   /**
    * Generates random corridors between the specified rooms and rooms that surround them
    *
    * @param roomsGraph
    * @param sequenceOfRooms
    * @param params
    * @param outCorridors
    */
   static void generateRandomSecondaryCorridors( const Graph< GL2DRoom* >& roomsGraph, const List< GL2DRoom* >& sequenceOfRooms, const GL2DProceduralLevel& params, List< GL2DCorridor >& outCorridors );

   /**
    * Removes unconnected rooms.
    *
    * @param rooms
    */
   static void removeUnconnectedRooms( Array< GL2DRoom* >& rooms );

   /**
    * Calculates the tightest bounds around the non-hole voxels located in the grid.
    *
    * @param grid
    * @param outSceneBounds
    */
   static void getBoundsAroundOccupiedSpace( const Grid< uint >& grid, GL2DBounds& outSceneBounds );

protected:
   static void calcGalaxiesEscape( GL2DRoom* room, const Array< GL2DRoom* >& rooms, Point& outVelocity );
};

///////////////////////////////////////////////////////////////////////////////
