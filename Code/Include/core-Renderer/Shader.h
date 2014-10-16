/// @file   core-Renderer\Shader.h
/// @brief  a shader resource
#pragma once

#include "core\Resource.h"
#include "core\types.h"
#include "core\Array.h"
#include "core-Renderer\ShaderProfile.h"
#include "core-Renderer\ShaderRenderCommand.h"
#include "core-Renderer\TextureSamplerSettings.h"


///////////////////////////////////////////////////////////////////////////////

class RenderResource;
class ShaderProfile;

///////////////////////////////////////////////////////////////////////////////

enum ShaderRoutine
{
   SHADER_VERTEX,
   SHADER_TESS_CONTROL,       // hull shader in dx 11
   SHADER_TESS_EVALUATION,    // domain shader in dx 11
   SHADER_GEOMETRY,
   SHADER_FRAGMENT,

   LAST_SHADER_ROUTINE
};

///////////////////////////////////////////////////////////////////////////////

class Shader : public Resource
{
   DECLARE_ALLOCATOR( Shader, AM_DEFAULT );
   DECLARE_RESOURCE();

public:
   RenderResource*                           m_renderResource;

private:
   // static data
   FilePath                                  m_scriptPath;
   mutable ShaderProfile*                    m_shaderProfile;  // it's mutable, 'cause it might be modified when being called 
                                                               // from the constant context of the rendering thread that 
                                                               // will be asking for a profile that doesn't exit.
                                                               // I preferred to treat it this way then having to resave
                                                               // all assets every time I add a library ( which won't happen that often,
                                                               // but by that time I'll probably have a shitload of existing, functional shaders
                                                               // I don't want to loose.
                                                               // Besides - the access to this variable is fully contained, so I can account for how it's gonna be accessed.
                                                                

public:
   /**
    * Default constructor.
    *
    * @param path
    */
   Shader( const FilePath& path = FilePath() );
   ~Shader();

   /**
    * Rebuilds the shader. Call it after making changes to any of the referenced scripts,
    * or changing any of the script paths ( this won't happen automatically ).
    */
   void build();

   /**
    * Returns the script of the specified routine. 
    * 
    * NOTE: The script returned by this method does not have the #include directives unrolled.
    * You need to call the 'getScriptPreprocessed" method 
    *
    * @param lib
    * @param type
    * @param outScript
    */
   void getScript(  ShaderRoutine type, std::string& outScript ) const;

   /**
    * Returns the preprocessed version of the script of the specified routine - with all the #include
    * directives inlined.
    *
    * @param lib
    * @param type
    * @param outScript
    */
   void getScriptPreprocessed( ShaderRoutine type, std::string& outScript ) const;

   /**
    * Sets the script of the specified routine.
    *
    * @param lib
    * @param type
    * @param script
    */
   void setScript( ShaderRoutine type, const std::string& script ) const;

   /**
   * Sets a path to the specified sub-shader code.
   *
   * @param path
   */
   inline void setPath( const FilePath& path ) {
      m_scriptPath = path;
   }

   /**
    * Returns a path to the specified sub-shader file.
    */
   inline const FilePath& getScriptPath() const {
      return m_scriptPath;
   }

   /**
    * Returns an array of constants the shader operates on.
    */
   const Array< ShaderConstantDesc >& getConstants() const;

   // -------------------------------------------------------------------------
   // Resource implementation
   // -------------------------------------------------------------------------
   void replaceContents( Resource& rhs );
   void onResourceLoaded( ResourcesManager& resMgr );

private:
   void reloadShaders( ResourcesManager& mgr );
   ShaderProfile* getShaderProfile() const;
   void parseConstants();
   bool loadScript( const FilePath& path, std::string& outStr ) const;
};

///////////////////////////////////////////////////////////////////////////////

/**
* Binds a vertex shader to a rendering device.
*/
class RCBindShader : public ShaderRenderCommand
{
   DECLARE_ALLOCATOR( RCBindShader, AM_DEFAULT );

public:
   const Shader*           m_shader;

public:
   RCBindShader( const Shader* shader, Renderer& renderer );
   ~RCBindShader();

   // -------------------------------------------------------------------------
   // RenderCommand implementation
   // -------------------------------------------------------------------------
   void execute( Renderer& renderer );
};

///////////////////////////////////////////////////////////////////////////////

/**
* Unbinds a vertex shader from a rendering device.
*/
class RCUnbindShader : public RenderCommand
{
   DECLARE_ALLOCATOR( RCUnbindShader, AM_DEFAULT );

public:

   // -------------------------------------------------------------------------
   // RenderCommand implementation
   // -------------------------------------------------------------------------
   void execute( Renderer& renderer );
};

///////////////////////////////////////////////////////////////////////////////
