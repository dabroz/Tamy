#include "core-Renderer\MaterialDataStruct.h"
#include "core-Renderer\MaterialShaderBuilder.h"
#include "core\StringUtils.h"
#include "core\StringParser.h"


///////////////////////////////////////////////////////////////////////////////

MaterialDataStruct::MaterialDataStruct( MaterialShaderBuilder& builder, const std::string& descFileContents )
   : m_builder( builder )
   , m_occupiedFieldsCount( 0 )
{
   StringParser parser( &descFileContents );
   parser.findNext( "<INPUT_STRUCT>" );
   parser.getNextWord( m_name );

   std::string type, fieldName;
   while ( true )
   {
      parser.getNextWord( type );
      if ( type == "</INPUT_STRUCT>" )
      {
         break;
      }
      parser.getNextWord( fieldName );

      m_fieldTypes.push_back( type );
      m_fieldNames.push_back( fieldName );
      ++m_occupiedFieldsCount;
   }
}

///////////////////////////////////////////////////////////////////////////////

MaterialDataStruct::MaterialDataStruct( MaterialShaderBuilder& builder, const char* name, uint fieldsCount )
   : m_builder( builder )
   , m_fieldTypes( fieldsCount )
   , m_fieldNames( fieldsCount )
   , m_occupiedFieldsCount( 0 )
{
   m_name = StringUtils::replace( name, " ", "_" );

   m_fieldTypes.resize( fieldsCount, std::string() );
   m_fieldNames.resize( fieldsCount, std::string() );
}

///////////////////////////////////////////////////////////////////////////////

const char* MaterialDataStruct::addField( GBNodeSocket* socket )
{ 
   ASSERT( m_occupiedFieldsCount < m_fieldTypes.size() );

   const char* typeName = m_builder.mapType( socket );
   m_fieldTypes[m_occupiedFieldsCount] = typeName;
   m_fieldNames[m_occupiedFieldsCount] = StringUtils::replace( socket->getName(), " ", "_" );
   
   const char* fieldName = m_fieldNames[m_occupiedFieldsCount].c_str();
   ++m_occupiedFieldsCount;

   return fieldName;
}

///////////////////////////////////////////////////////////////////////////////

void MaterialDataStruct::compile( std::string& outCode ) const
{
   outCode += "struct ";
   outCode += m_name + "\n{\n";

   for ( uint i = 0; i < m_occupiedFieldsCount; ++i )
   {
      outCode += std::string( "\t" ) + m_fieldTypes[i] + "\t\t\t" + m_fieldNames[i] + ";\n";
   }

   outCode += "};\n\n";
}

///////////////////////////////////////////////////////////////////////////////
