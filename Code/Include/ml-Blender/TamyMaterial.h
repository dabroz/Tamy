/// @file   ml-Blender/TamyMaterial.h
/// @brief  Tamy material export-related proxies
#pragma once

#include "core/Color.h"


///////////////////////////////////////////////////////////////////////////////

struct TamyTexture
{
   const char*    path;
};

///////////////////////////////////////////////////////////////////////////////

struct TamyMaterial
{
   // can't be const, because we're inheriting MaterialDefinition from this class
   // and copying this string from a TamyMaterial instance to a MaterialDefinition instance,
   // and for that we need non-const access to this field.
   //
   // The inheritance is explained next to the MaterialDefinition struct definition.
   char*          name;

   Color          ambientColor;
   Color          diffuseColor;
   Color          specularColor;

   int            normalTextureIndex;
   int            specularTextureIndex;

   int*           diffuseTexturesIndices;
   int*           diffuseTexturesOperations;
   float*         diffuseTexturesOperationFactors;
   int            diffuseTexturesCount;
};

///////////////////////////////////////////////////////////////////////////////
