#include "core\InRawArrayStream.h"
#include <memory>


///////////////////////////////////////////////////////////////////////////////

InRawArrayStream::InRawArrayStream( const byte* buffer, uint bufferSize ) 
   : m_buffer( buffer )
   , m_bufferSize( bufferSize )
   , m_offset( 0 )
{}

///////////////////////////////////////////////////////////////////////////////

void InRawArrayStream::load( void* val, unsigned int dataSize )
{
   if ( m_offset + dataSize > m_bufferSize )
   {
      // end of buffer
      return;
   }

   memcpy( val, (const void*)( (const byte*)m_buffer + m_offset ), dataSize );
   m_offset += dataSize;
}

///////////////////////////////////////////////////////////////////////////////
