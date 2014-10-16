#include "core-Renderer\RenderStateRenderCommands.h"


///////////////////////////////////////////////////////////////////////////////

RCSetDepthTest::RCSetDepthTest( bool writeToZBuffer, bool useZBuffer, ComparisonFunc depthTestFunc )
   : m_useZBuffer( useZBuffer )
   , m_writeToZBuffer( writeToZBuffer )
   , m_depthTestFunc( depthTestFunc )
{}

///////////////////////////////////////////////////////////////////////////////

RCSetAlphaTest::RCSetAlphaTest( bool enable )
   : m_enable( enable )
{
}

///////////////////////////////////////////////////////////////////////////////

RCSetBlending::RCSetBlending( bool enabled, BlendFunc sourceFunc, BlendFunc destFunc )
   : m_enabled( enabled )
   , m_blendSourceFunc( sourceFunc )
   , m_blendDestFunc( destFunc )
{}

///////////////////////////////////////////////////////////////////////////////

RCSetFaceCulling::RCSetFaceCulling( CullingMode mode )
   : m_mode( mode )
{
}

///////////////////////////////////////////////////////////////////////////////
