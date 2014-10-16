/// @file   core-Physics\PXMemoryAllocator.h
/// @brief  a memory allocator required by PhysX
#pragma once

#include "core\MemoryRouter.h"
#include "foundation\PxAllocatorCallback.h"


///////////////////////////////////////////////////////////////////////////////

class DefaultAllocator;

///////////////////////////////////////////////////////////////////////////////

class PXMemoryAllocator : public physx::PxAllocatorCallback
{
   DECLARE_ALLOCATOR( PXMemoryAllocator, AM_DEFAULT );

private:
   DefaultAllocator*             m_allocator;

public:
   /**
    * Constructor.
    */
   PXMemoryAllocator();
   ~PXMemoryAllocator();

   // -------------------------------------------------------------------------
   // PxAllocatorCallback implementation
   // -------------------------------------------------------------------------
   void* allocate( size_t size, const char* typeName, const char* filename, int line );
   void deallocate( void* ptr );
};

///////////////////////////////////////////////////////////////////////////////
