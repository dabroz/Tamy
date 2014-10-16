#include "gl-Renderer\GLRShader.h"
#include "gl-Renderer\GLRDefinitions.h"
#include "gl-Renderer\GLRenderer.h"
#include "gl-Renderer\GLRErrorCheck.h"
#include "gl-Renderer\GLRShaderCompilerTool.h"
#include "gl-Renderer\GLRRenderTarget2D.h"
#include "gl-Renderer\GLRRenderTargetCube.h"
#include "core-Renderer\ShaderParam.h"
#include "core-Renderer\Renderer.h"
#include "core\Log.h"


///////////////////////////////////////////////////////////////////////////////

void RCBindShader::execute( Renderer& renderer )
{
   GLRenderer* glRenderer = static_cast< GLRenderer* >( renderer.implementation() );
   GLRShader* glShader = glRenderer->getShader( m_shader );
   if ( !glShader )
   {
      return;
   }

   // set the shader parameters
   glShader->beginRendering();
   setParams( renderer, glShader );
}

///////////////////////////////////////////////////////////////////////////////

void RCUnbindShader::execute( Renderer& renderer )
{
   glUseProgram( 0 );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

GLRShader::GLRShader( const GLRenderer& renderer, const Shader* shader )
   : m_shader( shader )
   , m_renderer( renderer )
{
   m_shader->addReference();

   compile();
}

///////////////////////////////////////////////////////////////////////////////

GLRShader::~GLRShader()
{
   releaseShaders();

   m_shader->removeReference();
   m_shader = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void GLRShader::compile()
{
   m_programId = 0;

   std::string errStr;
   bool result = GLRShaderCompilerTool::compileShader( *m_shader, m_programId, errStr );
   if ( !result )
   {
      WARNING( "Error while compiling shader %s:\n%s", m_shader->getFilePath().c_str(), errStr.c_str() );
   }
   else
   {
      // perform late binding of attributes
      bindVars();
   }
}

///////////////////////////////////////////////////////////////////////////////

void GLRShader::releaseShaders()
{
   if ( m_programId != ( uint ) -1 )
   {
      glDeleteProgram( m_programId );
      m_programId = ( uint ) -1;
   }

   const uint count = m_constantsLocations.size();
   for ( uint i = 0; i < count; ++i )
   {
      m_constantsLocations[i] = ( uint ) -1;
   }
}

///////////////////////////////////////////////////////////////////////////////

void GLRShader::bindVars()
{
   ASSERT( m_programId != ( uint ) -1 );

   // we need to bind the program before we can query for attribute locations
   glUseProgram( m_programId );
   
   // parse the shader constants
   const Array< ShaderConstantDesc >& constants = m_shader->getConstants();
   const uint constantsCount = constants.size();
   m_constantsLocations.resize( constantsCount, -1 );
   for ( uint i = 0; i < constantsCount; ++i )
   {
      const ShaderConstantDesc& constant = constants[i];
      if ( constant.m_type == ShaderConstantDesc::CT_DataBuffer )
      {
         m_constantsLocations[i] = glGetUniformBlockIndex( m_programId, constant.m_name.c_str() );
      }
      else
      {
         m_constantsLocations[i] = glGetUniformLocation( m_programId, constant.m_name.c_str() );
      }
      GLR_HALT_ERRORS();
   }

   // cleanup
   glUseProgram( 0 );
}

///////////////////////////////////////////////////////////////////////////////

int GLRShader::getConstantIdx( const char* constantName ) const
{
   const Array< ShaderConstantDesc >& constants = m_shader->getConstants();
   const uint count = constants.size();
   for ( uint i = 0; i < count; ++i )
   {
      const ShaderConstantDesc& constant = constants[i];
      if ( constant.m_name == constantName )
      {
         return m_constantsLocations[i];
      }
   }

   // the constant doesn't exist
   return -1;
}

///////////////////////////////////////////////////////////////////////////////

static GLint g_addressingModesMap[] = { GL_REPEAT, GL_MIRRORED_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER, GL_MIRROR_CLAMP_TO_EDGE };
static GLint g_filteringModesMap [] = { GL_NEAREST, GL_NEAREST, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_LINEAR };

///////////////////////////////////////////////////////////////////////////////

void GLRShader::setTexture( const char* paramName, uint textureID, const TextureSamplerSettings& samplerSettings )
{
   const int constantIdx = getConstantIdx( paramName );
   if ( constantIdx >= 0 )
   {
      glActiveTexture( GL_TEXTURE0 + m_boundTexturesCount );
      glBindTexture( GL_TEXTURE_2D, textureID );
      glUniform1i( constantIdx, m_boundTexturesCount );
      ++m_boundTexturesCount;

      //set texture parameters
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, g_filteringModesMap[samplerSettings.m_minFilter] );
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, g_filteringModesMap[samplerSettings.m_magFilter] );
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, g_addressingModesMap[samplerSettings.m_addressU] );
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, g_addressingModesMap[samplerSettings.m_addressV] );
   }
}

///////////////////////////////////////////////////////////////////////////////

void GLRShader::setCubeTexture( const char* paramName, uint textureID, const TextureSamplerSettings& samplerSettings )
{
   const int constantIdx = getConstantIdx( paramName );
   if ( constantIdx >= 0 )
   {
      glActiveTexture( GL_TEXTURE0 + m_boundTexturesCount );
      glBindTexture( GL_TEXTURE_CUBE_MAP, textureID );
      glUniform1i( constantIdx, m_boundTexturesCount );
      ++m_boundTexturesCount;

      //set texture parameters
      glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, g_filteringModesMap[samplerSettings.m_minFilter] );
      glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, g_filteringModesMap[samplerSettings.m_magFilter] );
      glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
      glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
      glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE );
      glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BASE_LEVEL, 0 );
      glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, 0 );
   }
}

///////////////////////////////////////////////////////////////////////////////

void GLRShader::setDepthBuffer( const char* paramName, uint depthBufferId )
{
   const int constantIdx = getConstantIdx( paramName );
   if ( constantIdx >= 0 )
   {
      glActiveTexture( GL_TEXTURE0 + m_boundTexturesCount );
      glBindTexture( GL_TEXTURE_2D, depthBufferId );
      glUniform1i( constantIdx, m_boundTexturesCount );
      ++m_boundTexturesCount;

      //set texture parameters
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE );
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0 );
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0 );
   }
}

///////////////////////////////////////////////////////////////////////////////

void GLRShader::setDataBuf( const char* paramName, const Array< byte >& dataBuf, uint bufferId )
{
   const int blockIdx = getConstantIdx( paramName );
   if ( blockIdx >= 0 )
   {
      // copy the data to the buffer
      glBindBuffer( GL_UNIFORM_BUFFER, bufferId );
      glBufferData( GL_UNIFORM_BUFFER, dataBuf.size(), (const byte*)dataBuf, GL_DYNAMIC_DRAW );
      glBindBuffer( GL_UNIFORM_BUFFER, 0 );
      GLR_HALT_ERRORS();

      // bind the buffer to the program
      glUniformBlockBinding( m_programId, blockIdx, m_bufferBindPoint );
      GLR_HALT_ERRORS();

      // bind the buffer object to the uniform block
      glBindBufferBase( GL_UNIFORM_BUFFER, m_bufferBindPoint, bufferId );
      GLR_HALT_ERRORS();

      ++m_bufferBindPoint;
   }
}

///////////////////////////////////////////////////////////////////////////////

void GLRShader::beginRendering()
{
   // reset the render frame data
   m_boundTexturesCount = 0;
   m_bufferBindPoint = 0;

   glUseProgram( m_programId );
   GLR_HALT_ERRORS();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void ShaderParamRenderTarget2D::setParam( Renderer& renderer, void* shaderPtr )
{
   GLRenderer* glRenderer = static_cast< GLRenderer* >( renderer.implementation() );
   GLRRenderTarget2D* renderTarget = glRenderer->getRenderTarget( m_val );

   if ( renderTarget )
   {
      uint textureID = renderTarget->getTexture( m_renderTextureIdx );

      GLRShader* glShader = reinterpret_cast< GLRShader* >( shaderPtr );
      glShader->setTexture( m_name.c_str(), textureID, m_samplerSettings );
   }
}

///////////////////////////////////////////////////////////////////////////////

void ShaderParamDepthBuffer2D::setParam( Renderer& renderer, void* shaderPtr )
{
   GLRenderer* glRenderer = static_cast< GLRenderer* >( renderer.implementation() );
   GLRRenderTarget2D* renderTarget = glRenderer->getRenderTarget( m_val );

   if ( renderTarget )
   {
      uint depthBufferId = renderTarget->getDepthBuffer();

      GLRShader* glShader = reinterpret_cast< GLRShader* >( shaderPtr );
      glShader->setDepthBuffer( m_name.c_str(), depthBufferId );
   }
}

///////////////////////////////////////////////////////////////////////////////

void ShaderParamRenderTargetCube::setParam( Renderer& renderer, void* shaderPtr )
{
   GLRenderer* glRenderer = static_cast< GLRenderer* >( renderer.implementation() );
   GLRRenderTargetCube* renderTarget = glRenderer->getRenderTarget( m_val );

   uint textureID = renderTarget->getTexture();

   GLRShader* glShader = reinterpret_cast< GLRShader* >( shaderPtr );
   glShader->setCubeTexture( m_name.c_str(), textureID, m_samplerSettings );
}

///////////////////////////////////////////////////////////////////////////////

void ShaderParamTexture::setParam( Renderer& renderer, void* shaderPtr )
{
   GLRenderer* glRenderer = static_cast< GLRenderer* >( renderer.implementation() );
   uint textureID = glRenderer->getTexture( m_val );

   GLRShader* glShader = reinterpret_cast< GLRShader* >( shaderPtr );
   glShader->setTexture( m_name.c_str(), textureID, m_samplerSettings );
}

///////////////////////////////////////////////////////////////////////////////

void ShaderParamCubeTexture::setParam( Renderer& renderer, void* shaderPtr )
{
   GLRenderer* glRenderer = static_cast< GLRenderer* >( renderer.implementation() );
   uint textureID = glRenderer->getCubeTexture( m_val );

   GLRShader* glShader = reinterpret_cast< GLRShader* >( shaderPtr );
   glShader->setCubeTexture( m_name.c_str(), textureID, m_samplerSettings );
}

///////////////////////////////////////////////////////////////////////////////

void ShaderParamProceduralTexture::setParam( Renderer& renderer, void* shaderPtr )
{
   GLRenderer* glRenderer = static_cast< GLRenderer* >( renderer.implementation() );
   uint textureID = glRenderer->getProceduralTexture( m_val );

   GLRShader* glShader = reinterpret_cast< GLRShader* >( shaderPtr );
   glShader->setTexture( m_name.c_str(), textureID, m_samplerSettings );
}

///////////////////////////////////////////////////////////////////////////////

void ShaderParamDataBuf::setParam( Renderer& renderer, void* shaderPtr )
{
   GLRenderer* glRenderer = static_cast< GLRenderer* >( renderer.implementation() );
   uint bufferId = glRenderer->getShaderDataBuffer( m_dataBuf );

   GLRShader* glShader = reinterpret_cast< GLRShader* >( shaderPtr );
   glShader->setDataBuf( m_name.c_str(), m_data, bufferId );
}

///////////////////////////////////////////////////////////////////////////////
