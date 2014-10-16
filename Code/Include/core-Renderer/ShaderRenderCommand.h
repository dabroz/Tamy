/// @file   core-Renderer/ShaderRenderCommands.h
/// @brief  common base for shader rendering commands
#ifndef _SHADER_RENDER_COMMAND_H
#define _SHADER_RENDER_COMMAND_H

#include "core-Renderer\RenderCommand.h"
#include "core-Renderer\ShaderParam.h"
#include "core\MemoryRouter.h"
#include "core\List.h"


///////////////////////////////////////////////////////////////////////////////

class ShaderTexture;
class RenderTarget2D;
struct TextureSamplerSettings;
class IDString;

///////////////////////////////////////////////////////////////////////////////

/**
 * A common base for shader rendering commands.
 */
class ShaderRenderCommand : public RenderCommand 
{
   DECLARE_ALLOCATOR( ShaderRenderCommand, AM_DEFAULT );

private:
   MemoryAllocator*                 m_allocator;
   ShaderParam*                     m_root;
   ShaderParam*                     m_tail;

public:
   /**
    * Constructor.
    *
    * @param allocator
    */
   ShaderRenderCommand( MemoryAllocator& allocator );
   virtual ~ShaderRenderCommand();
  
   void setTexture( const IDString& paramName, ShaderTexture* texture, const TextureSamplerSettings& samplerSettings, byte renderTextureIdx = 0 );
   void setDepthBuffer( const IDString& paramName, RenderTarget2D* renderTarget );
   void setDataBuf( const IDString& paramName, ShaderDataBuffer* dataBuffer );

protected:
   /**
    * Call this to set the parameters on an implementation-specific shader.
    *
    * @param renderer         host renderer
    * @param shaderImpl       pointer to a platform specific shader implementation
    */
   void setParams( Renderer& renderer, void* shaderImpl );
};

///////////////////////////////////////////////////////////////////////////////

#endif _SHADER_RENDER_COMMAND_H
