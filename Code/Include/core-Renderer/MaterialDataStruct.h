/// @file   core-Renderer\MaterialDataStruct.h
/// @brief  a custom data structure defined in the material shader
#pragma once

#include "core\MemoryRouter.h"
#include <string>
#include "core\Array.h"


///////////////////////////////////////////////////////////////////////////////

class GBNodeSocket;
class MaterialShaderBuilder;

///////////////////////////////////////////////////////////////////////////////

class MaterialDataStruct
{
   DECLARE_ALLOCATOR( MaterialDataStruct, AM_DEFAULT );

public:
   std::string             m_name;

private:
   MaterialShaderBuilder&  m_builder;
   Array< std::string >    m_fieldTypes;
   Array< std::string >    m_fieldNames;
   uint                    m_occupiedFieldsCount;

public:
   /**
    * Constructor.
    *
    * @param builder
    * @param name
    * @param fieldsCount
    */
   MaterialDataStruct( MaterialShaderBuilder& builder, const char* name, uint fieldsCount );

   /**
   * Constructor.
   *
   * @param builder
   * @param descFileContents
   */
   MaterialDataStruct( MaterialShaderBuilder& builder, const std::string& descFileContents );

   /**
    * Adds a field that's tide to the data flowing from the specified socket.
    *
    * @param socket
    * @return field name
    */
   const char* addField( GBNodeSocket* socket );

   /**
    * Returns the name of the specified field.
    */
   inline const char* getFieldName( uint idx ) const {
      return m_fieldNames[idx].c_str();
   }

   /**
   * Returns the type of the specified field.
   */
   inline const char* getFieldType( uint idx ) const {
      return m_fieldTypes[idx].c_str();
   }

   /**
    * Compiles the structure.
    *
    * @param outCode
    */
   void compile( std::string& outCode ) const;
};

///////////////////////////////////////////////////////////////////////////////

