#include "core-Renderer\ShaderRenderCommand.h"
#include "core-Renderer\ShaderParam.h"
#include "core-Renderer\ShaderTexture.h"
#include "core-Renderer\RenderTarget2D.h"
#include "core-Renderer\RenderTargetCube.h"
#include "core\IDString.h"


///////////////////////////////////////////////////////////////////////////////

ShaderRenderCommand::ShaderRenderCommand( MemoryAllocator& allocator )
   : m_allocator( &allocator )
   , m_root( new ShaderParam() )
   , m_tail( m_root )
{
}

///////////////////////////////////////////////////////////////////////////////

ShaderRenderCommand::~ShaderRenderCommand()
{
   ShaderParam* toDelete;
   for ( ShaderParam* it = m_root; it != NULL; )
   {
      toDelete = it;
      it = it->m_nextParam;
      delete toDelete;
   }
}

///////////////////////////////////////////////////////////////////////////////

void ShaderRenderCommand::setParams( Renderer& renderer, void* shaderImpl )
{
   for ( ShaderParam* it = m_root; it != NULL; it = it->m_nextParam)
   {
      it->setParam( renderer, shaderImpl );
   }
}

///////////////////////////////////////////////////////////////////////////////

void ShaderRenderCommand::setTexture( const IDString& paramName, ShaderTexture* texture, const TextureSamplerSettings& samplerSettings, byte renderTextureIdx )
{
   // we're using the good old Visitor pattern here to get around that we can't 
   // have template virtual methods - so we'll make our resources introduce
   // themselves to the ShaderTexture by calling a proper method on it,
   // and it will be the shader texture's responsibility to instantiate
   // a proper param setter.
   if ( texture )
   {
      ShaderParam* newParam = texture->createShaderTextureSetter( m_allocator, paramName, samplerSettings, renderTextureIdx );
      m_tail->m_nextParam = newParam;
      m_tail = newParam;
   }
}

///////////////////////////////////////////////////////////////////////////////

void ShaderRenderCommand::setDepthBuffer( const IDString& paramName, RenderTarget2D* renderTarget )
{
   ShaderParam* newParam = new ( m_allocator ) ShaderParamDepthBuffer2D( paramName, renderTarget );
   m_tail->m_nextParam = newParam;
   m_tail = newParam;
}

///////////////////////////////////////////////////////////////////////////////

void ShaderRenderCommand::setDataBuf( const IDString& paramName, ShaderDataBuffer* dataBuffer )
{
   ShaderParam* newParam = new ( m_allocator ) ShaderParamDataBuf( paramName, dataBuffer );
   m_tail->m_nextParam = newParam;
   m_tail = newParam;
}

///////////////////////////////////////////////////////////////////////////////
