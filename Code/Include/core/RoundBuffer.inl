#ifndef _ROUND_BUFFER_H
#error "This file can only be included from RoundBuffer.h"
#else

#include "core\Assert.h"


///////////////////////////////////////////////////////////////////////////////

uint RoundBuffer::getAllocationsCount() const 
{ 
   return m_allocationsCount;
}

///////////////////////////////////////////////////////////////////////////////

uint RoundBuffer::size() const
{
   return m_allocationsCount;
}

///////////////////////////////////////////////////////////////////////////////

bool RoundBuffer::empty() const
{
   return m_allocationsCount == 0;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
T* RoundBuffer::front()
{
   if ( getMemoryUsed() == 0 )
   {
      return NULL;
   }
   else
   {
      size_t tailOffset = 0;
      size_t tailSize = 0;
      getTail( tailOffset, tailSize ); 

      char* startAddr = m_memory + tailOffset + BUFFER_INFO_CHUNK_SIZE;

      startAddr = (char*)MemoryRouter::convertAllocatedToObjectAddress( startAddr );
      return reinterpret_cast< T* >( startAddr );
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
uint RoundBuffer::sizeOf()
{
   size_t allocSize = SIZE_OF( T ) + BUFFER_INFO_CHUNK_SIZE;
   return allocSize;
}

///////////////////////////////////////////////////////////////////////////////

#endif // _ROUND_BUFFER_H
