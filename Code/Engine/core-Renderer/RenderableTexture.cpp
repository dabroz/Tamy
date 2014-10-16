#include "core-Renderer\RenderableTexture.h"
#include "core-Renderer\Texture.h"
#include "core\IDString.h"


///////////////////////////////////////////////////////////////////////////////

RenderableTexture::RenderableTexture()
    : m_texture( NULL )
{
}

///////////////////////////////////////////////////////////////////////////////

RenderableTexture::~RenderableTexture()
{
   if ( m_texture )
   {
      m_texture->removeReference();
      m_texture = NULL;
   }
}

///////////////////////////////////////////////////////////////////////////////

void RenderableTexture::setTexture( Texture* texture )
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

unsigned int RenderableTexture::getWidth() const
{
   return m_texture ? m_texture->getWidth() : 0;
}

///////////////////////////////////////////////////////////////////////////////

unsigned int RenderableTexture::getHeight() const
{
   return m_texture ? m_texture->getHeight() : 0;
}

///////////////////////////////////////////////////////////////////////////////

ShaderParam* RenderableTexture::createShaderTextureSetter( MemoryAllocator* allocator, const IDString& paramName, const TextureSamplerSettings& samplerSettings, byte renderTextureIdx )
{
   return new ( allocator ) ShaderParamTexture( paramName.getId(), m_texture, samplerSettings );
}

///////////////////////////////////////////////////////////////////////////////
