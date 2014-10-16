/// @file   core\MemoryRouter.h
/// @brief  routes memory allocation/deallocation requests to a proper allocator
#ifndef _MEMORY_ROUTER_H
#define _MEMORY_ROUTER_H

#include "core\DefaultAllocator.h"
#include "core\EngineDefines.h"
#include "core\Singleton.h"


///////////////////////////////////////////////////////////////////////////////

enum AllocationMode
{
   AM_ALIGNED_16,
   AM_DEFAULT
};


///////////////////////////////////////////////////////////////////////////////

class CallstackTree;
class CallstackTracer;

///////////////////////////////////////////////////////////////////////////////

/**
 * Routes memory allocation/deallocation requests to a proper allocator.
 */
class MemoryRouter
{
   PRIORITY_SINGLETON( 0 );

public:
   DefaultAllocator              m_defaultAllocator;

private:
   static uint                   s_headerSize;

#ifdef _TRACK_MEMORY_ALLOCATIONS
   CallstackTracer*              m_tracer;
   CallstackTree*                m_callstacksTree;
#endif

public:
   /**
    * Singleton constructor.
    */
   MemoryRouter( const SingletonConstruct& );
   ~MemoryRouter();

   /**
    * Returns the size of allocated memory.
    */
   inline ulong getMemoryUsed() const { return m_defaultAllocator.getMemoryUsed(); }

   /**
    * Translates an address returned by a memory allocator to an object address
    * ( by taking into account the header that the MemoryRouter prepends to each allocation )
    *
    * @param allocatedAddr
    */
   static inline void* convertAllocatedToObjectAddress( void* allocatedAddr );

   /**
    * Calculates amount of memory required to allocate an object of the specified type.
    *
    * @param T
    */
   template< typename T >
   static uint sizeOf();

   // -------------------------------------------------------------------------
   // Default allocation
   // -------------------------------------------------------------------------
   /**
    * Destroys an object allocated by the allocator.
    *
    * @param T             object type
    * @param obj           object to release
    * @param allocMode     allocation mode used by the object
    */
   template< typename T >
   void destroy( T* obj, AllocationMode allocMode, MemoryAllocator* allocator );

   /**
    * Deallocates the memory from the specified address.
    *
    * @param ptr
    * @param allocMode     allocation mode used by the object
    */
   void dealloc( void* ptr, AllocationMode allocMode );

   /**
    * Allocates a chunk of memory of the specified size.
    *
    * @param size
    * @param allocMode     allocation mode used by the object
    */
   void* alloc( size_t size, AllocationMode allocMode, MemoryAllocator* allocator);
};

///////////////////////////////////////////////////////////////////////////////

#include "core\MemoryRouter.inl"

///////////////////////////////////////////////////////////////////////////////

template< typename T >
AllocationMode getAllocMode( const T& ) { return AM_DEFAULT; }

///////////////////////////////////////////////////////////////////////////////
/**
 * This macro defines a memory class the object should use when it's being dynamically allocated.
 */
#define DECLARE_ALLOCATOR( Type, AllocMode ) \
public: \
   static void* operator new( size_t size ) \
   { \
      MemoryRouter& router = TSingleton< MemoryRouter >::getInstance(); \
      void* ptr = router.alloc( size, AllocMode, &router.m_defaultAllocator ); \
      return ptr; \
   } \
   \
   static void operator delete( void* deletedPtr ) \
   { \
      TSingleton< MemoryRouter >::getInstance().dealloc( deletedPtr, AllocMode ); \
   } \
   static void* operator new[]( size_t size ) \
   { \
      MemoryRouter& router = TSingleton< MemoryRouter >::getInstance(); \
      void* ptr = router.alloc( size, AllocMode, &router.m_defaultAllocator ); \
      return ptr; \
   } \
   \
   static void operator delete[]( void* deletedPtr ) \
   { \
      TSingleton< MemoryRouter >::getInstance().dealloc( deletedPtr, AllocMode ); \
   } \
   \
   static void* operator new( size_t size, MemoryAllocator* allocator ) \
   { \
      void* ptr = TSingleton< MemoryRouter >::getInstance().alloc( size, AllocMode, allocator ); \
      return ptr; \
   } \
   static void* operator new[]( size_t size, MemoryAllocator* allocator ) \
   { \
      void* ptr = TSingleton< MemoryRouter >::getInstance().alloc( size, AllocMode, allocator ); \
      return ptr; \
   } \
   \
   static void operator delete( void* deletedPtr, MemoryAllocator* allocator ) \
   { \
      TSingleton< MemoryRouter >::getInstance().dealloc( deletedPtr, AllocMode ); \
   } \
   static void operator delete[]( void* deletedPtr, MemoryAllocator* allocator ) \
   { \
      TSingleton< MemoryRouter >::getInstance().dealloc( deletedPtr, AllocMode ); \
   } \
   \
   \
   static void* operator new( size_t size, void* placementPtr ) \
   { \
      return placementPtr; \
   } \
   static void* operator new[]( size_t size, void* placementPtr ) \
   { \
      return placementPtr; \
   } \
   static void operator delete( void* deletedPtr, void* placementPtr ) \
   { \
   } \
   static void operator delete[]( void* deletedPtr, void* placementPtr ) \
   { \
   } \
   \
   \
   friend AllocationMode getAllocMode( const Type& type ) { return AllocMode; }

///////////////////////////////////////////////////////////////////////////////

#define GET_ALLOC_MODE( Type ) getAllocMode( *( ( const Type* )16 ) )

///////////////////////////////////////////////////////////////////////////////

#define SIZE_OF( Type ) MemoryRouter::sizeOf< Type >()

///////////////////////////////////////////////////////////////////////////////

#endif // _MEMORY_ROUTER_H
