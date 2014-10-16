#include "core.h"
#include "core/EngineDefines.h"
#include "core/Log.h"
#include <memory>


#ifdef _TRACE_GLOBAL_ALLOCATIONS

class MemoryRouter
{
private:
   static MemoryRouter        s_theInstance;

public:

   /**
    * Returns the singleton instance of the memory router.
    */
   MemoryRouter& getInstance() const { return s_theInstance; }
};

///////////////////////////////////////////////////////////////////////////////

inline void* operator new( size_t size )
{
   ::OutputDebugStringA( "allocation\n" );
   return malloc( size );
}

///////////////////////////////////////////////////////////////////////////////

inline void operator delete( void* ptr )
{
   ::OutputDebugStringA( "deallocation\n" );
   free( ptr );
}

///////////////////////////////////////////////////////////////////////////////

inline void* operator new[]( size_t size )
{
   return malloc( size );
}

///////////////////////////////////////////////////////////////////////////////

inline void operator delete[]( void* ptr )
{
   free( ptr );
}

///////////////////////////////////////////////////////////////////////////////

#endif // _TRACE_GLOBAL_ALLOCATIONS

