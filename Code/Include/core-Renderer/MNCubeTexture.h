/// @file   core-Renderer\MNCubeTexture.h
/// @brief  a node that provides a cube texture
#pragma once

#include "core-Renderer\MaterialNode.h"
#include "core-Renderer\TextureSamplerSettings.h"


///////////////////////////////////////////////////////////////////////////////

class MSVec3Input;
class MSColorOutput;
class CubeTexture;
class RenderableCubeTexture;

///////////////////////////////////////////////////////////////////////////////

class MNCubeTexture : public MaterialNode
{
   DECLARE_ALLOCATOR( MNCubeTexture, AM_ALIGNED_16 );
   DECLARE_CLASS()

private:
   // static data
   CubeTexture*               m_texture;
   TextureSamplerSettings     m_samplerSettings;

   // runtime data
   MSVec3Input*               m_uvwInput;
   MSColorOutput*             m_output;
   RenderableCubeTexture*     m_renderableTexture;

   std::string                var_samplerName;

public:
   /**
   * Constructor.
   *
   * @param name
   * @param cubeTexture
   */
   MNCubeTexture( const char* name = "MNCubeTexture", CubeTexture* cubeTexture = NULL );

   /**
   * Copy constructor.
   */
   MNCubeTexture( const MNCubeTexture& rhs );
   ~MNCubeTexture();

   // -------------------------------------------------------------------------
   // Object implementation
   // -------------------------------------------------------------------------
   void onObjectLoaded() override;
   void onPropertyChanged( ReflectionProperty& property ) override;

   // -------------------------------------------------------------------------
   // MaterialNode implementation
   // -------------------------------------------------------------------------
   void buildMaterialShader( MaterialShaderBuilder& builder ) override;
   void render( Renderer& renderer, const RenderState& host, RCBindShader* shaderComm, ShaderDataBuffer* constantsBuf ) const override;
};

///////////////////////////////////////////////////////////////////////////////
