/// @file   ext-2DGameLevel\GL2DLevelGenerator.h
/// @brief  the level generator
#pragma once

#include "core\MemoryRouter.h"
#include "core\List.h"
#include "core\Array.h"
#include "core\Grid.h"


///////////////////////////////////////////////////////////////////////////////

class GL2DCorridor;
class GL2DRoom;
class GL2DProceduralLevel;
class GL2DVoxelizedItem;
class GL2DVoxelPrefabsMap;
class Entity;
class Prefab;
class GL2DRoomTheme;
class GL2DRoomLayout;

///////////////////////////////////////////////////////////////////////////////

enum GL2DDistanceHint
{
   GL2D_DH_Near,
   GL2D_DH_Far,
};

///////////////////////////////////////////////////////////////////////////////

class GL2DLevelGenerator
{
   DECLARE_ALLOCATOR( GL2DLevelGenerator, AM_DEFAULT );

public:
   GL2DVoxelPrefabsMap&                m_prefabsMap;
   GL2DProceduralLevel&                m_params;

   // pre-baking data
   Array< GL2DRoom* >                  m_rooms;
   List< GL2DCorridor* >               m_corridors;
   List< GL2DRoomLayout* >             m_layouts;

public:
   /**
    * Constructor.
    *
    * @param prefabsMap
    * @param params
    */
   GL2DLevelGenerator( GL2DVoxelPrefabsMap& prefabsMap, GL2DProceduralLevel& params );
   ~GL2DLevelGenerator();

   /**
    * Adds a predefined room.
    *
    * @param room
    */
   void addRoom( GL2DRoom* room );

   /**
    * Adds a room layout.
    *
    * @param layout
    */
   void addLayout( GL2DRoomLayout* layout );

   /**
    * Adds a corridor.
    *
    * @param corridor
    */
   void addCorridor( GL2DCorridor* corridor );

   /**
    * Resets the generation process.
    */
   void reset();

   /**
    * Renders the level to an entity.
    */
   Entity* renderLevel();
};

///////////////////////////////////////////////////////////////////////////////
