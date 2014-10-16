#include "core-Physics\PXMemoryAllocator.h"
#include "core\MemoryAllocator.h"


///////////////////////////////////////////////////////////////////////////////

PXMemoryAllocator::PXMemoryAllocator()
{
   m_allocator = new DefaultAllocator();
}

///////////////////////////////////////////////////////////////////////////////

PXMemoryAllocator::~PXMemoryAllocator()
{
   delete m_allocator;
   m_allocator = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void* PXMemoryAllocator::allocate( size_t size, const char* typeName, const char* filename, int line )
{
   MemoryRouter& memRouter = TSingleton< MemoryRouter >::getInstance();
   void* ptr = memRouter.alloc( size, AM_DEFAULT, m_allocator );
   return ptr;
}

///////////////////////////////////////////////////////////////////////////////

void PXMemoryAllocator::deallocate( void* ptr )
{
   MemoryRouter& memRouter = TSingleton< MemoryRouter >::getInstance();
   memRouter.dealloc( ptr, AM_DEFAULT );
}

///////////////////////////////////////////////////////////////////////////////
