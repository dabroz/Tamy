/// @file   ext-RenderingPipeline\AmbientOcclusionPass.h
/// @brief  ambient occlusion rendering pass
#pragma once

#include "core\MemoryRouter.h"
#include "core\ResourceHandle.h"
#include "core-Renderer\TextureSamplerSettings.h"
#include "core-Renderer\Shader.h"
#include "core\Vector.h"
#include "core\Matrix.h"
#include "core\Color.h"


///////////////////////////////////////////////////////////////////////////////

#define     MAX_AO_KERNEL_SIZE         32

///////////////////////////////////////////////////////////////////////////////

class RenderTarget2D;
class ShaderTexture;

///////////////////////////////////////////////////////////////////////////////

struct AmbientOcclusionSettings
{
   DECLARE_ALLOCATOR( AmbientOcclusionSettings, AM_DEFAULT );

   Vector               m_halfPixel;
   // -------------------------   16 bytes boundary ---------------------------
   Vector               m_projParams;
   // -------------------------   16 bytes boundary ---------------------------
   Vector               m_kernel[MAX_AO_KERNEL_SIZE];
   // -------------------------   16 bytes boundary ---------------------------
   Matrix               m_projectionMtx;
   // -------------------------   16 bytes boundary ---------------------------
   Color                m_ambientColor;
   // -------------------------   16 bytes boundary ---------------------------
   float                m_radius;
   float                m_power;
   int                  m_sampleKernelSize;
   int                  m_padding;
   // -------------------------   16 bytes boundary ---------------------------
};

///////////////////////////////////////////////////////////////////////////////

class AmbientOcclusionPass
{
   DECLARE_ALLOCATOR( AmbientOcclusionPass, AM_DEFAULT );

private:
   TResourceHandle< Shader >        m_shader;
   class ShaderDataBuffer*          m_constantsBuf;

   class ProceduralTexture*         m_randomTexture;
   TextureSamplerSettings           m_randomTextureSampler;
   TextureSamplerSettings           m_normalsBufferSampler;

   RenderTarget2D*                   m_intermediateAOTarget;
   class TexturesBlender*           m_blurEffect;

public:
   /**
    * Constructor.
    *
    * @param intermediateAOTarget
    */
   AmbientOcclusionPass( RenderTarget2D* intermediateAOTarget );
   ~AmbientOcclusionPass();

   /**
    * Call this method to update the occlusion settings.
    */
   AmbientOcclusionSettings& accessSettings();

   /**
    * Renders the pass.
    *
    * @param renderer
    * @param gBuffer
    * @param ambientColor
    * @param outRenderTarget
    */
   void render( Renderer& renderer, RenderTarget2D* gBuffer, const Color& ambientColor, RenderTarget2D* outRenderTarget );

   /**
    * Recalculates the sampling kernel.
    *
    * @param size
    */
   void calculateKernel( uint size );
};

///////////////////////////////////////////////////////////////////////////////
