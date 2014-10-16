/// @file   ml-Blender\BlenderMaterialBuilder.h
/// @brief  a material builder utility for the Blender exporter
#pragma once

#include "core\GraphBuilderTransaction.h"
#include "core\Vector.h"
#include <vector>


///////////////////////////////////////////////////////////////////////////////

struct TamyMaterial;
class Material;
class MaterialProfile;
class MaterialNode;
class Texture;
class BMNCompositeItem;

///////////////////////////////////////////////////////////////////////////////

class BlenderMaterialBuilder
{
private:
   const std::vector< Texture* >&            m_textures;
   const TamyMaterial*                       m_exportedMaterial;

public:
   /**
    * Constructor.
    *
    * @param textures
    */
   BlenderMaterialBuilder( const std::vector< Texture* >& textures );

   /**
    * Builds a material.
    *
    * @param exportedMaterial
    * @param outMaterial
    */
   void buildMaterial( const TamyMaterial& exportedMaterial, Material* outMaterial );

private:
   void setupDefaultProfile( MaterialProfile* outMaterialProfile ) const;

   BMNCompositeItem* buildDiffuseColorProvider( MaterialNode* inputStruct, MaterialNode* output, GraphBuilderTransaction< MaterialNode >& outTransaction ) const;
   
   BMNCompositeItem* buildSpecularColorProvider( MaterialNode* inputStruct, MaterialNode* output, GraphBuilderTransaction< MaterialNode >& outTransaction ) const;
   
   BMNCompositeItem* buildNormalMapProvider( MaterialNode* inputStruct, MaterialNode* lighting, GraphBuilderTransaction< MaterialNode >& outTransaction ) const;

};

///////////////////////////////////////////////////////////////////////////////
