#include "core.h"
#include "core/OutArrayStream.h"
#include <memory>


///////////////////////////////////////////////////////////////////////////////

OutArrayStream::OutArrayStream( Array< byte >& buffer ) 
   : m_buffer( buffer ) 
{}

///////////////////////////////////////////////////////////////////////////////

void OutArrayStream::save( const void* val, unsigned int dataSize )
{
   unsigned int prevBufSize = m_buffer.size();

   m_buffer.resize( m_buffer.size() + dataSize );

   memcpy( (void*)( (byte*)m_buffer + prevBufSize ), val, dataSize );
}

///////////////////////////////////////////////////////////////////////////////
