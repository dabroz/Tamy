/// @file   ml-Blender/TamyAnimation.h
/// @brief  helper structures used to export animation data
#pragma once

#include "ml-Blender\TamyScene.h"


///////////////////////////////////////////////////////////////////////////////

struct TamyBone
{
   const char*                name;

   TamyMatrix                 modelSpaceTransform;
   int                        parentBoneIdx;

   float                      boneLength;
};

///////////////////////////////////////////////////////////////////////////////

struct TamySkeleton
{
   const char*                name;

   TamyBone*                  bones;
   int                        bonesCount;
};

///////////////////////////////////////////////////////////////////////////////

struct TamyAnimationTrack
{   
   char*                      boneName;
   int                        trackIdx;
   int                        framesCount;
   TamyTransform*             frameKeys;
};

///////////////////////////////////////////////////////////////////////////////

struct TamyAnimation
{
   const char*                name;

   int                        skeletonIdx;
   int                        animatedEntityIdx;

   float                      playbackSpeed;
   int                        framesCount;

   int                        poseTracksCount;
   TamyAnimationTrack*        poseTracks;
   TamyAnimationTrack         motionTrack;
};

///////////////////////////////////////////////////////////////////////////////
