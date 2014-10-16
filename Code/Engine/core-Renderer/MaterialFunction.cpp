#include "core-Renderer\MaterialFunction.h"
#include "core-Renderer\MaterialShaderBuilder.h"
#include "core-Renderer\MaterialDataStruct.h"
#include "core-Renderer\MaterialCodeNode.h"
#include "core-Renderer\Shader.h"
#include "core\ReflectionTypesRegistry.h"
#include "core\StringUtils.h"
#include "core\Color.h"
#include "core\Vector.h"
#include <stdarg.h>

// logging 
#include "core\Log.h"

// string parser
#include "core\StringParser.h"


///////////////////////////////////////////////////////////////////////////////

MaterialFunction::MaterialFunction( MaterialShaderBuilder& shaderBuilder )
   : m_shaderBuilder( shaderBuilder )
   , m_inputStruct( NULL )
{
}

///////////////////////////////////////////////////////////////////////////////

MaterialFunction::~MaterialFunction()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

MatFuncPrecompiled::MatFuncPrecompiled( MaterialShaderBuilder& shaderBuilder, const std::string& descFileContents )
   : MaterialFunction( shaderBuilder )
{
   StringParser parser( &descFileContents );

   parser.findNext( "<FUNCTION>" );
   parser.getNextWord( m_outputType );
   parser.getNextWord( m_functionName );
   
   parser.restart();
   parser.getStringBetween( "<CODE>", "</CODE>", m_code );
}

///////////////////////////////////////////////////////////////////////////////

void MatFuncPrecompiled::compile( std::string& outCode )
{
   outCode += m_code;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

MatFuncUserDefined::MatFuncUserDefined( MaterialShaderBuilder& shaderBuilder )
   : MaterialFunction( shaderBuilder )
{
}

///////////////////////////////////////////////////////////////////////////////

MatFuncUserDefined::~MatFuncUserDefined()
{
   for ( List< MaterialCodeNode* >::iterator it = m_code.begin(); !it.isEnd(); ++it )
   {
      MaterialCodeNode* codeNode = *it;
      delete codeNode;
   }
   m_code.clear();
}

///////////////////////////////////////////////////////////////////////////////

void MatFuncUserDefined::addInputParam( MNInput* input, std::string& outVarName )
{
   const char* fieldName = m_inputStruct->addField( input );

   char tmpStr[128];
   sprintf_s( tmpStr, "input.%s", fieldName );
   outVarName = tmpStr;
}

///////////////////////////////////////////////////////////////////////////////

void MatFuncUserDefined::setReturnType( MNOutput* output )
{
   m_outputType = m_shaderBuilder.mapType( output );
}

///////////////////////////////////////////////////////////////////////////////

void MatFuncUserDefined::addCodeLine( const char* code, ... )
{
   char formattedStr[1024];

   va_list argptr;
   va_start(argptr, code);
   vsprintf_s( formattedStr, code, argptr );
   va_end(argptr);

   m_code.pushBack( new MaterialCodeLine( formattedStr ) );
}

///////////////////////////////////////////////////////////////////////////////

MaterialCodeLine* MatFuncUserDefined::addCodeLine()
{
   MaterialCodeLine* codeLine = new MaterialCodeLine();
   m_code.pushBack( codeLine );

   return codeLine;
}

///////////////////////////////////////////////////////////////////////////////

void MatFuncUserDefined::compile( std::string& outCode )
{
   std::string tmpFinalCode = m_outputType + " " + m_functionName + "( " + m_inputStruct->m_name + " input )\n";
   tmpFinalCode += "{\n";

   for ( List< MaterialCodeNode* >::iterator it = m_code.begin(); !it.isEnd(); ++it )
   {
      MaterialCodeNode* codeNode = *it;
      codeNode->compile( tmpFinalCode );
   }

   tmpFinalCode += "}\n\n";

   outCode += tmpFinalCode;
}

///////////////////////////////////////////////////////////////////////////////
