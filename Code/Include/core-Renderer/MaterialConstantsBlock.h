/// @file   core-Renderer\MaterialConstantsBlock.h
/// @brief  shader constants block builder
#pragma once

#include "core\MemoryRouter.h"
#include "core\types.h"
#include "core\List.h"
#include <string>


///////////////////////////////////////////////////////////////////////////////

class ReflectionType;

///////////////////////////////////////////////////////////////////////////////

class MaterialConstantsBlock
{
   DECLARE_ALLOCATOR( MaterialConstantsBlock, AM_DEFAULT );

private:
   struct Entry
   {
      DECLARE_ALLOCATOR( Entry, AM_DEFAULT );

      uint                    m_offset;
      const ReflectionType*   m_engineType;
      std::string             m_name;
      uint                    m_arrSize;

      Entry( uint offset, const ReflectionType* engineType, const char* name, uint arrSize )
         : m_offset( offset )
         , m_engineType( engineType )
         , m_name( name )
         , m_arrSize( arrSize )
      {}

      Entry( const Entry& rhs )
         : m_offset( rhs.m_offset )
         , m_engineType( rhs.m_engineType )
         , m_name( rhs.m_name )
         , m_arrSize( rhs.m_arrSize )
      {}
   };

private:
   std::string          m_name;
   List< Entry >        m_entries;
   uint                 m_nextOffset;

public:
   /**
    * Constructor.
    *
    * @param name
    */
   MaterialConstantsBlock( const char* name );

   /**
    * Adds a new global.
    *
    * @param engineType
    * @param outOffset
    * @param outName
    */
   void add( const ReflectionType* engineType, uint& outOffset, std::string& outName );

   /**
   * Adds a new global.
   *
   * @param engineType
   * @param name
   * @param arrSize
   * @param outOffset
   */
   void add( const ReflectionType* engineType, const char* name, uint arrSize, uint& outOffset );

   /**
    * Compiles the structure.
    *
    * @param outCode
    */
   void compile( std::string& outCode );

   /**
    * Returns the size of the buffer.
    */
   uint calculateBufSize() const;

   /**
    * Returns the name of the constants block.
    */
   inline const std::string& getName() const {
      return m_name;
   }

private:
   uint calulatePadding() const;
};

///////////////////////////////////////////////////////////////////////////////
