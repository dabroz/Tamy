#include "core-Renderer\RenderableCubeTexture.h"
#include "core-Renderer\CubeTexture.h"
#include "core\IDString.h"


///////////////////////////////////////////////////////////////////////////////

RenderableCubeTexture::RenderableCubeTexture()
   : m_texture( NULL )
{
}

///////////////////////////////////////////////////////////////////////////////

RenderableCubeTexture::~RenderableCubeTexture()
{
   if ( m_texture )
   {
      m_texture->removeReference();
      m_texture = NULL;
   }
}

///////////////////////////////////////////////////////////////////////////////

void RenderableCubeTexture::setTexture( CubeTexture* texture )
{
   if ( m_texture )
   {
      m_texture->removeReference();
   }

   m_texture = texture;

   if ( m_texture )
   {
      m_texture->addReference();
   }
}

///////////////////////////////////////////////////////////////////////////////

uint RenderableCubeTexture::getWidth() const
{
   return m_texture ? m_texture->getEdgeLength() : 0;
}

///////////////////////////////////////////////////////////////////////////////

uint RenderableCubeTexture::getHeight() const
{
   return m_texture ? m_texture->getEdgeLength() : 0;
}

///////////////////////////////////////////////////////////////////////////////

ShaderParam* RenderableCubeTexture::createShaderTextureSetter( MemoryAllocator* allocator, const IDString& paramName, const TextureSamplerSettings& samplerSettings, byte renderTextureIdx )
{
   return new ( allocator ) ShaderParamCubeTexture( paramName.getId(), m_texture, samplerSettings );
}

///////////////////////////////////////////////////////////////////////////////
