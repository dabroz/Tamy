#include "gl-Renderer\GLRRenderTargetResourcesStorage.h"


///////////////////////////////////////////////////////////////////////////////

template<>
GLRRenderTarget2D* RenderResourceStorage< GLRenderer, RenderTarget2D, GLRRenderTarget2D* >::createResource( const RenderTarget2D* obj ) const
{
   return new GLRRenderTarget2D( m_renderer, obj );
}

///////////////////////////////////////////////////////////////////////////////

template<>
void RenderResourceStorage< GLRenderer, RenderTarget2D, GLRRenderTarget2D* >::releaseResource( GLRRenderTarget2D* resource ) const
{
   delete resource;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

template<>
GLRRenderTargetCube* RenderResourceStorage< GLRenderer, RenderTargetCube, GLRRenderTargetCube* >::createResource( const RenderTargetCube* obj ) const
{
   return new GLRRenderTargetCube( m_renderer, obj );
}

///////////////////////////////////////////////////////////////////////////////

template<>
void RenderResourceStorage< GLRenderer, RenderTargetCube, GLRRenderTargetCube* >::releaseResource( GLRRenderTargetCube* resource ) const
{
   delete resource;
}

///////////////////////////////////////////////////////////////////////////////
