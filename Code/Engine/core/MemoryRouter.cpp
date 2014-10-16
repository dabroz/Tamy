#include "core.h"
#include "core\MemoryRouter.h"
#include "core\CallstackTree.h"
#include "core\CallstackTracer.h"
#include "core\dostream.h"
#include <strstream>


///////////////////////////////////////////////////////////////////////////////

INIT_SINGLETON( MemoryRouter );

uint MemoryRouter::s_headerSize = sizeof( void* );

///////////////////////////////////////////////////////////////////////////////

MemoryRouter::MemoryRouter( const SingletonConstruct& )
{
#ifdef _TRACK_MEMORY_ALLOCATIONS
   m_tracer = new CallstackTracer();
   m_callstacksTree = new CallstackTree();
#endif
}

///////////////////////////////////////////////////////////////////////////////

MemoryRouter::~MemoryRouter()
{
#ifdef _TRACK_MEMORY_ALLOCATIONS

   // dump unallocated callstacks
   {
      MemoryRouter& memoryRouter = TSingleton< MemoryRouter >::getInstance();

      dostream dbgOutput;
      uint callstacksCount = memoryRouter.m_callstacksTree->getCallstacksCount();
      if ( callstacksCount > 0 )
      {
         dbgOutput << 
            "\n\n===================================================================\n" <<
            " MEMORY LEAKS REPORT - " << callstacksCount << " leaks encountered, " << memoryRouter.m_defaultAllocator.getMemoryUsed() << " bytes leaked \n" <<
            "===================================================================\n\n";

         ulong callstackTrace[128];
         for ( uint i = 0; i < callstacksCount; ++i )
         {
            uint callstackId = memoryRouter.m_callstacksTree->getCallstackId( i );
            uint callstackSize = memoryRouter.m_callstacksTree->getCallstack( callstackId, callstackTrace, 128 );
            memoryRouter.m_tracer->printCallstack( dbgOutput, callstackTrace, callstackSize );
         }

         dbgOutput << 
            "\n\n===================================================================\n" <<
            " MEMORY LEAKS REPORT END - " << callstacksCount << " leaks encountered\n" <<
            "===================================================================\n\n";
      }
      else
      {
         dbgOutput << 
            "\n\n===================================================================\n" <<
            " No memory leaks detected\n" <<
            "===================================================================\n\n";
      }

      dbgOutput.flush();
   }


   delete m_tracer;
   m_tracer = NULL;

   delete m_callstacksTree;
   m_callstacksTree = NULL;
#endif
}

///////////////////////////////////////////////////////////////////////////////

void* MemoryRouter::alloc( size_t size, AllocationMode allocMode, MemoryAllocator* allocator )
{
   size_t alignedSize = MemoryUtils::calcAlignedSize( size );
   void* pa = allocator->alloc( alignedSize + s_headerSize );

   if( !pa )
   {
      return NULL;
   }

#ifdef _TRACK_MEMORY_ALLOCATIONS
   {
      // memorize the callstack
      ulong callstack[64];
      uint callstackSize = m_tracer->getStackTrace( callstack, 64 );
      m_callstacksTree->insert( (uint)pa, callstack, callstackSize );
   }
#endif

   // first - insert the header
   *(int*)pa = (int)allocator;
   pa = (char*)pa + s_headerSize;

   // then align the address
   void* ptr = MemoryUtils::alignAddressAndStoreOriginal( pa );
         

   return ptr;
}

///////////////////////////////////////////////////////////////////////////////

void MemoryRouter::dealloc( void* ptr, AllocationMode allocMode )
{
   if ( ptr == NULL )
   {
      return;
   }
   void* postHeaderPtr = MemoryUtils::resolveAlignedAddress( ptr );

   // decode the address of the allocator
   void* origPtr = (char*)postHeaderPtr - s_headerSize;
   int allocAddrRaw = *(int*)origPtr;
   MemoryAllocator* allocator = (MemoryAllocator*)allocAddrRaw;

   if ( origPtr )
   {
#ifdef _TRACK_MEMORY_ALLOCATIONS
      // remove the callstack
      m_callstacksTree->remove( (uint)origPtr );
#endif

      allocator->dealloc( origPtr );
   }
}

///////////////////////////////////////////////////////////////////////////////
