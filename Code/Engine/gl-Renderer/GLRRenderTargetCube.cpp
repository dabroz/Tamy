#include "gl-Renderer\GLRRenderTargetCube.h"
#include "core-Renderer\RenderTargetCube.h"
#include "gl-Renderer\GLRDefinitions.h"
#include "gl-Renderer\GLRenderer.h"
#include "gl-Renderer\GLRErrorCheck.h"


///////////////////////////////////////////////////////////////////////////////

void RCActivateRenderTargetCube::execute( Renderer& renderer )
{
   GLRenderer* glRenderer = static_cast< GLRenderer* >( renderer.implementation() );
   GLRRenderTargetCube* glRenderTarget = glRenderer->getRenderTarget( m_renderTarget );
   glRenderTarget->bindForWrite( m_sideIdx );
}

///////////////////////////////////////////////////////////////////////////////

void RCDeactivateRenderTargetCube::execute( Renderer& renderer )
{
   glBindFramebuffer( GL_DRAW_FRAMEBUFFER, 0 );
   glDrawBuffer( GL_BACK_LEFT );
}

///////////////////////////////////////////////////////////////////////////////

void RCClearRenderTargetCube::execute( Renderer& renderer )
{
   GLRenderer* glRenderer = static_cast< GLRenderer* >( renderer.implementation() );
   GLRRenderTargetCube* glRenderTarget = glRenderer->getRenderTarget( m_renderTarget );

   for ( byte sideIdx = 0; sideIdx < 6; ++sideIdx )
   {
      glRenderTarget->bindForClear( sideIdx );

      // clear
      glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
   }

   // unbind
   glBindFramebuffer( GL_DRAW_FRAMEBUFFER, 0 );
   glDrawBuffer( GL_BACK_LEFT );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

GLRRenderTargetCube::GLRRenderTargetCube( Renderer& renderer, const RenderTargetCube* renderTarget )
   : m_renderer( renderer )
   , m_renderTarget( renderTarget )
{
   m_renderTarget->addReference();
   initialize();
}

///////////////////////////////////////////////////////////////////////////////

GLRRenderTargetCube::~GLRRenderTargetCube()
{
   deinitialize();

   m_renderTarget->removeReference();
   m_renderTarget = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void GLRRenderTargetCube::initialize()
{
   // Create the FBO
   glGenFramebuffers( 1, &m_frameBufferObject );
   glBindFramebuffer( GL_FRAMEBUFFER, m_frameBufferObject );
   GLR_HALT_ERRORS();

   const uint edgeLength = m_renderTarget->getWidth();

   // Create the depth buffer
   glGenTextures( 1, &m_depthTexture );
   glBindTexture( GL_TEXTURE_2D, m_depthTexture );
   glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, edgeLength, edgeLength, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL );
   glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
   glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
   glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
   glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
   glFramebufferTexture2D( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthTexture, 0 );
   GLR_HALT_ERRORS();

   // Create the cube map
   glGenTextures( 1, &m_renderTexture );
   glBindTexture( GL_TEXTURE_CUBE_MAP, m_renderTexture );
   glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
   glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
   glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
   glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
   glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE );

   for ( byte i = 0; i < 6; i++ )
   {
      glTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_R32F, edgeLength, edgeLength, 0, GL_RED, GL_FLOAT, NULL );
   }
   GLR_HALT_ERRORS();



   GLenum status = glCheckFramebufferStatus( GL_FRAMEBUFFER );
   ASSERT_MSG( status == GL_FRAMEBUFFER_COMPLETE, "Error in the frame buffer object's setup." );

   // cleanup the binding
   glBindTexture( GL_TEXTURE_2D, 0 );
   glBindFramebuffer( GL_DRAW_FRAMEBUFFER, 0 );
   glDrawBuffer( GL_BACK_LEFT );
}

///////////////////////////////////////////////////////////////////////////////

void GLRRenderTargetCube::deinitialize()
{
   glDeleteTextures( 1, &m_renderTexture );
   glDeleteTextures( 1, &m_depthTexture );
   glDeleteFramebuffers( 1, &m_frameBufferObject );

   m_renderTexture = -1;
   m_depthTexture = -1;
   m_frameBufferObject = -1;
}

///////////////////////////////////////////////////////////////////////////////

void GLRRenderTargetCube::refresh()
{
   deinitialize();
   initialize();
}

///////////////////////////////////////////////////////////////////////////////

void GLRRenderTargetCube::bindForClear( byte sideIdx )
{
   glViewport( 0, 0, m_renderTarget->getWidth(), m_renderTarget->getHeight() );
   glBindFramebuffer( GL_DRAW_FRAMEBUFFER, m_frameBufferObject );
   glFramebufferTexture2D( GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + sideIdx, m_renderTexture, 0 );
   glDrawBuffer( GL_COLOR_ATTACHMENT0 );
}

///////////////////////////////////////////////////////////////////////////////

void GLRRenderTargetCube::bindForWrite( byte sideIdx )
{
   glViewport( 0, 0, m_renderTarget->getWidth(), m_renderTarget->getHeight() );
   glBindFramebuffer( GL_DRAW_FRAMEBUFFER, m_frameBufferObject );
   glFramebufferTexture2D( GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + sideIdx, m_renderTexture, 0 );
   glDrawBuffer( GL_COLOR_ATTACHMENT0 );

   // clear the depth buffer cause it will be stale from the time we rendered to the other side
   glClear( GL_DEPTH_BUFFER_BIT );
}

///////////////////////////////////////////////////////////////////////////////
