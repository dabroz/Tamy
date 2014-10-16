/// @file   ext-RenderingPipeline\DeferredRendererGBufferDesc.h
/// @brief  a description of the g-buffer used by the rendering pipeline
#pragma once


///////////////////////////////////////////////////////////////////////////////

enum GBuffer
{
   GBuf_Albedo,
   GBuf_Specular,
   GBuf_BRDF,
   GBuf_Normals,

   GBuf_Size
};

///////////////////////////////////////////////////////////////////////////////
