#ifndef _MATERIAL_SHADER_BUILDER_H
#error "This file can only be included in MaterialShaderBuilder.h"
#else

#include "core\ReflectionTypesRegistry.h"
#include "core-Renderer\MaterialCodeNode.h"
#include "core-Renderer\MaterialConstantsBlock.h"


///////////////////////////////////////////////////////////////////////////////

template< typename T >
void MaterialShaderBuilder::addGlobal( uint& outOffset, std::string& outName )
{
   ReflectionTypesRegistry& typesReg = TSingleton< ReflectionTypesRegistry >::getInstance();
   const ReflectionType* engineType = typesReg.find< T >();

   m_constantsBlock->add( engineType, outOffset, outName );
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
const char* MaterialShaderBuilder::addConstant( const MNOutput* output, const T& value )
{
   const char* varName = addVariable( output );
   m_constants.pushBack( new MaterialConstant< T >( output, varName, value ) );

   return varName;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
const char* MaterialShaderBuilder::addLocalVariable( const char* name )
{
   std::string nameStr = name;
   std::set< std::string >::iterator it = m_localVariables.find( nameStr );

   if ( it == m_localVariables.end() )
   {
      it = m_localVariables.insert( nameStr ).first;

      const char* hlslType = mapType( &T::getStaticRTTI() );
      addCodeLine( "%s %s;", hlslType, name );
   }

   return it->c_str();
}

///////////////////////////////////////////////////////////////////////////////

#endif // _MATERIAL_SHADER_BUILDER_H
