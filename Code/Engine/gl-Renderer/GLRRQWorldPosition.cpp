#include "core-Renderer\RQWorldPosition.h"
#include "core-Renderer\Renderer.h"
#include "core-Renderer\Camera.h"
#include "gl-Renderer\GLRDefinitions.h"
#include "gl-Renderer\GLRenderer.h"
#include "gl-Renderer\GLRErrorCheck.h"
#include "gl-Renderer\GLRRenderTarget2D.h"


///////////////////////////////////////////////////////////////////////////////

void RCQueryWorldPosition::execute( Renderer& renderer )
{
   GLRenderer* glRenderer = static_cast< GLRenderer* >( renderer.implementation() );
   GLRRenderTarget2D* glRenderTarget = glRenderer->getRenderTarget( m_renderTarget );
   if ( !glRenderTarget )
   {
      return;
   }

   Vector viewProjPoint;
   FastFloat depth;
   {
      glRenderTarget->bindForWrite();

      const uint width = m_renderTarget->getWidth();
      const uint height = m_renderTarget->getHeight();
      const uint pitch = 4 * width;

      // adjust point's coordinates so that they stay within bounds
      Point viewportPos;
      viewportPos.x = clamp<int>( m_viewportPos.x, 0, width );
      viewportPos.y = clamp<int>( m_viewportPos.y, 0, height );

      // query the depth value
      byte colorPtr[4];
      glReadPixels( viewportPos.x, viewportPos.y, 1, 1, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, ( byte* ) colorPtr );
      depth.setFromFloat( *( float* ) colorPtr );



      // calculate perspective coordinates of the specified viewport point
      float perspX = ( ( float ) viewportPos.x * 2.0f / ( float ) width ) - 1.0f;
      float perspY = 1.0f - ( ( float ) viewportPos.y * 2.0f / ( float ) height );

      viewProjPoint.set( perspX, perspY, 1.0f, 1.0f );

      // unbind the frame buffer
      glBindFramebuffer( GL_DRAW_FRAMEBUFFER, 0 );
      glDrawBuffer( GL_BACK_LEFT );
   }


   // now calculate world coordinates
   RCMarshallWorldPositionQueryResults* resultsComm = new ( glRenderer->mtComm() ) RCMarshallWorldPositionQueryResults( m_listener );
   {
      Camera& camera = renderer.getActiveCamera();

      Matrix viewProj;
      viewProj.setMul( camera.getViewMtx(), camera.getProjectionMtx() );

      Matrix invProj;
      invProj.setInverse( camera.getProjectionMtx() );

      Vector viewPos;
      invProj.transform4( viewProjPoint, viewPos );
      viewPos.mul( depth );

      Matrix invView;
      invView.setInverse( camera.getViewMtx() );

      invView.transform( viewPos, resultsComm->m_worldPos );
   }

   
}

///////////////////////////////////////////////////////////////////////////////
