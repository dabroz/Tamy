#include "core-Renderer\MaterialCodeNode.h"
#include "core-Renderer\MaterialFunction.h"
#include "core-Renderer\MaterialDataStruct.h"
#include "core-Renderer\MaterialShaderBuilder.h"
#include "core-Renderer\Shader.h"
#include "core\Vector.h"
#include "core\Color.h"
#include <stdarg.h>


///////////////////////////////////////////////////////////////////////////////

MaterialCodeLine::MaterialCodeLine( const char* codeLine )
   : m_codeLine( codeLine )
{
}

///////////////////////////////////////////////////////////////////////////////

void MaterialCodeLine::set( const char* code, ... )
{
   char formattedStr[1024];

   va_list argptr;
   va_start( argptr, code );
   vsprintf_s( formattedStr, code, argptr );
   va_end( argptr );

   m_codeLine = formattedStr;
}

///////////////////////////////////////////////////////////////////////////////

void MaterialCodeLine::compile( std::string& outCode ) const
{
   outCode += std::string( "\t" ) + m_codeLine + "\n"; return;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

MaterialFunctionCall::MaterialFunctionCall( MaterialFunction* function, MaterialShaderBuilder& builder )
   : m_function( function )
   , m_builder( builder )
   , m_output( NULL )
{
}

///////////////////////////////////////////////////////////////////////////////

void MaterialFunctionCall::bindInput( GBNodeInput< MaterialNode >* input )
{
   m_params.pushBack( input );
}

///////////////////////////////////////////////////////////////////////////////

void MaterialFunctionCall::bindOutput( GBNodeOutput< MaterialNode >* output )
{
   m_output = output;
   m_builder.addVariable( output );
}

///////////////////////////////////////////////////////////////////////////////

void MaterialFunctionCall::compile( std::string& outCode ) const
{
   const MaterialDataStruct& inputStruct = m_function->getInputStruct();

   const char* outputType = m_builder.mapType( m_output );
   const char* outputVarName = m_builder.addVariable( m_output );
   const char* funcName = m_function->getName();
   const char* inputStructTypeName = inputStruct.m_name.c_str();
   char inputStructVarName[64];
   sprintf_s( inputStructVarName, "input%s", funcName );

   // the beginning
   addLine( outCode, "%s %s = %s(0);", outputType, outputVarName, outputType );
   addLine( outCode, "{" );
   addLine( outCode, "\t%s %s;", inputStructTypeName, inputStructVarName, inputStructTypeName );

   // passing params
   const int count = ( int ) m_params.size();
   int fieldIdx = 0;
   for ( List< GBNodeInput< MaterialNode >* >::const_iterator it = m_params.begin(); !it.isEnd(); ++it, ++fieldIdx )
   {
      GBNodeInput< MaterialNode >* input = *it;
      const char* fieldName = inputStruct.getFieldName( fieldIdx );

      if ( input->isConnected() )
      {
         // the input is connected to an output, so look up what variable the output writes its value to
         const char* inputVarName = m_builder.getVariable( input );
         addLine( outCode, "\t%s.%s = %s;", inputStructVarName, fieldName, inputVarName );
      }
      else
      {
         const char* fieldType = inputStruct.getFieldType( fieldIdx );
         addLine( outCode, "\t%s.%s = %s(0);", inputStructVarName, fieldName, fieldType );
      }

   }

   // function call
   addLine( outCode, "\t%s = %s( %s );", outputVarName, funcName, inputStructVarName );
   addLine( outCode, "}" );

}

///////////////////////////////////////////////////////////////////////////////

void MaterialFunctionCall::addLine( std::string& outCode, const char* formatStr, ... )
{
   char formattedStr[1024];

   va_list argptr;
   va_start( argptr, formatStr );
   vsprintf_s( formattedStr, formatStr, argptr );
   va_end( argptr );

   outCode += std::string( "\t" ) + formattedStr + "\n";
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

template<>
void MaterialConstant< Vector >::compile( std::string& outCode ) const
{
   char tmpStr[128];
   sprintf_s( tmpStr, "const %s\t\t%s = %s( %f, %f, %f, %f );\n", m_typeId.c_str(), m_varName.c_str(), m_typeId.c_str(), m_value[0], m_value[1], m_value[2], m_value[3] );
   outCode += tmpStr;
}

///////////////////////////////////////////////////////////////////////////////

template<>
void MaterialConstant< Color >::compile( std::string& outCode ) const
{
   char tmpStr[128];
   sprintf_s( tmpStr, "const %s\t\t%s = %s( %f, %f, %f, %f );\n", m_typeId.c_str(), m_varName.c_str(), m_typeId.c_str(), m_value.r, m_value.g, m_value.b, m_value.a );
   outCode += tmpStr;
}

///////////////////////////////////////////////////////////////////////////////

template<>
void MaterialConstant< bool >::compile( std::string& outCode ) const
{
   char tmpStr[80];
   sprintf_s( tmpStr, "const %s\t\t%s = %s;\n", m_typeId.c_str(), m_varName.c_str(), m_value ? "true" : "false" );
   outCode += tmpStr;
}

///////////////////////////////////////////////////////////////////////////////

template<>
void MaterialConstant< int >::compile( std::string& outCode ) const
{
   char tmpStr[80];
   sprintf_s( tmpStr, "const %s\t\t%s = %d;\n", m_typeId.c_str(), m_varName.c_str(), m_value );
   outCode += tmpStr;
}

///////////////////////////////////////////////////////////////////////////////

template<>
void MaterialConstant< float >::compile( std::string& outCode ) const
{
   char tmpStr[80];
   sprintf_s( tmpStr, "const %s\t\t%s = %f;\n", m_typeId.c_str(), m_varName.c_str(), m_value );
   outCode += tmpStr;
}

///////////////////////////////////////////////////////////////////////////////
