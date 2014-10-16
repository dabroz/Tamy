/// @file   core-Renderer\ShaderDataBuffer.h
/// @brief  a data buffer used to send large data blocks as shader constants
#ifndef _SHADER_DATA_BUFFER_H
#define _SHADER_DATA_BUFFER_H

#include "core\MemoryRouter.h"
#include "core-Renderer\RenderResource.h"
#include "core-Renderer\RenderCommand.h"
#include "core\Array.h"


///////////////////////////////////////////////////////////////////////////////

class ShaderDataBuffer : public ReflectionObject
{
   DECLARE_ALLOCATOR( ShaderDataBuffer, AM_DEFAULT );
   DECLARE_CLASS();

public:
   RenderResource*            m_renderResource;

private:
   Array<byte>                m_dataBuf;

public:
   /**
   * Constructor.
   *
   * @param dataSize
   */
   ShaderDataBuffer( const uint dataSize );
   ~ShaderDataBuffer();

   /**
    * Creates the buffer and allocates the data structure.
    */
   template< typename T >
   static ShaderDataBuffer* create();

   /**
    * Resizes the data buffer.
    */
   void resize( const uint dataSize );

   /**
    * Returns the represented data structure.
    *
    * @param offset     memory offset into the data buffer
    */
   template< typename T >
   const T& getData( uint offset = 0 ) const;

   /**
    * Gives access to the represented data structure, allowing to modify its contents.
    *
    * @param offset     memory offset into the data buffer
    */
   template< typename T >
   T& accessData( uint offset = 0 );

   /**
    * Returns a pointer to the data stored in the buffer.
    */
   inline void* getRawData() const {
      return( void* ) ( ( const byte* ) m_dataBuf );
   }

   /**
    * Returns the size of the data stored in the buffer.
    */
   inline uint getSize() const {
      return m_dataBuf.size();
   }

   /**
    * Copies the data from the buffer to another buffer.
    *
    * @param outData
    */
   void copy( byte* outData ) const;
};

///////////////////////////////////////////////////////////////////////////////

#include "core-Renderer\ShaderDataBuffer.inl"

///////////////////////////////////////////////////////////////////////////////

#endif // _SHADER_DATA_BUFFER_H
