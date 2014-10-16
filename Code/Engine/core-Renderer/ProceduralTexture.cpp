#include "core-Renderer\ProceduralTexture.h"
#include "core-Renderer\RenderResource.h"
#include "core\IDString.h"


///////////////////////////////////////////////////////////////////////////////

uint g_dataSizePerFormat[] = {
   sizeof( PTVector ),
   sizeof( float ) * 4,
};

///////////////////////////////////////////////////////////////////////////////

ProceduralTexture::ProceduralTexture( uint width, uint height, ProceduralTextureFormat format, byte mipmapsCount )
   : m_width( width )
   , m_height( height )
   , m_mipmapsCount( mipmapsCount )
   , m_format( format )
   , m_imageBuf( width * height * 3 )
{
   m_imageBuf.resize( width * height * g_dataSizePerFormat[format], 0 );

   m_renderResource = RenderResource::instantiate< Texture >( this );
}

///////////////////////////////////////////////////////////////////////////////

ProceduralTexture::~ProceduralTexture()
{
   m_renderResource->removeReference();
   m_renderResource = NULL;
}

///////////////////////////////////////////////////////////////////////////////

ShaderParam* ProceduralTexture::createShaderTextureSetter( MemoryAllocator* allocator, const IDString& paramName, const TextureSamplerSettings& samplerSettings, byte renderTextureIdx )
{
   return new ( allocator ) ShaderParamProceduralTexture( paramName.getId(), this, samplerSettings );
}

///////////////////////////////////////////////////////////////////////////////

byte* ProceduralTexture::accessImageBuffer()
{
   byte* imageBuf = ( byte* ) m_imageBuf;
   m_renderResource->setDirty();

   return imageBuf;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void PTVector::fromVectorAsNormal( const Vector& vec )
{
   r = ( byte ) clamp<float>( vec[0] * 128.0f + 128.0f, 0.0f, 255.0f );
   g = ( byte ) clamp<float>( vec[1] * 128.0f + 128.0f, 0.0f, 255.0f );
   b = ( byte ) clamp<float>( vec[2] * 128.0f + 128.0f, 0.0f, 255.0f );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void PTFloat::fromFloat( float val )
{
   r = val;
   g = val;
   b = val;
   a = val;
}

///////////////////////////////////////////////////////////////////////////////
