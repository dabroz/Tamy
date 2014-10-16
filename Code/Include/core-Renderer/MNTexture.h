/// @file   core-Renderer/MNTexture.h
/// @brief  a node that provides a texture
#pragma once

#include "core-Renderer\MaterialNode.h"
#include "core-Renderer\TextureSamplerSettings.h"


///////////////////////////////////////////////////////////////////////////////

class MSVec2Input;
class MSColorOutput;
class Texture;
class RenderableTexture;

///////////////////////////////////////////////////////////////////////////////

class MNTexture : public MaterialNode
{
   DECLARE_ALLOCATOR( MNTexture, AM_ALIGNED_16 );
   DECLARE_CLASS()

private:
   // static data
   Texture*                   m_texture;
   TextureSamplerSettings     m_samplerSettings;

   // runtime data
   MSVec2Input*               m_uvInput;
   MSColorOutput*             m_output;
   RenderableTexture*         m_renderableTexture;

   std::string                var_samplerName;

public:
   /**
    * Constructor.
    *
    * @param name
    * @param texture
    */
   MNTexture( const char* name = "MNTexture", Texture* texture = NULL );

   /**
    * Copy constructor.
    */
   MNTexture( const MNTexture& rhs );
   ~MNTexture();

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
