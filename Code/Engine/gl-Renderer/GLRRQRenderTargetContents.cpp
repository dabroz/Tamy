#include "core-Renderer\RQRenderTargetContents.h"
#include "core-Renderer\PixelColorQuery.h"
#include "gl-Renderer\GLRDefinitions.h"
#include "gl-Renderer\GLRenderer.h"
#include "gl-Renderer\GLRErrorCheck.h"
#include "gl-Renderer\GLRRenderTarget2D.h"


///////////////////////////////////////////////////////////////////////////////

void RCGetRenderTargetContents::execute( Renderer& renderer )
{
   GLRenderer* glRenderer = static_cast< GLRenderer* >( renderer.implementation() );
   GLRRenderTarget2D* glRenderTarget = glRenderer->getRenderTarget( m_renderTarget );
   if ( !glRenderTarget )
   {
      return;
   }


   glRenderTarget->bindForWrite();

   const uint width = m_renderTarget->getWidth();
   const uint height = m_renderTarget->getHeight();

   // allocate a buffer large enough to be able to store query results
   m_query->allocateBuffer( width, height, 4 );
   glReadPixels( 0, 0, width, height, GL_BGRA_EXT, GL_UNSIGNED_BYTE, ( byte* ) m_query->m_buffer );

   // cleanup
   glBindFramebuffer( GL_DRAW_FRAMEBUFFER, 0 );
   glDrawBuffer( GL_BACK_LEFT );

   // notify that the query has been completed
   ++m_query->m_snapshotIdx;
}

///////////////////////////////////////////////////////////////////////////////
