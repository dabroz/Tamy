#ifndef _SHADER_DATA_BUFFER_H
#error "This file can only be included in ShaderDataBuffer.h"
#else

///////////////////////////////////////////////////////////////////////////////

template< typename T >
ShaderDataBuffer* ShaderDataBuffer::create()
{
   const uint dataSize = sizeof( T );
   ShaderDataBuffer* newBuf = new ShaderDataBuffer( dataSize );
   
   // fill the memory - this will work for all PODs, which is what  the data buffer will be operating on exclusively
   new ( ( void* ) ( ( byte* ) newBuf->m_dataBuf ) ) T();

   return newBuf;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
const T& ShaderDataBuffer::getData( uint offset ) const
{
   byte* rawData = ( ( byte* ) m_dataBuf + offset );
   const T* structPtr = ( const T* ) rawData;
   return *structPtr;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
T& ShaderDataBuffer::accessData( uint offset )
{
   byte* rawData = ( ( byte* ) m_dataBuf + offset );
   T* structPtr = ( T* ) rawData;
   return *structPtr;
}

///////////////////////////////////////////////////////////////////////////////

#endif // _SHADER_DATA_BUFFER_H.inl
