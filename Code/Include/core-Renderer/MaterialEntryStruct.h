/// @file   core-Renderer\MaterialEntryStruct.h
/// @brief  a description of a structure used by a material shader's subroutine
#pragma once

#include "core\MemoryRouter.h"
#include "core\Array.h"
#include <string>

///////////////////////////////////////////////////////////////////////////////

enum ShaderRoutine;
class GBNodeSocket;
class MaterialShaderBuilder;

///////////////////////////////////////////////////////////////////////////////

/**
 * A description of a structure used by a material shader's subroutine.
 */
class MaterialEntryStruct
{
   DECLARE_ALLOCATOR( MaterialEntryStruct, AM_DEFAULT );

public:
   bool                                      m_isInput;
   const char*                               m_inOutSignature;

   ShaderRoutine                             m_routine;

   std::string                               m_name;
   Array< std::string >                      m_fieldTypes;
   Array< std::string >                      m_fieldNames;

public:
   /**
    * Constructor.
    *
    * @param isInput    is this the description of the input or the output structure.
    * @param routine
    */
   MaterialEntryStruct( bool isInput, ShaderRoutine routine );

   /**
    * Assigns the structure a new name.
    *
    * @param name
    */
   void setName( const std::string& name );

   /**
    * Adds a new field - selected the next free position in the layout.
    *
    * @param socket
    * @return  >=0 if the type was successfully added, -1 if there were no more fields left to add it
    */
   int addField( const GBNodeSocket* socket );

   /**
    * Returns the tag assigned to the field.
    *
    * @param idx 
    * @param outTag
    */
   void getFieldTag( uint idx, std::string& outTag ) const;

   /**
    * Compiles the structure code.
    * 
    * @param outCode       code will be appended to this string
    */
   void compile( std::string& outCode ) const;

   /**
    * Replaces custom tokens with the actual shader code.
    *
    * @param outCode       code will be appended to this string
    */
   void replaceTokens( std::string& outCode ) const;

};

///////////////////////////////////////////////////////////////////////////////
