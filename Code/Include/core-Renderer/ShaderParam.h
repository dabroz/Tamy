/// @file   core-Renderer/ShaderParam.h
/// @brief  class that sets specific shader parameter
#ifndef _SHADER_PARAM_H
#define _SHADER_PARAM_H

#include <string>
#include "core\types.h"
#include "core\MemoryRouter.h"
#include "core\IDString.h"


///////////////////////////////////////////////////////////////////////////////

class Shader;
class Texture;
class CubeTexture;
struct TextureSamplerSettings;
class RenderTarget2D;
class RenderTargetCube;
class Renderer;
class ShaderDataBuffer;
class ProceduralTexture;

///////////////////////////////////////////////////////////////////////////////

/**
 * Class that sets specific shader parameter. Visitor pattern used.
 *
 * The class is designed to be instanced in a memory pool only,
 */
class ShaderParam
{
   DECLARE_ALLOCATOR( ShaderParam, AM_DEFAULT );

public:
   ShaderParam*         m_nextParam;      // parameters form a single-linked list

public:
   /**
    * Constructor.
    */
   ShaderParam() : m_nextParam( NULL ) {}
   virtual ~ShaderParam() {}

   /**
    * Sets the parameter on a shader implementation.
    *
    * @param renderer         host renderer
    * @param shaderPtr        pointer to an implementation specific shader - reinterpret_cast it
    */
   virtual void setParam( Renderer& renderer, void* shaderPtr ) {}

};

///////////////////////////////////////////////////////////////////////////////

/**
 * Texture shader parameter.
 */
class ShaderParamTexture : public ShaderParam
{
   DECLARE_ALLOCATOR( ShaderParamTexture, AM_DEFAULT );

private:
   IDString                         m_name;
   Texture*                         m_val;
   const TextureSamplerSettings&    m_samplerSettings;

public:
   ShaderParamTexture( const IDString& nameId, Texture* val, const TextureSamplerSettings& samplerSettings );
   ~ShaderParamTexture();

   // -------------------------------------------------------------------------
   // ShaderParam implementation
   // -------------------------------------------------------------------------
   void setParam( Renderer& renderer, void* shaderPtr );
};

///////////////////////////////////////////////////////////////////////////////

/**
* Texture shader parameter.
*/
class ShaderParamCubeTexture : public ShaderParam
{
   DECLARE_ALLOCATOR( ShaderParamCubeTexture, AM_DEFAULT );

private:
   IDString                         m_name;
   CubeTexture*                     m_val;
   const TextureSamplerSettings&    m_samplerSettings;

public:
   ShaderParamCubeTexture( const IDString& nameId, CubeTexture* val, const TextureSamplerSettings& samplerSettings );
   ~ShaderParamCubeTexture();

   // -------------------------------------------------------------------------
   // ShaderParam implementation
   // -------------------------------------------------------------------------
   void setParam( Renderer& renderer, void* shaderPtr );
};

///////////////////////////////////////////////////////////////////////////////

/**
 * Render target shader parameter.
 */
class ShaderParamRenderTarget2D : public ShaderParam
{
   DECLARE_ALLOCATOR( ShaderParamRenderTarget2D, AM_DEFAULT );

private:
   IDString                            m_name;
   RenderTarget2D*                     m_val;
   const TextureSamplerSettings&       m_samplerSettings;
   byte                                m_renderTextureIdx;

public:
   ShaderParamRenderTarget2D( const IDString& nameId, RenderTarget2D* val, const TextureSamplerSettings& samplerSettings, byte renderTextureIdx = 0 );
   ~ShaderParamRenderTarget2D();

   // -------------------------------------------------------------------------
   // ShaderParam implementation
   // -------------------------------------------------------------------------
   void setParam( Renderer& renderer, void* shaderPtr );
};

///////////////////////////////////////////////////////////////////////////////

class ShaderParamDepthBuffer2D : public ShaderParam
{
   DECLARE_ALLOCATOR( ShaderParamDepthBuffer2D, AM_DEFAULT );

private:
   IDString                            m_name;
   RenderTarget2D*                     m_val;

public:
   ShaderParamDepthBuffer2D( const IDString& nameId, RenderTarget2D* renderTarget );
   ~ShaderParamDepthBuffer2D();

   // -------------------------------------------------------------------------
   // ShaderParam implementation
   // -------------------------------------------------------------------------
   void setParam( Renderer& renderer, void* shaderPtr );
};

///////////////////////////////////////////////////////////////////////////////

/**
* Render target shader parameter.
*/
class ShaderParamRenderTargetCube : public ShaderParam
{
   DECLARE_ALLOCATOR( ShaderParamRenderTargetCube, AM_DEFAULT );

private:
   IDString                            m_name;
   RenderTargetCube*                   m_val;
   const TextureSamplerSettings&       m_samplerSettings;

public:
   ShaderParamRenderTargetCube( const IDString& nameId, RenderTargetCube* val, const TextureSamplerSettings& samplerSettings );
   ~ShaderParamRenderTargetCube();

   // -------------------------------------------------------------------------
   // ShaderParam implementation
   // -------------------------------------------------------------------------
   void setParam( Renderer& renderer, void* shaderPtr );
};

///////////////////////////////////////////////////////////////////////////////

/**
 * Procedural texture shader parameter.
 */
class ShaderParamProceduralTexture : public ShaderParam
{
   DECLARE_ALLOCATOR( ShaderParamProceduralTexture, AM_DEFAULT );

private:
   IDString                            m_name;
   ProceduralTexture*                  m_val;
   const TextureSamplerSettings&       m_samplerSettings;

public:
   ShaderParamProceduralTexture( const IDString& nameId, ProceduralTexture* val, const TextureSamplerSettings& samplerSettings );
   ~ShaderParamProceduralTexture();

   // -------------------------------------------------------------------------
   // ShaderParam implementation
   // -------------------------------------------------------------------------
   void setParam( Renderer& renderer, void* shaderPtr );
};

///////////////////////////////////////////////////////////////////////////////

class ShaderParamDataBuf : public ShaderParam
{
   DECLARE_ALLOCATOR( ShaderParamDataBuf, AM_DEFAULT );

private:
   IDString                            m_name;
   ShaderDataBuffer*                   m_dataBuf;
   Array< byte >                       m_data;

public:
   ShaderParamDataBuf( const IDString& nameId, ShaderDataBuffer* dataBuf );
   ~ShaderParamDataBuf();

   // -------------------------------------------------------------------------
   // ShaderParam implementation
   // -------------------------------------------------------------------------
   void setParam( Renderer& renderer, void* shaderPtr );
};

///////////////////////////////////////////////////////////////////////////////

#endif _SHADER_PARAM_H
