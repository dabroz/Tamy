#include "gl-Renderer\GLRTriangleMeshResourcesStorage.h"
#include "gl-Renderer\GLRDefinitions.h"


///////////////////////////////////////////////////////////////////////////////

template<>
GLRTriangleMesh* RenderResourceStorage< GLRenderer, TriangleMesh, GLRTriangleMesh* >::createResource( const TriangleMesh* obj ) const
{
   GLRScopedContextActivator contextActivator( m_rendererImplementation );
   return new GLRTriangleMesh( m_rendererImplementation, obj );
}

///////////////////////////////////////////////////////////////////////////////

template<>
void RenderResourceStorage< GLRenderer, TriangleMesh, GLRTriangleMesh* >::releaseResource( GLRTriangleMesh* resource ) const
{
   GLRScopedContextActivator contextActivator( m_rendererImplementation );
   delete resource;
}

///////////////////////////////////////////////////////////////////////////////
