/// @file   gl-Renderer\GLRShader.h
/// @brief  OpenGL shader implementation
#pragma once

#include "core-Renderer\Shader.h"
#include "core\Array.h"


///////////////////////////////////////////////////////////////////////////////

class GLRenderer;
struct TextureSamplerSettings;

///////////////////////////////////////////////////////////////////////////////

enum GLStreamDataType
{
   GLATTR_POS,
   GLATTR_NORMAL,
   GLATTR_TANGENT,
   GLATTR_TEXCOORDS0,
   GLATTR_VERTEXWEIGHT,
   GLATTR_BONEINDEX,

   GLATTR_COUNT
};

///////////////////////////////////////////////////////////////////////////////

class GLRShader
{
   DECLARE_ALLOCATOR( GLRShader, AM_DEFAULT );

private:
   const Shader*                 m_shader;
   const GLRenderer&             m_renderer;

   Array< int >                  m_constantsLocations;

   uint                          m_programId;

   // render frame data - gets reset with every call to beginRendering
   uint                          m_bufferBindPoint;
   byte                          m_boundTexturesCount;

public:
   /**
    * Constructor.
    *
    * @param renderer
    * @param shader
    */
   GLRShader( const GLRenderer& renderer, const Shader* shader );
   ~GLRShader();

   /**
   * Compiles the shader.
   */
   void compile();

   /**
    * Returns an index the specified shader constant is bound to.
    *
    * @param constantName
    */
   int getConstantIdx( const char* constantName ) const;

   // -------------------------------------------------------------------------
   // IRenderResourceStorage interface
   // -------------------------------------------------------------------------
   const Shader* getEngineResource() {
      return m_shader;
   }

   // -------------------------------------------------------------------------
   // param setters
   // -------------------------------------------------------------------------
   void setTexture( const char* paramName, uint textureID, const TextureSamplerSettings& samplerSettings );
   void setCubeTexture( const char* paramName, uint textureID, const TextureSamplerSettings& samplerSettings );
   void setDepthBuffer( const char* paramName, uint depthBufferId );
   void setDataBuf( const char* paramName, const Array< byte >& dataBuf, uint bufferId );
   void beginRendering();

private:
   /**
    * Releases loaded shaders.
    */
   void releaseShaders();

   /**
    * Performs the late binding of attributes and uniform variables.
    */
   void bindVars();
};

///////////////////////////////////////////////////////////////////////////////
