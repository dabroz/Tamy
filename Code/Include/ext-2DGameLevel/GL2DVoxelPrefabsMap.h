/// @file   ext-2DGameLevel\GL2DVoxelPrefabsMap.h
/// @brief  a map of prefabs used to populate the voxelized level
#pragma once

#include "core\MemoryRouter.h"
#include "core\Array.h"
#include <string>


///////////////////////////////////////////////////////////////////////////////

class FilePath;
class Prefab;
class GL2DRoomTheme;

///////////////////////////////////////////////////////////////////////////////

class GL2DVoxelPrefabsMap
{
   DECLARE_ALLOCATOR( GL2DVoxelPrefabsMap, AM_DEFAULT );

private:
   uint                          m_rootDirPathOffset;
   Array< GL2DRoomTheme* >       m_themes;

public:
   /**
    * Constructor.
    *
    * @param prefabsRootDir
    */
   GL2DVoxelPrefabsMap( const FilePath& prefabsRootDir );
   ~GL2DVoxelPrefabsMap();
   
   // -------------------------------------------------------------------------
   // Room types
   // -------------------------------------------------------------------------

   /**
    * Returns an existing theme.
    *
    * @param themeName
    */
   GL2DRoomTheme* findTheme( const std::string& themeName );

   /**
    * Adds a new theme.
    *
    * @param themeName
    */
   GL2DRoomTheme* addTheme( const std::string& themeName );

   /**
    * Returns a theme selected at random.
    */
   GL2DRoomTheme* getRandomTheme() const;
};

///////////////////////////////////////////////////////////////////////////////
