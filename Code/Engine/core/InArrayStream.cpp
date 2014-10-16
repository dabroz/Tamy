#include "core.h"
#include "core/InArrayStream.h"
#include <memory>


///////////////////////////////////////////////////////////////////////////////

InArrayStream::InArrayStream( const Array< byte >& buffer ) 
   : m_buffer( buffer )
   , m_offset( 0 )
{}

///////////////////////////////////////////////////////////////////////////////

void InArrayStream::load( void* val, unsigned int dataSize )
{
   if ( m_offset + dataSize > m_buffer.size() )
   {
      // end of buffer
      return;
   }

   memcpy( val, (const void*)( (const byte*)m_buffer + m_offset ), dataSize );
   m_offset += dataSize;
}

///////////////////////////////////////////////////////////////////////////////
