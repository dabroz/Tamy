#include "core-Renderer\RenderStateCommand.h"
#include "core-Renderer\RenderStateRenderCommands.h"
#include "core-Renderer\RenderStateFlags.h"
#include "core-Renderer\Renderer.h"


///////////////////////////////////////////////////////////////////////////////

#define SWAP_BOOL( rhs, lhs ) { bool tmp = lhs; lhs = rhs; rhs = tmp; }
#define SWAP_INT( rhs, lhs ) { int tmp = lhs; lhs = rhs; rhs = tmp; }
#define SWAP_ENUM( Type, rhs, lhs ) { Type tmp = lhs; lhs = rhs; rhs = tmp; }

///////////////////////////////////////////////////////////////////////////////

RSSetDepthTest::RSSetDepthTest( bool writeToZBuffer, bool useZBuffer, ComparisonFunc depthTestFunc )
   : m_writeToZBuffer( writeToZBuffer )
   , m_useZBuffer( useZBuffer )
   , m_depthTestFunc( depthTestFunc )
{
}

///////////////////////////////////////////////////////////////////////////////

void RSSetDepthTest::commit( Renderer& renderer )
{
   RenderStateFlags& flags = renderer.accessRenderStateFlags();

   // a quick, numeric comparison, of 3 subsequent flags. 
   const int fRhs = ( int ) flags.m_depthTestFunc + ( ( int ) flags.m_useZBuffer << 8 ) + ( ( int ) flags.m_writeToZBuffer << 9 );
   const int fLhs = ( int ) m_depthTestFunc + ( ( int ) m_useZBuffer << 8 ) + ( ( int ) m_writeToZBuffer << 9 );
   if ( fRhs != fLhs )
   {
      new ( renderer.rtComm() ) RCSetDepthTest( m_writeToZBuffer, m_useZBuffer, m_depthTestFunc );

      // update the state flags and set the rollback values
      SWAP_BOOL( m_writeToZBuffer, flags.m_writeToZBuffer );
      SWAP_BOOL( m_useZBuffer, flags.m_useZBuffer );
      SWAP_ENUM( ComparisonFunc, m_depthTestFunc, flags.m_depthTestFunc );
   }
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

RSSetAlphaTest::RSSetAlphaTest( bool enable )
   : m_enable( enable )
{
}

///////////////////////////////////////////////////////////////////////////////

void RSSetAlphaTest::commit( Renderer& renderer )
{
   RenderStateFlags& flags = renderer.accessRenderStateFlags();
   if ( flags.m_useAlphaTest != m_enable )
   {
      new ( renderer.rtComm() ) RCSetAlphaTest( m_enable );

      // update the state flags and set the rollback values
      SWAP_BOOL( m_enable, flags.m_useAlphaTest );
   }
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

RSSetBlending::RSSetBlending( bool enabled, BlendFunc sourceFunc, BlendFunc destFunc )
   : m_enabled( enabled )
   , m_blendSourceFunc( sourceFunc )
   , m_blendDestFunc( destFunc )
{
}

///////////////////////////////////////////////////////////////////////////////

void RSSetBlending::commit( Renderer& renderer )
{
   RenderStateFlags& flags = renderer.accessRenderStateFlags();

   // a quick, numeric comparison, of 3 subsequent flags. 
   const int fRhs = ( int ) flags.m_blendSourceFunc + ( ( int ) flags.m_blendDestFunc << 8 ) + ( ( int ) flags.m_useBlending << 16 );
   const int fLhs = ( int ) m_blendSourceFunc + ( ( int ) m_blendDestFunc << 8 ) + ( ( int ) m_enabled << 16 );
   if ( fRhs != fLhs )
   {
      new ( renderer.rtComm() ) RCSetBlending( m_enabled, m_blendSourceFunc, m_blendDestFunc );

      // update the state flags and set the rollback values
      SWAP_BOOL( m_enabled, flags.m_useBlending );
      SWAP_ENUM( BlendFunc, m_blendSourceFunc, flags.m_blendSourceFunc );
      SWAP_ENUM( BlendFunc, m_blendDestFunc, flags.m_blendDestFunc );
   }
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

RSSetFaceCulling::RSSetFaceCulling( CullingMode mode )
   : m_mode( mode )
{
}

///////////////////////////////////////////////////////////////////////////////

void RSSetFaceCulling::commit( Renderer& renderer )
{
   RenderStateFlags& flags = renderer.accessRenderStateFlags();
   if ( flags.m_cullingMode != m_mode )
   {
      new ( renderer.rtComm() ) RCSetFaceCulling( m_mode );

      // update the state flags and set the rollback values
      SWAP_ENUM( CullingMode, m_mode, flags.m_cullingMode );
   }
}

///////////////////////////////////////////////////////////////////////////////
