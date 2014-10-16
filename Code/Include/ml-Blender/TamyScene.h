/// @file   ml-Blender/TamyScene.h
/// @brief  Tamy scene export-related proxies
#pragma once

#include "ml-Blender\TamyMesh.h"
#include "ml-Blender\BaseExportStructs.h"
#include "core\Matrix.h"
#include "core\Transform.h"
#include "core\Vector.h"
#include "core\Color.h"
#include "core\Quaternion.h"


///////////////////////////////////////////////////////////////////////////////

class Entity;

///////////////////////////////////////////////////////////////////////////////

struct TamyMatrix
{
   ExportMatrix         rotationMtx;
   ExportVector         position;

   void applyTo( Matrix& outTransform ) const;
};

///////////////////////////////////////////////////////////////////////////////

struct TamyTransform
{
   ExportVector         translation;   // ( x, y, z, 1 )
   ExportVector         rotation;      // ( x, y, z, w )

   void applyTo( Transform& outTransform ) const;
};

///////////////////////////////////////////////////////////////////////////////

/**
 * A class that represents a basic Entity
 */
struct TamyEntity
{
   const char*          name;

   TamyMatrix           localMatrix;
   int                  parentIdx;
};

///////////////////////////////////////////////////////////////////////////////

/**
 * This structure represents a GeometryComponent entity
 */
struct TamyGeometry
{
   const char*          name;

   TamyMatrix           localMatrix;
   int                  parentIdx;

   TamyMesh             mesh;
};

///////////////////////////////////////////////////////////////////////////////

/**
 * This structure represents a Light entity
 */
struct TamyLight
{
   const char*          name;

   int                  type;                         // @see TamyLightType enum
   
   Color                lightColor;
   float                energy;
   float                distance;
   bool                 useSpecular;

   bool                 castShadows;
   Color                shadowColor;

   float                linearAttenuation;
   float                quadraticAttenuation;

   int                  falloffType;                  // @see LightFalloffType

   float                spotAttenuation;
   float                spotAngle;
   bool                 spotlightCastsOnlyShadows;
     
   TamyMatrix           localMatrix;
   int                  parentIdx;
};

///////////////////////////////////////////////////////////////////////////////

enum TamyLightType
{
   LT_Directional = 0,
   LT_Point = 1,
   LT_Spotlight = 2,
   LT_Hemisphere = 3,
   LT_Area = 4,
};

///////////////////////////////////////////////////////////////////////////////

enum LightFalloffType
{
   FOT_Constant = 0,
   FOT_InverseLinear = 1,
   FOT_InverseSquare = 2
};


///////////////////////////////////////////////////////////////////////////////

/**
 * This structure represents a SkeletonComponent attached to an entity
 */
struct TamySkeletonComponent
{
   const char*          name;

   TamyMatrix           localMatrix;
   int                  parentIdx;

   int                  skeletonIdx;
};

///////////////////////////////////////////////////////////////////////////////

/**
 * This structure holds global rendering settings.
 */
struct TamyWorld
{
   Color                ambientLightColor;
};

///////////////////////////////////////////////////////////////////////////////
