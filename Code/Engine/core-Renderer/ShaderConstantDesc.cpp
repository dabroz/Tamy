#include "core-Renderer/ShaderConstantDesc.h"
#include "core/OutStream.h"
#include "core/InStream.h"


///////////////////////////////////////////////////////////////////////////////

bool ShaderConstantDesc::operator==( const ShaderConstantDesc& rhs ) const
{
   return m_type == rhs.m_type && m_id == rhs.m_id;
}

///////////////////////////////////////////////////////////////////////////////

OutStream& operator<<( OutStream& stream, const ShaderConstantDesc& params )
{
   stream << params.m_type;
   stream << params.m_name;

   return stream;
}

///////////////////////////////////////////////////////////////////////////////

InStream& operator>>( InStream& stream, ShaderConstantDesc& params )
{
   stream >> params.m_type;
   stream >> params.m_name;

   params.m_id.set( params.m_name );

   return stream;
}

///////////////////////////////////////////////////////////////////////////////
