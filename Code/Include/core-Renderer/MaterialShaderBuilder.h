/// @file   core-Renderer/MaterialShaderBuilder.h
/// @brief  class used by Material class to build pixel shader code
#ifndef _MATERIAL_SHADER_BUILDER_H
#define _MATERIAL_SHADER_BUILDER_H

#include <string>
#include <map>
#include <set>
#include "core\Vector.h"
#include "core\List.h"
#include "core\MemoryRouter.h"
#include "core\GraphBuilderSockets.h"
#include "core-Renderer\MaterialFunction.h"
#include "core-Renderer\MaterialEntryStruct.h"
#include "core-Renderer\Defines.h"


///////////////////////////////////////////////////////////////////////////////

class Shader;
class MaterialNode;
class MaterialShaderLayout;
typedef GBNodeInput< MaterialNode > MNInput;
typedef GBNodeOutput< MaterialNode > MNOutput;
enum ShaderRoutine;
class MaterialEntryStruct;
class GBNodeSocket;
class MaterialCodeNode;
class MaterialDataStruct;
class MaterialFunctionCall;
class MaterialCodeLine;
class MaterialConstantsBlock;

///////////////////////////////////////////////////////////////////////////////

class MaterialLanguageCode
{
   DECLARE_ALLOCATOR( MaterialLanguageCode, AM_DEFAULT );
private:
   std::string                m_code;
   List< MaterialFunction* >  m_functions;

public:
   void addCode( const std::string& code );

   MaterialFunction* addFunction( const std::string& name );
};

///////////////////////////////////////////////////////////////////////////////

/**
 * This class will compile a shader ( and probably a group of shaders for particular render passes
 * in the near future ) that renders a material.
 *
 * Every rendering pass will use a certain number of render targets the shader needs to know about.
 * That's why this class defines HLSL structures PS_OUTPUT that defines the order and application
 * of each render target, so that the shader knows where to put its data.
 *
 * If it turns out we need additional render targets in our pipeline, be sure to update
 * MaterialSceneRenderer so that it activates them.
 */
class MaterialShaderBuilder
{
   DECLARE_ALLOCATOR( MaterialShaderBuilder, AM_DEFAULT );

private:
   typedef std::map< const MNOutput*, std::string > VariablesMap;

   ShaderRoutine                 m_routine;
   MaterialConstantsBlock*       m_constantsBlock;
   VariablesMap                  m_variablesMap;
   List< MaterialDataStruct* >   m_structureDeclarations;
   List< MaterialFunction* >     m_functionDeclarations;
   List< std::string >           m_defines;
   List< std::string >           m_temporaryVariables;
   List< std::string >           m_samplersList;
   MaterialEntryStruct           m_inputStruct;
   MaterialEntryStruct           m_outputStruct;

   List< MaterialCodeNode* >     m_constants;
   std::string                   m_samplers;
   List< MaterialCodeNode* >     m_code;
   std::set< std::string >       m_localVariables;

public:
   /**
    * Constructor.
    *
    * @param routine
    */
   MaterialShaderBuilder( ShaderRoutine routine );
   ~MaterialShaderBuilder();

   /**
    * Compiles a shader.
    *
    * @param shader
    * @param outConstantsBufSize
    * @param outConstantsBlockName
    */
   void compile( Shader* shader, uint& outConstantsBufSize, std::string& outConstantsBlockName );

   // -------------------------------------------------------------------------
   // Code
   // -------------------------------------------------------------------------
   /**
    * Adds a new line of code.
    *
    * @param code ( printf-style format codes accepted )
    * @param variable length arguments list of substitutes for the formatting tokens supplied in the 'code' string
    */
   void addCodeLine( const char* code, ... );

   /**
    * Adds a custom line of code. Call this method if the HLSL version differs from the GLSL version.
    */
   MaterialCodeLine* addCodeLine();

   // -------------------------------------------------------------------------
   // Structures
   // -------------------------------------------------------------------------
   /**
    * Returns the routine's input structure.
    */
   inline MaterialEntryStruct& getInputStruct() {
      return m_inputStruct;
   }

   /**
   * Returns the routine's output structure.
   */
   inline MaterialEntryStruct& getOutputStruct() {
      return m_outputStruct;
   }

   /**
    * Creates a custom data structure
    *
    * @param structName
    * @param fieldsCount
    */
   MaterialDataStruct* createStructure( const char* structName, const uint fieldsCount );

   // -------------------------------------------------------------------------
   // Variables
   // -------------------------------------------------------------------------

   /**
    * Adds a new 2d texture sampler.
    */
   const char* addSampler();

   /**
   * Adds a new cube texture sampler.
   */
   const char* addCubeSampler();

   /**
    * Adds a new global variable and defines a name for it.
    *
    * @param output
    * @return data offset
    */
   uint addGlobal( const MNOutput* output );

   /**
    * Adds a global that's will be operated from inside of a node only ( through its properties for instance ).
    *
    * @param outOffset     data offset
    * @param outName       name of the variable
    */
   template< typename T >
   void addGlobal( uint& outOffset, std::string& outName );

   /**
    * Adds a new constant variable and defines a name for it.
    *
    * @param output
    * @param value
    */
   template< typename T >
   const char* addConstant( const MNOutput* output, const T& value );

   /**
    * Reserves a unique name for a local variable.
    *
    * @param name       desired variable name   
    * return            unique variable name
    */
   template< typename T >
   const char* addLocalVariable( const char* name  );

   /**
    * Adds a new variable and defines a name for it.
    *
    * CAUTION: unlike other methods, this one will not define the variable. The user 
    * needs to do it himself using the 'addCodeLine' method
    *
    * @param output
    */
   const char* addVariable( const MNOutput* output );

   /**
    * Call this function to define a variable that will correspond to the name of a field in the INPUT structure.
    *
    * @param output
    * @param 
    */
   void addStructVariable( const GBNodeOutput< MaterialNode >* output, const char* tag );


   /**
    * Checks if the specified input is represented by a variable ( defined for the output it's connected to ).
    *
    * @param input
    */
   bool isVariableDefined( const MNInput* input ) const;

   /**
    * Returns the name under which a variable assigned to the specified input is registered.
    */
   const char* getVariable( const MNInput* input ) const;

   // -------------------------------------------------------------------------
   // Functions
   // -------------------------------------------------------------------------

   /**
    * Returns an existing function definition.
    */
   MaterialFunction* getFunction( const char* functionName );

   /**
    * Adds a function defined in a text file.
    *
    * @param functionFile
    */
   MaterialFunction* addFunction( const class FilePath& functionFile );

   /**
    * Adds function.
    *
    * @param functionName
    * @param inputParamsCount
    */
   MatFuncUserDefined* addFunction( const char* functionName, const uint inputParamsCount );

   /**
    * Inserts a call to a function.
    *
    * @param function
    */
   MaterialFunctionCall* callFunction( MaterialFunction* function );

   // -------------------------------------------------------------------------
   // Types resolution
   // -------------------------------------------------------------------------
   /**
    * Maps a socket type to a shader type.
    *
    * @param socketType
    */
   static const char* mapType( const GBNodeSocket* socketType );

   /**
   * Maps an engine type to a shader type.
   *
   * @param engineType
   */
   static const char* mapType( const ReflectionType* engineType );

   /**
    * Returns the data size of the specified engine type.
    *
    * @param engineType
    */
   static uint getTypeSize( const ReflectionType* engineType );

private:
   /**
    * Replaces common tokens used by the builder with the actual corresponding pieces of code.
    *
    * @param code
    */
   void replaceTokens( std::string& code );

   // -------------------------------------------------------------------------
   // Shader routines compilation
   // -------------------------------------------------------------------------
   /**
    * Compiles the shader routine.
    *
    * @param outCode
    */
   void compileRoutine( std::string& outCode );

   /**
   * Compiles the main function
   */
   void compileMainFunc( std::string& outCode ) const;
};

///////////////////////////////////////////////////////////////////////////////

#include "core-Renderer\MaterialShaderBuilder.inl"

///////////////////////////////////////////////////////////////////////////////

#endif // _MATERIAL_SHADER_BUILDER_H
