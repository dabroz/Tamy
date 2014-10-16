/// @file   ext-2DGameLevel\GL2DWallVoxel.h
/// @brief  wall voxel definition
#pragma once

#include "core\MemoryRouter.h"
#include "ext-2DGameLevel\GL2DLevelVoxels.h"
#include <string>


///////////////////////////////////////////////////////////////////////////////

class GL2DRoomTheme;

///////////////////////////////////////////////////////////////////////////////

struct GL2DWallVoxel
{
   DECLARE_ALLOCATOR( GL2DWallVoxel, AM_DEFAULT );

   const GL2DRoomTheme*       m_theme;
   WallsVoxelizationIndices   m_tileId;

   GL2DWallVoxel()
      : m_theme( NULL )
      , m_tileId( LL_HOLE_ID )
   {
   }

   inline void set( const GL2DRoomTheme* theme, WallsVoxelizationIndices tileId )
   {
      m_theme = theme;
      m_tileId = tileId;
   }

   /**
    * Converts the voxel to a textual form.
    *
    * @para voxel
    * @param outStr
    */
   friend void toString( const GL2DWallVoxel& voxel, std::string& outStr );
};

///////////////////////////////////////////////////////////////////////////////
