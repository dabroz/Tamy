/// @file   gl-Renderer\GLRenderer.h
/// @brief  OpenGL renderer implementation
#pragma once

#include "core-Renderer\RendererImplementation.h"
#include <windows.h>
#include "core\types.h"
#include "core\Array.h"

// resource storages
#include "gl-Renderer\GLRTriangleMeshResourcesStorage.h"
#include "gl-Renderer\GLRShaderResourcesStorage.h"
#include "gl-Renderer\GLRTextureResourcesStorage.h"
#include "gl-Renderer\GLRCubeTextureResourcesStorage.h"
#include "gl-Renderer\GLRProceduralTextureResourcesStorage.h"
#include "gl-Renderer\GLRRenderTargetResourcesStorage.h"
#include "gl-Renderer\GLShaderDataBufferResourcesStorage.h"


///////////////////////////////////////////////////////////////////////////////

class GLRFullscreenQuad;
class GLRMutableMesh;

///////////////////////////////////////////////////////////////////////////////

class GLRenderer : public RendererImplementation
{
   DECLARE_ALLOCATOR( GLRenderer, AM_DEFAULT );

public:
   HGLRC                                           m_hrc;      // Rendering context  
   HDC                                             m_hdc;      // Device context  
   HWND                                            m_hwnd;     // Window identifier

   uint                                            m_currWidth;
   uint                                            m_currHeight;

   // -------------------------------------------------------------------------
   // Resources
   // -------------------------------------------------------------------------
   TriangleMeshesStorage*                          m_triMeshes;
   ShadersStorage*                                 m_shaders;
   TexturesStorage*                                m_textures;
   CubeTexturesStorage*                            m_cubeTextures;
   ProceduralTexturesStorage*                      m_proceduralTextures;
   RenderTargets2DStorage*                         m_renderTargets2D;
   RenderTargetsCubeStorage*                       m_renderTargetsCube;
   ShaderDataBufferStorage*                        m_shaderDataBuffers;
   Array< IRenderResourceStorage* >                m_storages;

   // -------------------------------------------------------------------------
   // Singleton resources
   // -------------------------------------------------------------------------
   GLRFullscreenQuad*                              m_fullscreenQuad;
   GLRMutableMesh*                                 m_mutableMesh;

public:
   /**
    * Constructor.
    *
    * @param renderWindowHandle
    */
   GLRenderer( HWND renderWindowHandle );
   ~GLRenderer();

   /**
    * Creates a rendering display
    */
   void createDisplay();

   /**
   * Resizes the viewport along with the front buffer.
   *
   * @param width
   * @param height
   */
   void resizeViewport( uint width, uint height );

   // -------------------------------------------------------------------------
   // RendererImplementation implementation
   // -------------------------------------------------------------------------
   void initialize( Renderer& renderer ) override;
   void deinitialize( Renderer& renderer ) override;
   void bindRenderingContext( Renderer& renderer ) override;
   void unbindRenderingContext( Renderer& renderer ) override;

   // -------------------------------------------------------------------------
   // Resources
   // -------------------------------------------------------------------------

   /**
   * Returns an implementation dedicated to the specified triangle mesh resource.
   *
   * @param mesh
   */
   GLRTriangleMesh* getTriangleMesh( const TriangleMesh* mesh );

   /**
    * Returns an implementation dedicated to the specified shader resource.
    *
    * @param shader
    */
   GLRShader* getShader( const Shader* shader );

   /**
    * Returns an index assigned to the specified texture resource.
    *
    * @param texture
    */
   uint getTexture( const Texture* texture );

   /**
    * Returns an index assigned to the specified cube texture resource.
    *
    * @param texture
    */
   uint getCubeTexture( const CubeTexture* texture );

   /**
    * Returns an index assigned to the specified procedural texture resource.
    *
    * @param texture
    */
   uint getProceduralTexture( const ProceduralTexture* texture );

   /**
    * Returns an implementation dedicated to the specified 2D render target resource.
    *
    * @param renderTarget
    */
   GLRRenderTarget2D* getRenderTarget( const RenderTarget2D* renderTarget );

   /**
    * Returns an implementation dedicated to the specified cube render target resource.
    *
    * @param renderTarget
    */
   GLRRenderTargetCube* getRenderTarget( const RenderTargetCube* renderTarget );

   /**
    * Returns an implementation dedicated to the specified shader data buffer resource.
    *
    * @param dataBuffer
    */
   uint getShaderDataBuffer( const ShaderDataBuffer* dataBuffer );

   /**
    * Returns a full screen quad instance.
    */
   inline const GLRFullscreenQuad* getFullscreenQuad() const {
      return m_fullscreenQuad;
   }

   /**
    * Returns a mutable mesh instance.
    */
   inline const GLRMutableMesh* getMutableMesh() const {
      return m_mutableMesh;
   }

};

///////////////////////////////////////////////////////////////////////////////
