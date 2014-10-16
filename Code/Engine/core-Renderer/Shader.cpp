#include "core-Renderer\Shader.h"
#include "core-Renderer\Renderer.h"
#include "core-Renderer\RenderResource.h"
#include "core-Renderer\ShaderTexture.h"
#include "core-Renderer\ShaderCompiler.h"
#include "core\TSContinuousMemoryPool.h"
#include "core\StreamBuffer.h"
#include "core\Filesystem.h"
#include "core\StringParser.h"
#include "core\Log.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_RESOURCE( Shader, tsh, AM_BINARY );
   PROPERTY( FilePath, m_scriptPath );
   PROPERTY( ShaderProfile*, m_shaderProfile );
END_RESOURCE();

///////////////////////////////////////////////////////////////////////////////

Shader::Shader( const FilePath& path )
   : Resource( path )
   , m_shaderProfile( NULL )
{
   m_renderResource = RenderResource::instantiate< Shader >( this );
}

///////////////////////////////////////////////////////////////////////////////

Shader::~Shader()
{
   if ( m_shaderProfile )
   {
      m_shaderProfile->removeReference();
      m_shaderProfile = NULL;
   }

   m_renderResource->removeReference();
   m_renderResource = NULL;
}

///////////////////////////////////////////////////////////////////////////////

ShaderProfile* Shader::getShaderProfile() const
{
   if ( !m_shaderProfile )
   {
      m_shaderProfile = new ShaderProfile();
   }

   return m_shaderProfile;
}

///////////////////////////////////////////////////////////////////////////////

void Shader::parseConstants()
{
   ShaderProfile* profile = getShaderProfile();

   Array< ShaderConstantDesc >& constants = profile->accessConstants();
   constants.clear();

   ShaderCompiler compiler;
   compiler.parseConstants( *this, constants );
}

///////////////////////////////////////////////////////////////////////////////

void Shader::build()
{
   parseConstants();
   m_renderResource->setDirty();
}

///////////////////////////////////////////////////////////////////////////////

const char* g_scriptSectionTag[LAST_SHADER_ROUTINE] = 
{
   "<<SHADER_VERTEX_GLSL>>",
   "<<SHADER_TESS_CONTROL_GLSL>>",
   "<<SHADER_TESS_EVALUATION_GLSL>>",
   "<<SHADER_GEOMETRY_GLSL>>",
   "<<SHADER_FRAGMENT_GLSL>>",
};

const char* g_sectionEndMarker = "<<SHADER_ROUTINE_END>>";

///////////////////////////////////////////////////////////////////////////////

void Shader::getScript( ShaderRoutine type, std::string& outScript ) const
{
   outScript = "";

   Filesystem& fs = TSingleton< Filesystem >::getInstance();
   if ( m_scriptPath.empty() )
   {
      return;
   }

   File* file = fs.open( m_scriptPath, std::ios_base::in | std::ios_base::binary );
   if ( file == NULL )
   {
      return;
   }

   StreamBuffer<char> fileContentsBuf( *file );
   std::string scriptCode = fileContentsBuf.getBuffer();
   delete file;

   // find a dedicated section and load it
   int sectionStartPos = scriptCode.find( g_scriptSectionTag[type] );
   if ( sectionStartPos == std::string::npos )
   {
      // the section doesn't exist, so the code's just not there
      return;
   }

   sectionStartPos += strlen( g_scriptSectionTag[type] );

   int sectionEndPos = scriptCode.find( g_sectionEndMarker, sectionStartPos );
   ASSERT( sectionEndPos != std::string::npos ); // every section needs to end with an ending tag

   outScript = scriptCode.substr( sectionStartPos, sectionEndPos - sectionStartPos );
}

///////////////////////////////////////////////////////////////////////////////

void Shader::getScriptPreprocessed( ShaderRoutine type, std::string& outScript ) const
{
   getScript( type, outScript );

   StringParser parser( &outScript );
   std::size_t directiveStart, directiveEnd;
   while ( parser.findNext( "#include", directiveStart ) )
   {
      std::string pathToProcess;
      parser.getNextWord( pathToProcess, directiveEnd );

      std::size_t unprocessedPathLength = pathToProcess.length();
      directiveEnd += unprocessedPathLength;

      // confirm that this is a valid path - that it's embraced with quotation marks that is
      if ( unprocessedPathLength < 2 || pathToProcess[0] != '"' || pathToProcess[unprocessedPathLength - 1] != '"' )
      {
         LOG( "Shader::getScriptPreprocessed - invalid #include directive found" );
         return;
      }

      FilePath pathToInclude = pathToProcess.substr( 1, pathToProcess.length() - 2 );

      // load the script from the file
      std::string includedScript;
      if ( !loadScript( pathToInclude, includedScript ) )
      {
         return;
      }

      // replace the directive with the included script
      std::string str1 = outScript.substr( 0, directiveStart );
      std::string str2 = outScript.substr( directiveEnd );
      outScript = str1 + includedScript + str2;

      // start over
      parser.restart();
   }
}

///////////////////////////////////////////////////////////////////////////////

bool Shader::loadScript( const FilePath& path, std::string& outStr ) const
{
   Filesystem& fs = TSingleton< Filesystem >::getInstance();
   File* file = fs.open( path, std::ios_base::in | std::ios_base::binary );
   if ( file == NULL )
   {
      LOG( "Shader::loadScript - can't open the file '%s'", path.c_str() );
      return false;
   }

   StreamBuffer<char> fileContentsBuf( *file );
   outStr = fileContentsBuf.getBuffer();
   delete file;

   return true;
}

///////////////////////////////////////////////////////////////////////////////

void Shader::setScript( ShaderRoutine type, const std::string& script ) const
{
   Filesystem& fs = TSingleton< Filesystem >::getInstance();
   if ( m_scriptPath.empty() )
   {
      return;
   }

   std::string scriptCode;
   File* file = fs.open( m_scriptPath, std::ios_base::in | std::ios_base::binary );
   if ( file != NULL )
   {
      StreamBuffer<char> fileContentsBuf( *file );
      scriptCode = fileContentsBuf.getBuffer();
      delete file;

      // find a dedicated section, and if it already exists, we're gonna delete it and replace it with a new one
      int sectionStartPos = scriptCode.find( g_scriptSectionTag[type] );
      if ( sectionStartPos != std::string::npos )
      {
         int sectionEndPos = scriptCode.find( g_sectionEndMarker, sectionStartPos + strlen( g_scriptSectionTag[type] ) );
         ASSERT( sectionEndPos != std::string::npos ); // every section needs to end with an ending tag
         sectionEndPos += strlen( g_sectionEndMarker );

         // put the remainder of the script together without the erased part
         std::string scriptFrontPart = scriptCode.substr( 0, sectionStartPos );
         std::string scriptRearPart = scriptCode.substr( sectionEndPos );
         scriptCode = scriptFrontPart + scriptRearPart;
      }
   }

   // add the new part
   scriptCode += g_scriptSectionTag[type];
   scriptCode += script;
   scriptCode += g_sectionEndMarker;

   // save the file
   file = fs.open( m_scriptPath, std::ios_base::out | std::ios_base::binary );
   if ( file == NULL )
   {
      LOG( "File %s can't be opened for writing", m_scriptPath.c_str() );
      return;
   }

   file->writeString( scriptCode.c_str() );
   delete file;
}

///////////////////////////////////////////////////////////////////////////////

const Array< ShaderConstantDesc >& Shader::getConstants() const
{
   ShaderProfile* profile = getShaderProfile();
   return profile->getConstants();
}

///////////////////////////////////////////////////////////////////////////////

void Shader::replaceContents( Resource& rhs )
{
   Shader& rhsShader = static_cast< Shader& >( rhs );

   m_scriptPath = rhsShader.m_scriptPath;
   m_shaderProfile = rhsShader.m_shaderProfile;

   // clear the shader profiles we've just taken the ownership of, otherwise they'll get deleted
   // in a sec
   rhsShader.m_shaderProfile = NULL;

   // rebuild the shader
   build();
}

///////////////////////////////////////////////////////////////////////////////

void Shader::onResourceLoaded( ResourcesManager& resMgr )
{
   Resource::onResourceLoaded( resMgr );

   if ( m_scriptPath.empty() )
   {
      // assign an initial path
      getFilePath().changeFileExtension( "code", m_scriptPath );
   }

   parseConstants();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

RCBindShader::RCBindShader( const Shader* shader, Renderer& renderer )
   : ShaderRenderCommand( *renderer.rtMemPool() )
   , m_shader( shader )
{
}

///////////////////////////////////////////////////////////////////////////////

RCBindShader::~RCBindShader()
{
}

///////////////////////////////////////////////////////////////////////////////
