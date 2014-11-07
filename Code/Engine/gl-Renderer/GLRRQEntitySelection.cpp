#include "core-Renderer\RQEntitySelection.h"
#include "core-Renderer\Renderer.h"
#include "core-Renderer\RendererUtils.h"
#include "core\ListUtils.h"
#include "gl-Renderer\GLRDefinitions.h"
#include "gl-Renderer\GLRenderer.h"
#include "gl-Renderer\GLRErrorCheck.h"
#include "gl-Renderer\GLRRenderTarget2D.h"


///////////////////////////////////////////////////////////////////////////////

void RCSelectEntities::execute( Renderer& renderer )
{
   GLRenderer* glRenderer = static_cast< GLRenderer* >( renderer.implementation() );
   RCMarshallSelectionQueryResults* resultsComm = new ( glRenderer->mtComm() ) RCMarshallSelectionQueryResults( m_listener );

   if ( !m_renderTarget )
   {
      // this command doesn't operate on the back buffer
      return;
   }

   GLRRenderTarget2D* glRenderTarget = glRenderer->getRenderTarget( m_renderTarget );
   if ( !glRenderTarget )
   {
      return;
   }

   glRenderTarget->bindForRead();

   const uint width = m_renderTarget->getWidth();
   const uint height = m_renderTarget->getHeight();

   // adjust point's coordinates so that they stay within bounds
   Point topLeftCorner, bottomRightCorner;
   topLeftCorner.x = clamp<int>( m_topLeftCorner.x, 0, width );
   topLeftCorner.y = clamp<int>( m_topLeftCorner.y, 0, height );
   bottomRightCorner.x = clamp<int>( m_bottomRightCorner.x, 0, width );
   bottomRightCorner.y = clamp<int>( m_bottomRightCorner.y, 0, height );

   const uint queryFrameWidth = bottomRightCorner.x - topLeftCorner.x + 1;
   const uint queryFrameHeight = bottomRightCorner.y - topLeftCorner.y + 1;

   // allocate a buffer large enough to be able to store query results
   byte* colorBuf = new byte[queryFrameWidth * queryFrameHeight * 4];
   glReadPixels( topLeftCorner.x, height- topLeftCorner.y, queryFrameWidth, queryFrameHeight, GL_BGRA, GL_UNSIGNED_BYTE, colorBuf );

   // scan the colors and translate them to pointers
   Color scannedColor;
   const uint pitch = queryFrameWidth * 4;
   for ( uint x = 0; x < queryFrameWidth; ++x )
   {
      for ( uint y = 0; y < queryFrameHeight; ++y )
      {
         uint addr = y * pitch + x * 4;
         byte* colorPtr = colorBuf + addr;
         scannedColor.b = colorPtr[0] / 255.f;
         scannedColor.g = colorPtr[1] / 255.f;
         scannedColor.r = colorPtr[2] / 255.f;
         scannedColor.a = colorPtr[3] / 255.f;

         Entity* entityPtr = ( Entity* ) RendererUtils::vecToPtr( ( const Vector& ) scannedColor );
         if ( !entityPtr )
         {
            continue;
         }

         // verify the entity's not on the list
         List< Entity* >::iterator entityIt = ListUtils::find( resultsComm->m_selectedEntities, entityPtr );
         if ( entityIt.isEnd() )
         {
            resultsComm->m_selectedEntities.pushBack( entityPtr );
         }
      }
   }


   // cleanup
   delete [] colorBuf;
   glBindFramebuffer( GL_READ_FRAMEBUFFER, 0 );
}

///////////////////////////////////////////////////////////////////////////////
