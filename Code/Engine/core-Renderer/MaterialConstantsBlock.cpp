#include "core-Renderer\MaterialConstantsBlock.h"
#include "core-Renderer\MaterialShaderBuilder.h"
#include "core-Renderer\Shader.h"


///////////////////////////////////////////////////////////////////////////////

MaterialConstantsBlock::MaterialConstantsBlock( const char* name )
   : m_name( name )
   , m_nextOffset( 0 )
{
}

///////////////////////////////////////////////////////////////////////////////

void MaterialConstantsBlock::add( const ReflectionType* engineType, uint& outOffset, std::string& outName )
{
   char tmpName[128];
   sprintf_s( tmpName, "internalGlobal_%03d", m_entries.size() );

   m_entries.pushBack( Entry( m_nextOffset, engineType, tmpName, 1 ) );
   outOffset = m_nextOffset;
   outName = tmpName;

   m_nextOffset += MaterialShaderBuilder::getTypeSize( engineType );
}

///////////////////////////////////////////////////////////////////////////////

void MaterialConstantsBlock::add( const ReflectionType* engineType, const char* name, uint arrSize, uint& outOffset )
{
   m_entries.pushBack( Entry( m_nextOffset, engineType, name, arrSize ) );
   outOffset = m_nextOffset;

   m_nextOffset += MaterialShaderBuilder::getTypeSize( engineType ) * arrSize;
}

///////////////////////////////////////////////////////////////////////////////

void MaterialConstantsBlock::compile( std::string& outCode )
{
   if ( m_entries.empty() )
   {
      // nothing to generate
      return;
   }

   char tmpLine[128];

   // header
   sprintf_s( tmpLine, "uniform %s\n{\n", m_name.c_str() );
   outCode += tmpLine;

   // fields
   for ( List< Entry >::iterator it = m_entries.begin(); !it.isEnd(); ++it )
   {
      const Entry& entry = *it;

      const char* typeName = MaterialShaderBuilder::mapType( entry.m_engineType );
      const uint typeSize = MaterialShaderBuilder::getTypeSize( entry.m_engineType ) * entry.m_arrSize;

      if ( entry.m_arrSize <= 1 )
      {
         sprintf_s( tmpLine, "\t%s\t\t\t%s;\t\t//%d bytes\n", typeName, entry.m_name.c_str(), typeSize );
      }
      else
      {
         sprintf_s( tmpLine, "\t%s\t\t\t%s[%d];\t\t//%d bytes\n", typeName, entry.m_name.c_str(), entry.m_arrSize, typeSize );
      }
      outCode += tmpLine;
   }

   // padding
   uint padding = calulatePadding();
   if ( padding > 0 )
   {
      sprintf_s( tmpLine, "\tint\t\t\tpadding[%d];\n", padding / 4 );
      outCode += tmpLine;
   }

   // footer
   sprintf_s( tmpLine, "};\t\t//Total: %d bytes\n", m_nextOffset + padding );
   outCode += tmpLine;
}

///////////////////////////////////////////////////////////////////////////////

uint MaterialConstantsBlock::calculateBufSize() const
{
   const uint padding = calulatePadding();
   const uint totalSize = m_nextOffset + padding;
   return totalSize;
}

///////////////////////////////////////////////////////////////////////////////

uint MaterialConstantsBlock::calulatePadding() const
{
   const uint padding = ( uint ) ceil( ( float )m_nextOffset / 16.0f ) * 16 - m_nextOffset;
   return padding;
}

///////////////////////////////////////////////////////////////////////////////
