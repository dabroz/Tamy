#include "core-Renderer\RenderStateRenderCommands.h"
#include "gl-Renderer\GLRDefinitions.h"


///////////////////////////////////////////////////////////////////////////////

void RCSetDepthTest::execute( Renderer& renderer )
{
   if ( m_useZBuffer )
   {
      glEnable( GL_DEPTH_TEST );
   }
   else
   {
      glDisable( GL_DEPTH_TEST );
   }
   glDepthMask( m_writeToZBuffer );
   glDepthFunc( glComparisonFuncs[m_depthTestFunc] );
}

///////////////////////////////////////////////////////////////////////////////

void RCSetAlphaTest::execute( Renderer& renderer )
{
   if ( m_enable )
   {
      glEnable( GL_ALPHA_TEST );
   }
   else
   {
      glDisable( GL_ALPHA_TEST );
   }
}

///////////////////////////////////////////////////////////////////////////////

void RCSetBlending::execute( Renderer& renderer )
{
   if ( m_enabled )
   {
      glEnable( GL_BLEND );
      glBlendEquation( GL_FUNC_ADD );

      glBlendFunc( glBlendingFunc[m_blendSourceFunc], glBlendingFunc[m_blendDestFunc] );
   }
   else
   {
      glDisable( GL_BLEND );
   }
}

///////////////////////////////////////////////////////////////////////////////

void RCSetFaceCulling::execute( Renderer& renderer )
{
   if ( m_mode == CULL_NONE )
   {
      glDisable( GL_CULL_FACE );
   }
   else
   {
      glEnable( GL_CULL_FACE );

      // the culling order is considered inverted in OpenGL
      if ( m_mode == CULL_CCW )
      {
         glFrontFace( GL_CW );
      }
      else
      {
         glFrontFace( GL_CCW );
      }

   }
}

///////////////////////////////////////////////////////////////////////////////
