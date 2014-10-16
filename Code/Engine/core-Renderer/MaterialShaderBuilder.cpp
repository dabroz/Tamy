#include "core-Renderer\MaterialShaderBuilder.h"
#include "core-Renderer\Shader.h"
#include "core-Renderer\MaterialSockets.h"
#include "core-Renderer\MaterialCodeNode.h"
#include "core-Renderer\MaterialDataStruct.h"
#include "core-Renderer\MaterialConstantsBlock.h"
#include "core\StringUtils.h"
#include <stdarg.h>

// files
#include "core\Filesystem.h"
#include "core\File.h"
#include "core\StreamBuffer.h"

// logging
#include "core\Log.h"


///////////////////////////////////////////////////////////////////////////////

const char* HEADER = "#version 330 core";

const char* INPUT_OUTPUT_STRUCTS =
"// ----------------------------------------------------------------------------\n\
// Input/Output structures\n\
// ----------------------------------------------------------------------------\n";

const char* CONSTANTS_HEADER =
"// ----------------------------------------------------------------------------\n\
// Constants\n\
// ----------------------------------------------------------------------------\n\n";

const char* GLOBALS_HEADER =
"// ----------------------------------------------------------------------------\n\
// Globals\n\
// ----------------------------------------------------------------------------\n\n";

const char* SAMPLERS_HEADER = 
"// ----------------------------------------------------------------------------\n\
// Samplers\n\
// ----------------------------------------------------------------------------\n\n";

const char* FUNCTIONS_HEADER =
"// ----------------------------------------------------------------------------\n\
// Functions\n\
// ----------------------------------------------------------------------------\n\n";

const char* g_routineConstantsBlockName [] = {
   "VertexShaderConstants",
   "TessCtrlShaderConstants",
   "TessEvalShaderConstants",
   "GeometryShaderConstants",
   "FragmentShaderConstants",
};

///////////////////////////////////////////////////////////////////////////////

MaterialShaderBuilder::MaterialShaderBuilder( ShaderRoutine routine )
   : m_routine( routine )
   , m_constantsBlock( new MaterialConstantsBlock( g_routineConstantsBlockName[m_routine] ) )
   , m_inputStruct( true, routine )
   , m_outputStruct( false, routine )
{
}

///////////////////////////////////////////////////////////////////////////////

MaterialShaderBuilder::~MaterialShaderBuilder()
{
   delete m_constantsBlock;
   m_constantsBlock = NULL;

   for ( List< MaterialFunction* >::iterator it = m_functionDeclarations.begin(); !it.isEnd(); ++it )
   {
      MaterialFunction* function = *it;
      delete function;
   }
   m_functionDeclarations.clear();

   for ( List< MaterialDataStruct* >::iterator it = m_structureDeclarations.begin(); !it.isEnd(); ++it )
   {
      MaterialDataStruct* structure = *it;
      delete structure;
   }
   m_structureDeclarations.clear();

   for ( List< MaterialCodeNode* >::iterator it = m_constants.begin(); !it.isEnd(); ++it )
   {
      MaterialCodeNode* constantNode = *it;
      delete constantNode;
   }
   m_constants.clear();

   for ( List< MaterialCodeNode* >::iterator it = m_code.begin(); !it.isEnd(); ++it )
   {
      MaterialCodeNode* codeNode = *it;
      delete codeNode;
   }
   m_code.clear();
}

///////////////////////////////////////////////////////////////////////////////

void MaterialShaderBuilder::compile( Shader* shader, uint& outConstantsBufSize, std::string& outConstantsBlockName )
{
   if ( m_code.empty() )
   {
      // save both files as empty
      shader->setScript( m_routine, "" );

      outConstantsBufSize = 0;
      outConstantsBlockName = "EmptyConstantsBlock";
   }
   else
   {
      std::string code;
      compileRoutine( code );
      replaceTokens( code );
      shader->setScript( m_routine, code );

      outConstantsBufSize = m_constantsBlock->calculateBufSize();
      outConstantsBlockName = m_constantsBlock->getName();
   }
}

///////////////////////////////////////////////////////////////////////////////

void MaterialShaderBuilder::replaceTokens( std::string& code )
{

   code = StringUtils::replace( code, "<type_Color>", "vec4" );
   code = StringUtils::replace( code, "<type_Mtx4x4>", "mat4" );
   code = StringUtils::replace( code, "<type_Mtx4x4Arr>", "mat4" );
   code = StringUtils::replace( code, "<type_Mtx3x3>", "mat3" );
   code = StringUtils::replace( code, "<type_Vector2>", "vec2" );
   code = StringUtils::replace( code, "<type_Vector3>", "vec3" );
   code = StringUtils::replace( code, "<type_Vector4>", "vec4" );
   code = StringUtils::replace( code, "<type_Sampler2D>", "sampler2D" );
   code = StringUtils::replace( code, "<type_SamplerCube>", "samplerCube" );

   m_inputStruct.replaceTokens( code );
   m_outputStruct.replaceTokens( code );
}

///////////////////////////////////////////////////////////////////////////////

void MaterialShaderBuilder::compileRoutine( std::string& code )
{
   code = HEADER;
   code += INPUT_OUTPUT_STRUCTS;

   m_inputStruct.compile( code );
   m_outputStruct.compile( code );

   for ( List< MaterialDataStruct* >::iterator it = m_structureDeclarations.begin(); !it.isEnd(); ++it )
   {
      MaterialDataStruct* structure = *it;
      structure->compile( code );
   }

   code += CONSTANTS_HEADER;
   for ( List< MaterialCodeNode* >::iterator it = m_constants.begin(); !it.isEnd(); ++it )
   {
      MaterialCodeNode* constantNode = *it;
      constantNode->compile( code );
   }

   code += GLOBALS_HEADER;
   m_constantsBlock->compile( code );

   code += SAMPLERS_HEADER;
   code += m_samplers;

   code += FUNCTIONS_HEADER;

   for ( List< MaterialFunction* >::iterator it = m_functionDeclarations.begin(); !it.isEnd(); ++it )
   {
      MaterialFunction* function = *it;
      function->compile( code );
   }

   compileMainFunc( code );
}

///////////////////////////////////////////////////////////////////////////////

static const char* g_mainFunctionStart =
"\n// ----------------------------------------------------------------------------\n\
// ----------------------------------------------------------------------------\n\n\
void main()\n\
{\n";

static const char* g_mainFunctionEnd = "}\n";

void MaterialShaderBuilder::compileMainFunc( std::string& outCode ) const
{
   outCode += g_mainFunctionStart;

   for ( List< MaterialCodeNode* >::const_iterator it = m_code.begin(); !it.isEnd(); ++it )
   {
      MaterialCodeNode* codeNode = *it;
      codeNode->compile( outCode );
   }

   outCode += g_mainFunctionEnd;
}

///////////////////////////////////////////////////////////////////////////////

void MaterialShaderBuilder::addCodeLine( const char* code, ... )
{
   char formattedStr[1024];

   va_list argptr;
   va_start( argptr, code );
   vsprintf_s( formattedStr, code, argptr );
   va_end( argptr );

   m_code.pushBack( new MaterialCodeLine( formattedStr ) );
}

///////////////////////////////////////////////////////////////////////////////

MaterialCodeLine* MaterialShaderBuilder::addCodeLine()
{
   MaterialCodeLine* codeLine = new MaterialCodeLine();
   m_code.pushBack( codeLine );

   return codeLine;
}

///////////////////////////////////////////////////////////////////////////////

MaterialDataStruct* MaterialShaderBuilder::createStructure( const char* structName, const uint fieldsCount )
{
   MaterialDataStruct* structure = new MaterialDataStruct( *this, structName, fieldsCount );
   m_structureDeclarations.pushBack( structure );

   return structure;
}

///////////////////////////////////////////////////////////////////////////////

void MaterialShaderBuilder::addStructVariable( const GBNodeOutput< MaterialNode >* output, const char* tag )
{
   VariablesMap::const_iterator it = m_variablesMap.find( output );

   if ( it == m_variablesMap.end() )
   {
      it = m_variablesMap.insert( std::make_pair( output, tag ) ).first;
   }
}

///////////////////////////////////////////////////////////////////////////////

const char* MaterialShaderBuilder::addVariable( const GBNodeOutput< MaterialNode >* output )
{
   VariablesMap::const_iterator it = m_variablesMap.find( output );

   if ( it == m_variablesMap.end() )
   {
      char tmpVarName[64];
      uint nextFreeVariableIdx = m_variablesMap.size();
      sprintf_s( tmpVarName, "%s_%03d", output->getName().c_str(), nextFreeVariableIdx );

      it = m_variablesMap.insert( std::make_pair( output, tmpVarName ) ).first;
   }

   return it->second.c_str();
}

///////////////////////////////////////////////////////////////////////////////

const char* MaterialShaderBuilder::addSampler()
{
   char tmpSamplerName[64];
   uint nextFreeSamplerIdx = m_samplersList.size();
   sprintf_s( tmpSamplerName, "sampler_%02d", nextFreeSamplerIdx );

   m_samplersList.pushBack( tmpSamplerName );

   m_samplers += std::string( "uniform <type_Sampler2D>\t\t" ) + tmpSamplerName + ";\n";

   return m_samplersList.back().c_str();
}

///////////////////////////////////////////////////////////////////////////////

const char* MaterialShaderBuilder::addCubeSampler()
{
   char tmpSamplerName[64];
   uint nextFreeSamplerIdx = m_samplersList.size();
   sprintf_s( tmpSamplerName, "sampler_%02d", nextFreeSamplerIdx );

   m_samplersList.pushBack( tmpSamplerName );

   m_samplers += std::string( "uniform <type_SamplerCube>\t\t" ) + tmpSamplerName + ";\n";

   return m_samplersList.back().c_str();

}

///////////////////////////////////////////////////////////////////////////////

uint MaterialShaderBuilder::addGlobal( const GBNodeOutput< MaterialNode >* output )
{
   const SerializableReflectionType* engineType = &output->getVirtualRTTI();
   const char* varName = addVariable( output );

   uint arrSize = 1;
   std::string typeStr = mapType( engineType );
   if ( typeStr.find( "Arr" ) != std::string::npos )
   {
      arrSize = 54;
   }
   uint dataOffset = 0;
   m_constantsBlock->add( engineType, varName, arrSize, dataOffset );
   return dataOffset;
}

///////////////////////////////////////////////////////////////////////////////

bool MaterialShaderBuilder::isVariableDefined( const GBNodeInput< MaterialNode >* input ) const
{
   GBNodeOutput< MaterialNode >* output = input->getOutput();
   VariablesMap::const_iterator it = m_variablesMap.find( output );

   bool doesExist = it != m_variablesMap.end();
   return doesExist;
}

///////////////////////////////////////////////////////////////////////////////

const char* MaterialShaderBuilder::getVariable( const GBNodeInput< MaterialNode >* input ) const
{
   GBNodeOutput< MaterialNode >* output = input->getOutput();
   if ( output )
   {
      VariablesMap::const_iterator it = m_variablesMap.find( output );
      if ( it != m_variablesMap.end() )
      {
         return it->second.c_str();
      }
   }

   return NULL;
}

///////////////////////////////////////////////////////////////////////////////

MaterialFunction* MaterialShaderBuilder::getFunction( const char* functionName )
{
   for ( List< MaterialFunction* >::iterator it = m_functionDeclarations.begin(); !it.isEnd(); ++it )
   {
      MaterialFunction* function = *it;
      if ( function->getName() == functionName )
      {
         return function;
      }
   }

   return NULL;
}

///////////////////////////////////////////////////////////////////////////////

MaterialFunction* MaterialShaderBuilder::addFunction( const FilePath& functionFile )
{
   Filesystem& fs = TSingleton< Filesystem >::getInstance();

   File* file = fs.open( functionFile );
   StreamBuffer<char> fileReader( *file );
   std::string fileContents = fileReader.getBuffer();
   delete file;
   

   MaterialDataStruct* inputStruct = new MaterialDataStruct( *this, fileContents );
   m_structureDeclarations.pushBack( inputStruct );

   MatFuncPrecompiled* function = new MatFuncPrecompiled( *this, fileContents );
   function->setInputStruct( inputStruct );
   m_functionDeclarations.pushBack( function );

   return function;
}

///////////////////////////////////////////////////////////////////////////////

MatFuncUserDefined* MaterialShaderBuilder::addFunction( const char* functionName, const uint inputParamsCount )
{
   char tmpStructName[64];
   sprintf_s( tmpStructName, "%sStruct", functionName );
   MaterialDataStruct* inputStruct = createStructure( tmpStructName, inputParamsCount );

   MatFuncUserDefined* function = new MatFuncUserDefined( *this );
   function->setFunctionName( functionName );
   function->setInputStruct( inputStruct );
   m_functionDeclarations.pushBack( function );
   
   return function;
}

///////////////////////////////////////////////////////////////////////////////

MaterialFunctionCall* MaterialShaderBuilder::callFunction( MaterialFunction* function )
{
   MaterialFunctionCall* node = new MaterialFunctionCall( function, *this );
   m_code.pushBack( node );

   return node;
}

///////////////////////////////////////////////////////////////////////////////

const char* MaterialShaderBuilder::mapType( const GBNodeSocket* socketType )
{
   const SerializableReflectionType* engineType = &socketType->getVirtualRTTI();
   return mapType( engineType );
}

///////////////////////////////////////////////////////////////////////////////

const char* MaterialShaderBuilder::mapType( const ReflectionType* engineType )
{
   if ( engineType )
   {
      if ( engineType->m_name == "MSVec2Input" || engineType->m_name == "MSVec2Output" )
      {
         return "<type_Vector2>";
      }
      else if ( engineType->m_name == "MSVec3Input" || engineType->m_name == "MSVec3Output" )
      {
         return "<type_Vector3>";
      }
      else if ( engineType->m_name == "MSVec4Input" || engineType->m_name == "MSVec4Output" )
      {
         return "<type_Vector4>";
      }
      else if ( engineType->m_name == "MSColorInput" || engineType->m_name == "MSColorOutput" )
      {
         return "<type_Color>";
      }
      else if ( engineType->m_name == "MSMatrixInput" || engineType->m_name == "MSMatrixOutput" )
      {
         return "<type_Mtx4x4>";
      }
      else if ( engineType->m_name == "MSMatrixArrInput" || engineType->m_name == "MSMatrixArrOutput" )
      {
         return "<type_Mtx4x4Arr>";
      }
      else if ( engineType->m_name == "MSFloatInput" || engineType->m_name == "MSFloatOutput" )
      {
         return "float";
      }
      else if ( engineType->m_name == "MSIntInput" || engineType->m_name == "MSIntOutput" )
      {
         return "int";
      }
      else if ( engineType->m_name == "MSBoolInput" || engineType->m_name == "MSBoolOutput" )
      {
         return "bool";
      }
      else
      {
         return "void";
      }
   }
   else
   {
      return "void";
   }

   return "<<unknown_type>>";
}

///////////////////////////////////////////////////////////////////////////////

uint MaterialShaderBuilder::getTypeSize( const ReflectionType* engineType )
{
   if ( engineType )
   {
      if ( engineType->m_name == "MSVec2Input" || engineType->m_name == "MSVec2Output" )
      {
         return 8;
      }
      else if ( engineType->m_name == "MSVec3Input" || engineType->m_name == "MSVec3Output" )
      {
         return 12;
      }
      else if ( engineType->m_name == "MSVec4Input" || engineType->m_name == "MSVec4Output" )
      {
         return 16;
      }
      else if ( engineType->m_name == "MSColorInput" || engineType->m_name == "MSColorOutput" )
      {
         return 16;
      }
      else if ( engineType->m_name == "MSMatrixInput" || engineType->m_name == "MSMatrixOutput" )
      {
         return 64;
      }
      else if ( engineType->m_name == "MSMatrixArrInput" || engineType->m_name == "MSMatrixArrOutput" )
      {
         return 64;
      }
      else if ( engineType->m_name == "MSFloatInput" || engineType->m_name == "MSFloatOutput" )
      {
         return 4;
      }
      else if ( engineType->m_name == "MSIntInput" || engineType->m_name == "MSIntOutput" )
      {
         return 4;
      }
      else if ( engineType->m_name == "MSBoolInput" || engineType->m_name == "MSBoolOutput" )
      {
         return 4;
      }
      else
      {
         return 0;
      }
   }
   else
   {
      return 0;
   }

   return 0;
}

///////////////////////////////////////////////////////////////////////////////
