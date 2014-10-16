#ifndef _MATERIAL_CODE_NODE_H
#error "This file can only be included by MaterialCodeNode.h"
#else

#include "core-Renderer\MaterialShaderBuilder.h"


///////////////////////////////////////////////////////////////////////////////

template< typename T >
MaterialConstant< T >::MaterialConstant( const MNOutput* output, const char* varName, const T& value )
   : m_varName( varName )
   , m_value( value )
{
   m_typeId = MaterialShaderBuilder::mapType( output );
}

///////////////////////////////////////////////////////////////////////////////


#endif // _MATERIAL_CODE_NODE_H
