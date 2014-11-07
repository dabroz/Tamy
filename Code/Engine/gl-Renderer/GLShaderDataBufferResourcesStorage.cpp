#include "gl-Renderer\GLShaderDataBufferResourcesStorage.h"
#include "gl-Renderer\GLRenderer.h"
#include "gl-Renderer\GLRDefinitions.h"
#include "gl-Renderer\GLRErrorCheck.h"
#include "core\Log.h"


///////////////////////////////////////////////////////////////////////////////

template<>
uint RenderResourceStorage< GLRenderer, ShaderDataBuffer, uint >::createResource( const ShaderDataBuffer* obj ) const
{
   uint bufferId = 0;

   glGenBuffers( 1, &bufferId );

   return bufferId;
}

///////////////////////////////////////////////////////////////////////////////

template<>
void RenderResourceStorage< GLRenderer, ShaderDataBuffer, uint >::releaseResource( uint bufferId ) const
{
   if ( bufferId != ( uint ) -1 )
   {
      glDeleteBuffers( 1, &bufferId );
   }
}

///////////////////////////////////////////////////////////////////////////////

template<>
void RenderResourceStorage< GLRenderer, ShaderDataBuffer, uint >::refreshResource( const ShaderDataBuffer* obj, uint& bufferId ) const
{
   releaseResource( bufferId );
   bufferId = createResource( obj );
}

///////////////////////////////////////////////////////////////////////////////
