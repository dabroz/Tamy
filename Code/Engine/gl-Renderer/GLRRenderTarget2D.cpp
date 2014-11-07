#include "core-Renderer\RenderTarget2D.h"
#include "gl-Renderer\GLRRenderTarget2D.h"
#include "gl-Renderer\GLRDefinitions.h"
#include "gl-Renderer\GLRenderer.h"
#include "gl-Renderer\GLRErrorCheck.h"


///////////////////////////////////////////////////////////////////////////////

void RCActivateRenderTarget::execute( Renderer& renderer )
{
   if ( m_renderTarget )
   {
      GLRenderer* glRenderer = static_cast< GLRenderer* >( renderer.implementation() );
      GLRRenderTarget2D* glRenderTarget = glRenderer->getRenderTarget( m_renderTarget );
      glRenderTarget->bindForWrite();
   }
   else
   {
      // we're gonna be using the back buffer
      const uint width = renderer.getViewportWidth();
      const uint height = renderer.getViewportHeight();
      glViewport( 0, 0, width, height );

      glBindFramebuffer( GL_DRAW_FRAMEBUFFER, 0 );
      glDrawBuffer( GL_BACK_LEFT );
   }
}

///////////////////////////////////////////////////////////////////////////////

void RCClearRenderTarget::execute( Renderer& renderer )
{
   if ( m_renderTarget )
   {
      GLRenderer* glRenderer = static_cast< GLRenderer* >( renderer.implementation() );
      GLRRenderTarget2D* glRenderTarget = glRenderer->getRenderTarget( m_renderTarget );
      glRenderTarget->bindForWrite();
   }
   else
   {

      // we're gonna be using the back buffer
      const uint width = renderer.getViewportWidth();
      const uint height = renderer.getViewportHeight();
      glViewport( 0, 0, width, height );

      glBindFramebuffer( GL_DRAW_FRAMEBUFFER, 0 );
      glDrawBuffer( GL_BACK_LEFT );
   }

   // clear
   glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

   // unbind
   glBindFramebuffer( GL_DRAW_FRAMEBUFFER, 0 );
   glDrawBuffer( GL_BACK_LEFT );
}

///////////////////////////////////////////////////////////////////////////////

void RCGetPixel::execute( Renderer& renderer )
{
}

///////////////////////////////////////////////////////////////////////////////

void RCDeactivateRenderTarget::execute( Renderer& renderer )
{
   glBindFramebuffer( GL_DRAW_FRAMEBUFFER, 0 );
   glDrawBuffer( GL_BACK_LEFT );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

GLRRenderTarget2D::GLRRenderTarget2D( Renderer& renderer, const RenderTarget2D* renderTarget )
   : m_renderer( renderer )
   , m_renderTarget( renderTarget )
   , m_frameBufferObject( 0 )
   , m_renderTexture( 0 )
{
   m_renderTarget->addReference();

   initialize();
}

///////////////////////////////////////////////////////////////////////////////

GLRRenderTarget2D::~GLRRenderTarget2D()
{
   deinitialize();

   m_renderTarget->removeReference();
   m_renderTarget = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void GLRRenderTarget2D::initialize()
{
   // generate and bind a frame buffer object
   glGenFramebuffers( 1, &m_frameBufferObject );
   glBindFramebuffer( GL_FRAMEBUFFER, m_frameBufferObject );
   GLR_HALT_ERRORS();

   // generate the off screen texture
   const byte renderTexturesCount = m_renderTarget->getRenderTexturesCount();
   m_renderTexture.resize( renderTexturesCount, 0 );
   glGenTextures( renderTexturesCount, ( uint* ) m_renderTexture );
   glGenTextures( 1, &m_depthTexture );

   GLR_HALT_ERRORS();

   const uint currentWidth = m_renderTarget->getWidth();
   const uint currentHeight = m_renderTarget->getHeight();

   // initialize the color texture
   m_attachments.resize( renderTexturesCount, GL_COLOR_ATTACHMENT0 );
   for ( byte i = 0; i < renderTexturesCount; ++i )
   {
      glBindTexture( GL_TEXTURE_2D, m_renderTexture[i] );
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER );
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER );
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
      glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA8, currentWidth, currentHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL );
      glFramebufferTexture2D( GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, m_renderTexture[i], 0 );
      GLR_HALT_ERRORS();

      m_attachments[i] = GL_COLOR_ATTACHMENT0 + i;
   }

   // initialize the depth texture
   glBindTexture( GL_TEXTURE_2D, m_depthTexture );
   glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER );
   glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER );
   glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
   glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
   glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, currentWidth, currentHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL );
   glFramebufferTexture2D( GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthTexture, 0 );
   GLR_HALT_ERRORS();

   GLuint status = glCheckFramebufferStatus( GL_DRAW_FRAMEBUFFER );
   ASSERT_MSG( status == GL_FRAMEBUFFER_COMPLETE, "Error in the frame buffer object's setup." );

   // cleanup the binding
   glBindTexture( GL_TEXTURE_2D, 0 );
   glBindFramebuffer( GL_DRAW_FRAMEBUFFER, 0 );
   glDrawBuffer( GL_BACK_LEFT );
}

///////////////////////////////////////////////////////////////////////////////

void GLRRenderTarget2D::deinitialize()
{
   const byte renderTexturesCount = m_renderTarget->getRenderTexturesCount();

   glDeleteTextures( renderTexturesCount, ( uint* ) m_renderTexture );
   glDeleteTextures( 1, &m_depthTexture );
   glDeleteFramebuffers( 1, &m_frameBufferObject );

   m_depthTexture = -1;
   m_frameBufferObject = -1;
}

///////////////////////////////////////////////////////////////////////////////

void GLRRenderTarget2D::refresh()
{
   deinitialize();
   initialize();
}

///////////////////////////////////////////////////////////////////////////////

void GLRRenderTarget2D::onViewportResized( uint width, uint height )
{
   // resize the buffer, but to the size of the render target, not the viewport ( those sizes might differ
   // depending on whether this is a dynamic or a static buffer )
   if ( !m_renderTarget )
   {
      return;
   }
   const uint currWidth = m_renderTarget->getWidth();
   const uint currHeight = m_renderTarget->getHeight();

   glFramebufferParameteri( GL_DRAW_FRAMEBUFFER, GL_FRAMEBUFFER_DEFAULT_WIDTH, currWidth );
   glFramebufferParameteri( GL_DRAW_FRAMEBUFFER, GL_FRAMEBUFFER_DEFAULT_HEIGHT, currHeight );

   const byte renderTexturesCount = m_renderTarget->getRenderTexturesCount();
   for ( byte i = 0; i < renderTexturesCount; ++i )
   {
      glBindTexture( GL_TEXTURE_2D, m_renderTexture[i] );
      glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA8, currWidth, currHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL );
   }

   glBindTexture( GL_TEXTURE_2D, m_depthTexture );
   glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, currWidth, currHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL );
}

///////////////////////////////////////////////////////////////////////////////

void GLRRenderTarget2D::bindForWrite()
{
   glViewport( 0, 0, m_renderTarget->getWidth(), m_renderTarget->getHeight() );
   glBindFramebuffer( GL_DRAW_FRAMEBUFFER, m_frameBufferObject );

   const byte renderTexturesCount = m_renderTarget->getRenderTexturesCount();
   glDrawBuffers( renderTexturesCount, ( uint* ) m_attachments );
}

///////////////////////////////////////////////////////////////////////////////

void GLRRenderTarget2D::bindForRead()
{
   glViewport( 0, 0, m_renderTarget->getWidth(), m_renderTarget->getHeight() );
   glBindFramebuffer( GL_READ_FRAMEBUFFER, m_frameBufferObject );

   glReadBuffer( GL_COLOR_ATTACHMENT0 );
}

///////////////////////////////////////////////////////////////////////////////

