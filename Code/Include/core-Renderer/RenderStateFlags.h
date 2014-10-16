/// @file   core-Renderer\RenderStateFlags.h
/// @brief  a set of all render state flags
#pragma once

#include "core\MemoryRouter.h"
#include "core-Renderer\RenderingParams.h"


///////////////////////////////////////////////////////////////////////////////

struct RenderStateFlags
{
   DECLARE_ALLOCATOR( RenderStateFlags, AM_DEFAULT );


   CullingMode                   m_cullingMode;

   bool                          m_useZBuffer;
   bool                          m_writeToZBuffer;
   ComparisonFunc                m_depthTestFunc;

   bool                          m_useAlphaTest;
   ComparisonFunc                m_alphaTestFunc;
   float                         m_alphaTestRefVal;

   bool                          m_useBlending;
   BlendFunc                     m_blendSourceFunc;
   BlendFunc                     m_blendDestFunc;

   bool                          m_useSeparateAlphaBlend;
   BlendFunc                     m_alphaBlendSourceFunc;
   BlendFunc                     m_alphaBlendDestFunc;

   bool                          m_stencilEnable;
   StencilOp                     m_stencilFail;
   StencilOp                     m_stencilZFail;
   StencilOp                     m_stencilPass;
   ComparisonFunc                m_stencilFunc;
   ulong                         m_stencilRef;
   ulong                         m_stencilMask;
   ulong                         m_stencilWriteMask;

   /**
    * Constructor.
    */
   RenderStateFlags()
      : m_cullingMode( CULL_CCW )
      , m_useZBuffer( true )
      , m_writeToZBuffer( true )
      , m_depthTestFunc( CMP_LESSEQUAL )
      , m_useAlphaTest( false )
      , m_alphaTestFunc( CMP_ALWAYS )
      , m_alphaTestRefVal( 1 )
      , m_useBlending( false )
      , m_blendSourceFunc( BLEND_ONE )
      , m_blendDestFunc( BLEND_ZERO )
      , m_useSeparateAlphaBlend( false )
      , m_alphaBlendSourceFunc( BLEND_ONE )
      , m_alphaBlendDestFunc( BLEND_ZERO )
      , m_stencilEnable( false )
      , m_stencilFail( STENCILOP_KEEP )
      , m_stencilZFail( STENCILOP_KEEP )
      , m_stencilPass( STENCILOP_KEEP )
      , m_stencilFunc( CMP_ALWAYS )
      , m_stencilRef( 0 )
      , m_stencilMask( 0xFFFFFFFF )
      , m_stencilWriteMask( 0xFFFFFFFF )
   {}
};

///////////////////////////////////////////////////////////////////////////////
