/// @file   ext-2DGameLevel\GL2DVoxelPrefabsMap.h
/// @brief  a map of prefabs used to populate the voxelized level
#pragma once

#include "core\MemoryRouter.h"
#include "core\Array.h"
#include <string>


///////////////////////////////////////////////////////////////////////////////

class FilePath;
class Prefab;

///////////////////////////////////////////////////////////////////////////////

class GL2DVoxelPrefabsMap
{
   DECLARE_ALLOCATOR( GL2DVoxelPrefabsMap, AM_DEFAULT );

private:
   Array< Prefab* >              m_prefabs;

public:
   /**
    * Constructor.
    *
    * @param prefabsRootDir
    */
   GL2DVoxelPrefabsMap( const FilePath& prefabsRootDir );
   ~GL2DVoxelPrefabsMap();
   
   /**
    * Adds a new prefab.
    *
    * @param prefab
    */
   void addPrefab( Prefab* prefab );

   /**
    * Returns the number of stored prefabs.
    */
   inline const uint getPrefabsCount() const
   {
      return m_prefabs.size();
   }

   /**
    * Returns the specified prefab.
    */
   inline const Prefab* getPrefab( uint idx ) const
   {
      return m_prefabs[idx];
   }

};

///////////////////////////////////////////////////////////////////////////////
