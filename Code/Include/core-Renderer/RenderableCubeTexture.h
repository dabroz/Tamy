/// @file   core-Renderer\RenderableCubeTexture.h
/// @brief  An adapter that allows to plug a cube texture image into a shader
#pragma once

#include "core-Renderer\ShaderTexture.h"


///////////////////////////////////////////////////////////////////////////////

class CubeTexture;

///////////////////////////////////////////////////////////////////////////////

/**
* An adapter that allows to plug a cube texture image into a shader.
*/
class RenderableCubeTexture : public ShaderTexture
{
   DECLARE_ALLOCATOR( RenderableCubeTexture, AM_DEFAULT );

private:
   CubeTexture*    m_texture;

public:
   /**
    * Constructor.
    */
   RenderableCubeTexture();
   ~RenderableCubeTexture();

   /**
   * Sets a new texture the adapter should proxy the calls to.
   */
   void setTexture( CubeTexture* texture );

   // -------------------------------------------------------------------------
   // ShaderTexture implementation
   // -------------------------------------------------------------------------
   uint getWidth() const override;
   uint getHeight() const override;
   ShaderParam* createShaderTextureSetter( MemoryAllocator* allocator, const IDString& paramName, const TextureSamplerSettings& samplerSettings, byte renderTextureIdx ) override;
};

///////////////////////////////////////////////////////////////////////////////
