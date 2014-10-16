#include "core\DefaultAllocator.h"
#include "core\MemoryUtils.h"
#include "core\CriticalSection.h"
#include <stdio.h>
#include <stdlib.h>


///////////////////////////////////////////////////////////////////////////////

DefaultAllocator::DefaultAllocator()
   : m_allocationLock( new CriticalSection() )
   , m_allocatedMemorySize( 0 )
{
}

///////////////////////////////////////////////////////////////////////////////

DefaultAllocator::~DefaultAllocator()
{
   delete m_allocationLock;
   m_allocationLock = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void* DefaultAllocator::alloc( size_t size )
{
   m_allocationLock->enter();
   
   // allocate a bit more to store allocation size
   void* origPtr = ::malloc( size + sizeof(size_t) );

   // store the size
   *(size_t*)origPtr = size;

   // get the data pointer
   void* ptr = (size_t*)origPtr + 1;

   m_allocatedMemorySize += size;

   m_allocationLock->leave();

   return ptr;
}

///////////////////////////////////////////////////////////////////////////////

void DefaultAllocator::dealloc( void* ptr )
{
   m_allocationLock->enter();

   // get the actual pointer to the allocated memory chunk
   size_t* origPtr = (size_t*)ptr - 1;

   // get the size of the allocated chunk
   size_t allocatedSize = *origPtr;
   m_allocatedMemorySize -= allocatedSize;

#ifdef _DEBUG
   // fill memory with recognizable value to eradicate its previous contents
   // and facilitate corrupt memory access calls
   memset( origPtr, 0xff, sizeof( char ) * allocatedSize );
#endif

   // deallocate it
   ::free( origPtr );

   m_allocationLock->leave();
}

///////////////////////////////////////////////////////////////////////////////

ulong DefaultAllocator::getMemoryUsed() const 
{ 
   m_allocationLock->enter();
   ulong val = m_allocatedMemorySize;
   m_allocationLock->leave();

   return val; 
}

///////////////////////////////////////////////////////////////////////////////
