#include "core-Renderer\MaterialEntryStruct.h"
#include "core-Renderer\Shader.h"
#include "core-Renderer\MaterialShaderBuilder.h"
#include "core\GraphBuilderSockets.h"
#include "core\StringUtils.h"


///////////////////////////////////////////////////////////////////////////////

MaterialEntryStruct::MaterialEntryStruct( bool isInput, ShaderRoutine routine )
   : m_isInput( isInput )
   , m_routine( routine )
{
   m_inOutSignature = m_isInput ? "In" : "Out";
}

///////////////////////////////////////////////////////////////////////////////

void MaterialEntryStruct::setName( const std::string& name )
{
   m_name = StringUtils::replace( name, " ", "_" );
}

///////////////////////////////////////////////////////////////////////////////

int MaterialEntryStruct::addField( const GBNodeSocket* socket )
{
   const char* typeTag = MaterialShaderBuilder::mapType( socket );

   // get the index of the next available field
   int idx = m_fieldTypes.size();
   m_fieldTypes.push_back( typeTag );
   m_fieldNames.push_back( socket->getName() );

   // no more fields left
   return idx;
}

///////////////////////////////////////////////////////////////////////////////

void MaterialEntryStruct::getFieldTag( uint idx, std::string& outTag ) const
{
   char tmpStr[64];
   sprintf_s( tmpStr, "<%s_%s_vParam%d>", m_name.c_str(), m_inOutSignature, idx );

   outTag = tmpStr;
}

///////////////////////////////////////////////////////////////////////////////

void MaterialEntryStruct::compile( std::string& outCode ) const
{
   // header
   outCode += "// struct ";
   outCode += m_name;
   outCode += " start\n\n";

   // fields
   if ( m_isInput )
   {
      char tmpStr[256];
      const uint count = m_fieldTypes.size();
      for ( uint i = 0; i < count; ++i )
      {
         if ( m_routine == SHADER_VERTEX )
         {
            // vertex shaders require an explicit input parameters layout specification
            sprintf_s( tmpStr, "layout(location = %d) in %s %s;\n", i, m_fieldTypes[i].c_str(), m_fieldNames[i].c_str() );
         }
         else
         {
            // other shader routines - not so much
            sprintf_s( tmpStr, "smooth in %s %s;\n", m_fieldTypes[i].c_str(), m_fieldNames[i].c_str() );
         }
         outCode += tmpStr;
      }
   }
   else
   {
      char tmpStr[256];
      const uint count = m_fieldTypes.size();
      for ( uint i = 0; i < count; ++i )
      {
         if ( m_routine == SHADER_VERTEX && i == 0 )
         {
            // the first element in the output layout of a vertex shader always maps to "gl_Position",
            // and therefore there's no need to describe it explicitly
            tmpStr[0] = 0;
         }
         else if ( m_routine == SHADER_FRAGMENT )
         {
            sprintf_s( tmpStr, "layout(location = %d) out %s %s;\n", i, m_fieldTypes[i].c_str(), m_fieldNames[i].c_str() );
         }
         else
         {
            sprintf_s( tmpStr, "smooth out %s %s;\n", m_fieldTypes[i].c_str(), m_fieldNames[i].c_str() );
         }

         outCode += tmpStr;
      }

   }

   // footer
   outCode += "// struct ";
   outCode += m_name;
   outCode += " end\n\n";
}

///////////////////////////////////////////////////////////////////////////////

void MaterialEntryStruct::replaceTokens( std::string& outCode ) const
{
   // In GLSL we need to match the names of the vertex shader's input stage exactly to 
   // a set of specific names the code will try to bind the data streams to.
   // The remaining stages can be named in an arbitrary way
   char tmpInName[64];
   const uint count = m_fieldNames.size();
   for ( uint i = 0; i < count; ++i )
   {
      sprintf_s( tmpInName, "<%s_%s_vParam%d>", m_name.c_str(), m_inOutSignature, i );

      if ( m_routine == SHADER_VERTEX && m_isInput )
      {
         outCode = StringUtils::replace( outCode, tmpInName, m_fieldNames[i].c_str() );
      }
      else if ( m_routine == SHADER_VERTEX && !m_isInput && i == 0 )
      {
         // the first element in the output layout of a vertex shader always maps to "gl_Position"

         outCode = StringUtils::replace( outCode, tmpInName, "gl_Position" );
      }
      else
      {
         outCode = StringUtils::replace( outCode, tmpInName, m_fieldNames[i].c_str() );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////
