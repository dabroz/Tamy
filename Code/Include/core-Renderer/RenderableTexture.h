/// @file   core-Renderer\RenderableTexture.h
/// @brief  An adapter that allows to plug a texture image into a shader
#pragma once

#include "core-Renderer\ShaderTexture.h"


///////////////////////////////////////////////////////////////////////////////

/**
 * An adapter that allows to plug a texture image into a shader.
 */
class RenderableTexture : public ShaderTexture
{
   DECLARE_ALLOCATOR( RenderableTexture, AM_DEFAULT );

private:
   Texture*    m_texture;

public:
   /**
    * Constructor.
    */
   RenderableTexture();
   ~RenderableTexture();

   /**
    * Sets a new texture the adapter should proxy the calls to.
    */
   void setTexture( Texture* texture );

   // -------------------------------------------------------------------------
   // ShaderTexture implementation
   // -------------------------------------------------------------------------
   uint getWidth() const override;
   uint getHeight() const override;
   ShaderParam* createShaderTextureSetter( MemoryAllocator* allocator, const IDString& paramName, const TextureSamplerSettings& samplerSettings, byte renderTextureIdx ) override;
};

///////////////////////////////////////////////////////////////////////////////
