/// @file   core-Renderer/ShaderConstantDesc.h
/// @brief  description of a shader constant
#pragma once

#include <string>
#include "core\MemoryRouter.h"
#include "core\IDString.h"


///////////////////////////////////////////////////////////////////////////////

class InStream;
class OutStream;

///////////////////////////////////////////////////////////////////////////////

/**
 * This structure describes a single shader constant.
 */
struct ShaderConstantDesc
{
   DECLARE_ALLOCATOR( ShaderConstantDesc, AM_DEFAULT );

   enum Type
   {
      CT_Bool,
      CT_Int,
      CT_Float,
      CT_String,
      CT_Matrix,
      CT_Vec4,
      CT_Texture,
      CT_DataBuffer
   };

   Type              m_type;
   std::string       m_name;

   IDString          m_id; // runtime

   /**
    * Default constructor.
    */
   ShaderConstantDesc() : m_type( CT_Float ) {}

   /**
    * Constructor.
    *
    * @param type
    * @param name
    */
   ShaderConstantDesc( Type type, const char* name ) : m_type( type ), m_name( name ), m_id( m_name ) {}

   /**
    * Copy constructor.
    *
    * @param rhs
    */
   ShaderConstantDesc( const ShaderConstantDesc& rhs ) : m_type( rhs.m_type ), m_name( rhs.m_name ), m_id( rhs.m_id ) {}

   // -------------------------------------------------------------------------
   // Operators
   // -------------------------------------------------------------------------
   bool operator==( const ShaderConstantDesc& rhs ) const;

   // -------------------------------------------------------------------------
   // Serialization support
   // -------------------------------------------------------------------------
   friend OutStream& operator<<( OutStream& stream, const ShaderConstantDesc& params );
   friend InStream& operator>>( InStream& stream, ShaderConstantDesc& params );
};

///////////////////////////////////////////////////////////////////////////////
