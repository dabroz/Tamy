#include "gl-Renderer\GLRShaderResourcesStorage.h"
#include "gl-Renderer\GLRDefinitions.h"


///////////////////////////////////////////////////////////////////////////////

template<>
GLRShader* RenderResourceStorage< GLRenderer, Shader, GLRShader* >::createResource( const Shader* obj ) const
{
   GLRScopedContextActivator contextActivator( m_rendererImplementation );

   return new GLRShader( m_rendererImplementation, obj );
}

///////////////////////////////////////////////////////////////////////////////

template<>
void RenderResourceStorage< GLRenderer, Shader, GLRShader* >::releaseResource( GLRShader* resource ) const
{
   GLRScopedContextActivator contextActivator( m_rendererImplementation );
   delete resource;
}

///////////////////////////////////////////////////////////////////////////////

template<>
void RenderResourceStorage< GLRenderer, Shader, GLRShader* >::refreshResource( const Shader* obj, GLRShader*& resource ) const
{
   resource->releaseShaders();
   resource->compile();
}

///////////////////////////////////////////////////////////////////////////////
