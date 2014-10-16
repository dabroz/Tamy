#include "core-Renderer\FullscreenQuad.h"
#include "gl-Renderer\GLRFullscreenQuad.h"
#include "gl-Renderer\GLRDefinitions.h"
#include "gl-Renderer\GLRenderer.h"
#include "gl-Renderer\GLRErrorCheck.h"


///////////////////////////////////////////////////////////////////////////////

void RCFullscreenQuad::execute( Renderer& renderer )
{
   GLRenderer* glRenderer = static_cast< GLRenderer* >( renderer.implementation() );
   const GLRFullscreenQuad* quad = glRenderer->getFullscreenQuad();
   quad->render();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

GLRFullscreenQuad::GLRFullscreenQuad()
{
   float vertices[4][2] = { { 0.0, 0.0 }, { 0.0, 1.0 }, { 1.0, 1.0 }, { 1.0, 0.0 } };
   word indices[6] = { 0, 1, 2, 0, 2, 3 };
   

   glGenVertexArrays( 1, &m_vaoID );
   glGenBuffers( 1, &m_vboVerticesID );
   glGenBuffers( 1, &m_vboIndicesID );

   glBindVertexArray( m_vaoID );

   glBindBuffer( GL_ARRAY_BUFFER, m_vboVerticesID );

   // fill in the vertex buffer
   glBufferData( GL_ARRAY_BUFFER, sizeof( vertices ), &vertices[0], GL_STATIC_DRAW );
   glEnableVertexAttribArray( 0 );
   glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, 0, 0 );

   // fill in the index buffer
   glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_vboIndicesID );
   glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof( indices ), &indices[0], GL_STATIC_DRAW );

}

///////////////////////////////////////////////////////////////////////////////

GLRFullscreenQuad::~GLRFullscreenQuad()
{
   glDeleteBuffers( 1, &m_vboVerticesID );
   glDeleteBuffers( 1, &m_vboIndicesID );
   glDeleteVertexArrays( 1, &m_vaoID );
}

///////////////////////////////////////////////////////////////////////////////

void GLRFullscreenQuad::render() const
{
   glBindVertexArray( m_vaoID );
   glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0 );
}

///////////////////////////////////////////////////////////////////////////////
