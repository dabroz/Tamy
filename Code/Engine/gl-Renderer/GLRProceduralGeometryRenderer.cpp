#include "core-Renderer\ProceduralGeometryRenderer.h"
#include "gl-Renderer\GLRProceduralGeometryRenderer.h"
#include "gl-Renderer\GLRenderer.h"
#include "gl-Renderer\GLRDefinitions.h"
#include "gl-Renderer\GLRErrorCheck.h"


///////////////////////////////////////////////////////////////////////////////

void RCRenderProceduralMesh::execute( Renderer& renderer )
{
   GLRenderer* glRenderer = static_cast< GLRenderer* >( renderer.implementation() );

   const GLRMutableMesh* mesh = glRenderer->getMutableMesh();
   mesh->render( m_vertices, m_indices );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

GLRMutableMesh::GLRMutableMesh()
   : m_maxVertices( 256 )
   , m_maxIndices( 1024 )
{
   glGenVertexArrays( 1, &m_vaoID );
   glGenBuffers( 1, &m_vboVerticesID );
   glGenBuffers( 1, &m_vboIndicesID );
   GLR_LOG_ERRORS();

   glBindVertexArray( m_vaoID );

   // initialize the vertex buffer
   glBindBuffer( GL_ARRAY_BUFFER, m_vboVerticesID );
   glBufferData( GL_ARRAY_BUFFER, m_maxVertices * sizeof( Vector ), NULL, GL_STREAM_DRAW );
   GLR_LOG_ERRORS();

   // define the vertex buffer's layout
   glEnableVertexAttribArray( 0 );
   glVertexAttribPointer( 0, 4, GL_FLOAT, GL_FALSE, 0, 0 );
   GLR_LOG_ERRORS();


   // initialize the index buffer
   glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_vboIndicesID );
   glBufferData( GL_ELEMENT_ARRAY_BUFFER, m_maxIndices * sizeof( word ), NULL, GL_STREAM_DRAW );
   GLR_LOG_ERRORS();
}

///////////////////////////////////////////////////////////////////////////////

GLRMutableMesh::~GLRMutableMesh()
{
   glDeleteBuffers( 1, &m_vboVerticesID );
   glDeleteBuffers( 1, &m_vboIndicesID );
   glDeleteVertexArrays( 1, &m_vaoID );
}

///////////////////////////////////////////////////////////////////////////////

void GLRMutableMesh::render( const Array< Vector >& vertices, const Array< word >& indices ) const
{ 
   glBindVertexArray( m_vaoID );
   
   // fill in the vertex buffer
   glBindBuffer( GL_ARRAY_BUFFER, m_vboVerticesID );
   glBufferSubData( GL_ARRAY_BUFFER, 0, vertices.size() * sizeof( Vector ), ( const Vector* ) vertices );
   GLR_LOG_ERRORS();

   // fill in the index buffer
   glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_vboIndicesID );
   glBufferSubData( GL_ELEMENT_ARRAY_BUFFER, 0, indices.size() * sizeof( word ), ( const word* ) indices );
   GLR_LOG_ERRORS();

   // draw the mesh
   glDrawElements( GL_TRIANGLES, indices.size(), GL_UNSIGNED_SHORT, 0 );
   GLR_LOG_ERRORS();
}

///////////////////////////////////////////////////////////////////////////////
