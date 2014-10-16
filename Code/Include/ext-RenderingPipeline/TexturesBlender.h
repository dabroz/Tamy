/// @file   ext-RenderingPipeline\TexturesBlender.h
/// @brief  a set of utilities used by the forward rendering algorithm
#pragma once

#include "core\Array.h"
#include "core-Renderer\TextureSamplerSettings.h"
#include "core-Renderer\ShaderDataBuffer.h"
#include "core\ResourceHandle.h"
#include "core\Color.h"
#include "core\Quaternion.h"
#include "core\Matrix.h"


///////////////////////////////////////////////////////////////////////////////

class ShaderTexture;
class RenderTarget2D;
class Shader;
class TriangleMesh;
class GeometryComponent;
class Light;

///////////////////////////////////////////////////////////////////////////////

class TexturesBlender
{
private:
   static TResourceHandle< Shader >          m_alphaShader;
   static TResourceHandle< Shader >          m_colorShader;
   static TResourceHandle< Shader >          m_blurShader;

   class ShaderDataBuffer*                   m_constantsBuf;
   TextureSamplerSettings                    m_samplerSettings;

public:
   /**
    * Constructor.
    */
   TexturesBlender();
   ~TexturesBlender();

   /**
    * Blends two textures by color.
    *
    * @param renderer
    * @param tex1
    * @param tex2
    * @param outRenderTarget
    */
   void blendByColor( class Renderer& renderer, ShaderTexture* tex1, ShaderTexture* tex2, RenderTarget2D* outRenderTarget );

   /**
    * Blends two textures by alpha.
    *
    * @param renderer
    * @param tex1
    * @param tex2
    * @param outRenderTarget
    */
   void blendByAlpha( class Renderer& renderer, ShaderTexture* tex1, ShaderTexture* tex2, RenderTarget2D* outRenderTarget );

   /**
    * Blurs the specified texture.
    *
    * @param renderer
    * @param inTex
    * @param outTarget
    */
   void blur( class Renderer& renderer, ShaderTexture* inTex, RenderTarget2D* outTarget );
};

///////////////////////////////////////////////////////////////////////////////
