/// @file   ext-2DGameLevel\GL2DLevelGenerator.h
/// @brief  the level generator
#pragma once

#include "core\MemoryRouter.h"


///////////////////////////////////////////////////////////////////////////////

class FilePath;
class Entity;

///////////////////////////////////////////////////////////////////////////////

class GL2DLevelGenerator
{
   DECLARE_ALLOCATOR( GL2DLevelGenerator, AM_DEFAULT );

private:
   const FilePath&                  m_lSystemDir;
   const FilePath&                  m_geometryDeploymentDir;

public:
   /**
    * Constructor.
    *
    * @param lSystemDir
    * @param prefabsMap
    */
   GL2DLevelGenerator( const FilePath& lSystemDir, const FilePath& geometryDeploymentDir );
   ~GL2DLevelGenerator();

   /**
    * Generates a level.
    *
    * @param levelLength
    */
   Entity* generateLevel( float levelLength );
};

///////////////////////////////////////////////////////////////////////////////
