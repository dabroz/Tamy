/// @file   ext-2DGameLevel\GL2DLevelGenerator.h
/// @brief  the level generator
#pragma once

#include "core\MemoryRouter.h"


///////////////////////////////////////////////////////////////////////////////

class GL2DVoxelPrefabsMap;
class FilePath;
class Entity;

///////////////////////////////////////////////////////////////////////////////

class GL2DLevelGenerator
{
   DECLARE_ALLOCATOR( GL2DLevelGenerator, AM_DEFAULT );

private:
   const FilePath&                  m_geometryDeploymentDir;
   const GL2DVoxelPrefabsMap&       m_prefabsMap;

public:
   /**
    * Constructor.
    *
    * @param geometryDeploymentDir
    * @param prefabsMap
    */
   GL2DLevelGenerator( const FilePath& geometryDeploymentDir, const GL2DVoxelPrefabsMap& prefabsMap );
   ~GL2DLevelGenerator();

   /**
    * Generates a level.
    *
    * @param levelLength
    */
   Entity* generateLevel( float levelLength );
};

///////////////////////////////////////////////////////////////////////////////
