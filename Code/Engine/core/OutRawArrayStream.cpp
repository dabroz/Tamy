#include "core/OutRawArrayStream.h"
#include <memory>


///////////////////////////////////////////////////////////////////////////////

OutRawArrayStream::OutRawArrayStream( byte* buffer, uint bufferSize ) 
   : m_buffer( buffer )
   , m_bufferSize( bufferSize )
   , m_offset( 0 )
{}

///////////////////////////////////////////////////////////////////////////////

void OutRawArrayStream::save( const void* val, unsigned int dataSize )
{
   if ( m_offset + dataSize >= m_bufferSize )
   {
      // buffer overflow
      return;
   }

   void* destPtr = m_buffer + m_offset;

   memcpy( destPtr, val, dataSize );
   m_offset += dataSize;
}

///////////////////////////////////////////////////////////////////////////////
