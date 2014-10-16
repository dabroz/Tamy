/// @file   ext-2DGameLevel\GL2DLevelVoxels.h
/// @brief  all voxel types used to build a level
#pragma once

#include "core\Quaternion.h"


///////////////////////////////////////////////////////////////////////////////

enum PathfindingVoxelIdx
{
   PF_HOLE_ID,
   PF_CORRIDOR_FLOOR_ID,
   PF_ROOM_FLOOR_ID,
};

///////////////////////////////////////////////////////////////////////////////

/**
 * The values of these voxels are tightly coupled with the weighting
 * algorithm used in v GL2DLevelGenUtils::voxelizeWalls.
 *
 * Do not change them!!~!
 */
enum WallsVoxelizationIndices
{
   LL_HOLE_ID = 0,
   LL_FLOOR_ID = 1,
   LL_WALL_ID = 2,
};

///////////////////////////////////////////////////////////////////////////////

enum GL2DVoxelRotation
{
   VRot_0,
   VRot_90,
   VRot_180,
   VRot_270,
};

///////////////////////////////////////////////////////////////////////////////
