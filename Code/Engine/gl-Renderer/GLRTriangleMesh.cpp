#include "gl-Renderer\GLRTriangleMesh.h"
#include "gl-Renderer\GLRDefinitions.h"
#include "core-Renderer\Renderer.h"
#include "gl-Renderer\GLRenderer.h"
#include "gl-Renderer\GLRErrorCheck.h"


///////////////////////////////////////////////////////////////////////////////

void RCRenderTriangleMesh::execute( Renderer& renderer )
{
   GLRenderer* glRenderer = static_cast< GLRenderer* >( renderer.implementation() );
   GLRTriangleMesh* mesh = glRenderer->getTriangleMesh( m_mesh );
   if ( mesh )
   {
      mesh->render();
   }
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

GLRTriangleMesh::GLRTriangleMesh( GLRenderer& renderer, const TriangleMesh* mesh )
   : m_renderer( renderer )
   , m_mesh( mesh )
   , m_vaoID( (uint)-1 )
   , m_vboVerticesID( (uint)-1 )
   , m_vboIndicesID( (uint)-1 )
   , m_vboVertexWeights( (uint)-1 )
   , m_verticesCount( 0 )
   , m_facesCount( 0 )
{
   m_mesh->addReference();
   initialize();
}

///////////////////////////////////////////////////////////////////////////////

GLRTriangleMesh::~GLRTriangleMesh()
{
   if ( m_vboVertexWeights != ( uint ) -1 )
   {
      glDeleteBuffers( 1, &m_vboVertexWeights );
   }

   if ( m_vboVerticesID != ( uint ) -1 )
   {
      glDeleteBuffers( 1, &m_vboVerticesID );
   }

   if ( m_vboIndicesID != ( uint ) -1 )
   {
      glDeleteBuffers( 1, &m_vboIndicesID );
   }

   if ( m_vaoID != ( uint ) -1 )
   {
      glDeleteVertexArrays( 1, &m_vaoID );
   }

   m_mesh->removeReference();
   m_mesh = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void GLRTriangleMesh::initialize()
{
   const Array< LitVertex >& vertices = m_mesh->getVertices();
   const Array< Face >& faces = m_mesh->getFaces();
   m_verticesCount = vertices.size();
   m_facesCount = faces.size();

   if ( m_verticesCount == 0 || m_facesCount == 0 )
   {
      return;
   }

   glGenVertexArrays( 1, &m_vaoID );
   glGenBuffers( 1, &m_vboVerticesID );
   glGenBuffers( 1, &m_vboIndicesID );
   glBindVertexArray( m_vaoID );
   GLR_LOG_ERRORS();

   // this is where we bind them to the context
   {
      const LitVertex* ptrArrVertices = ( const LitVertex* ) vertices;
      glBindBuffer( GL_ARRAY_BUFFER, m_vboVerticesID );
      glBufferData( GL_ARRAY_BUFFER, sizeof( LitVertex ) * vertices.size(), ptrArrVertices, GL_STATIC_DRAW );
      GLR_LOG_ERRORS();

      // set the vertex description
      const GLsizei stride = sizeof( LitVertex );

      glEnableVertexAttribArray( GLATTR_POS );
      glVertexAttribPointer( GLATTR_POS, 4, GL_FLOAT, GL_FALSE, stride, 0 );
      GLR_LOG_ERRORS();

      glEnableVertexAttribArray( GLATTR_NORMAL );
      glVertexAttribPointer( GLATTR_NORMAL, 3, GL_FLOAT, GL_FALSE, stride, ( const GLvoid* ) offsetof( LitVertex, m_normal ) );
      GLR_LOG_ERRORS();

      glEnableVertexAttribArray( GLATTR_TANGENT );
      glVertexAttribPointer( GLATTR_TANGENT, 3, GL_FLOAT, GL_FALSE, stride, ( const GLvoid* ) offsetof( LitVertex, m_tangent ) );
      GLR_LOG_ERRORS();

      glEnableVertexAttribArray( GLATTR_TEXCOORDS0 );
      glVertexAttribPointer( GLATTR_TEXCOORDS0, 2, GL_FLOAT, GL_FALSE, stride, ( const GLvoid* ) offsetof( LitVertex, m_textureCoords ) );
      GLR_LOG_ERRORS();
   }

   // create a buffer for vertex weights
   if ( m_mesh->hasVertexWeights() )
   {
      glGenBuffers( 1, &m_vboVertexWeights );

      const Array< VertexWeight >& weights = m_mesh->getVertexWeights();
      const VertexWeight* ptrArrWeights = ( const VertexWeight* ) weights;
      glBindBuffer( GL_ARRAY_BUFFER, m_vboVertexWeights );
      glBufferData( GL_ARRAY_BUFFER, sizeof( VertexWeight ) * weights.size(), ptrArrWeights, GL_STATIC_DRAW );
      GLR_LOG_ERRORS();


      const GLsizei weightsStride = sizeof( VertexWeight );
      glEnableVertexAttribArray( GLATTR_VERTEXWEIGHT );
      glVertexAttribPointer( GLATTR_VERTEXWEIGHT, 4, GL_FLOAT, GL_FALSE, weightsStride, 0 );
      GLR_LOG_ERRORS();

      glEnableVertexAttribArray( GLATTR_BONEINDEX );
      glVertexAttribPointer( GLATTR_BONEINDEX, 4, GL_FLOAT, GL_FALSE, weightsStride, ( const GLvoid* ) offsetof( VertexWeight, m_indices ) );
      GLR_LOG_ERRORS();
   }

   const Face* ptrArrFaces = ( const Face* ) faces;
   glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_vboIndicesID );
   glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof( Face ) * faces.size(), ptrArrFaces, GL_STATIC_DRAW );
   GLR_LOG_ERRORS();

   // unbind the array object
   glBindVertexArray( 0 );
}

///////////////////////////////////////////////////////////////////////////////

void GLRTriangleMesh::render()
{
   // bind the vertex array
   glBindVertexArray( m_vaoID );

   // draw the mesh
   glDrawElements( GL_TRIANGLES, m_facesCount * 3, GL_UNSIGNED_SHORT, 0 );
   GLR_LOG_ERRORS();
}

///////////////////////////////////////////////////////////////////////////////
