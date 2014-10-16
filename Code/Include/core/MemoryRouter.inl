#ifndef _MEMORY_ROUTER_H
#error "This file can be included only in MemoryRouter.h"
#else

#include <memory>
#include "core\MemoryUtils.h"


///////////////////////////////////////////////////////////////////////////////

template< typename T >
void MemoryRouter::destroy( T* obj, AllocationMode allocMode, MemoryAllocator* allocator )
{
   if ( obj )
   {
      obj->~T();
      dealloc( obj, allocMode, allocator );
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
uint MemoryRouter::sizeOf()
{
   size_t origSize = sizeof( T );
   size_t alignedSize = MemoryUtils::calcAlignedSize( origSize );
   return alignedSize + s_headerSize;
}

///////////////////////////////////////////////////////////////////////////////

void* MemoryRouter::convertAllocatedToObjectAddress( void* allocatedAddr )
{
   const size_t headerSize = sizeof( void* );

   void* retAddress = ( char* ) allocatedAddr + s_headerSize;

   // then align the address
   retAddress = MemoryUtils::alignAddress( retAddress );
   return retAddress;
}

///////////////////////////////////////////////////////////////////////////////

#endif // _MEMORY_ROUTER_H
