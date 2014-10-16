#ifndef _ARRAY_H
#error "This file can only be included from Array.h"
#else

#include "core\EngineDefines.h"
#include "core\MemoryRouter.h"
#include "core\Algorithms.h"


///////////////////////////////////////////////////////////////////////////////

template< typename T >
Array< T >::Array( unsigned int size, MemoryAllocator* allocator )
   : m_size(1)
   , m_elementsCount(0)
   , m_allocator( allocator )
{
   if ( !m_allocator )
   {
      // no allocator was specified, so we need to create our own
      m_allocator = &TSingleton< MemoryRouter >::getInstance().m_defaultAllocator;
   }

   m_arr = (T*)TSingleton< MemoryRouter >::getInstance().alloc( sizeof( T ), GET_ALLOC_MODE( T ), m_allocator );

   if ( size > 1 )
   {
      allocate( size );
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
Array< T >::Array( const Array& rhs )
   : m_size(rhs.m_size)
   , m_elementsCount( rhs.m_elementsCount )
{
   // use the same allocator as the other array
   m_allocator = rhs.m_allocator;
   m_arr = (T*)TSingleton< MemoryRouter >::getInstance().alloc( sizeof( T ) * m_size, GET_ALLOC_MODE( T ), m_allocator );

   for ( unsigned int i = 0; i < m_elementsCount; ++i )
   {
      new ( &m_arr[i] ) T( rhs.m_arr[i] );
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
Array< T >::~Array()
{
   destruct( m_arr, m_elementsCount );

   TSingleton< MemoryRouter >::getInstance().dealloc( m_arr, GET_ALLOC_MODE( T ) );
   m_arr = NULL;

   m_elementsCount = 0;
   m_size = 0;

   m_allocator = NULL;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void Array< T >::operator=( const Array& rhs )
{
   // delete the old representation
   {
      destruct( m_arr, m_elementsCount );
      TSingleton< MemoryRouter >::getInstance().dealloc( m_arr, GET_ALLOC_MODE( T ) );
   }

   // create the new array
   {
      m_size = rhs.m_size;
      m_elementsCount = rhs.m_elementsCount;

      // use the same allocator as the other array
      m_allocator = rhs.m_allocator;

      m_arr = (T*)TSingleton< MemoryRouter >::getInstance().alloc( sizeof( T ) * m_size, GET_ALLOC_MODE( T ), m_allocator );

      for ( unsigned int i = 0; i < m_elementsCount; ++i )
      {
         new ( &m_arr[i] ) T( rhs.m_arr[i] );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void Array< T >::clear()
{
   destruct( m_arr, m_elementsCount );
   m_elementsCount = 0;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
unsigned int Array< T >::containerSize() const 
{
   return m_size;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
bool Array< T >::empty() const 
{
   return m_elementsCount == 0;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
unsigned int Array< T >::size() const 
{
   return m_elementsCount;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void Array< T >::copyFrom( const Array<T>& rhs )
{
   allocate( size() + rhs.size() );

   for ( uint i = 0; i < rhs.m_elementsCount; ++i )
   {
      new ( &m_arr[m_elementsCount + i] ) T( rhs.m_arr[i] );
   }


   m_elementsCount += rhs.m_elementsCount;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
T* Array< T >::allocArray( uint newSize, uint& outNewSizePow2 ) const
{
   uint newSizePow2 = m_size;
   while (newSizePow2 < newSize)
   {
      newSizePow2 = newSizePow2 << 1;
   }

   MemoryRouter& router = TSingleton< MemoryRouter >::getInstance();
   T* newArr = (T*)router.alloc( sizeof( T ) * newSizePow2, GET_ALLOC_MODE( T ), m_allocator );

   outNewSizePow2 = newSizePow2;
   return newArr;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void Array< T >::allocate( uint newSize )
{
   if ( newSize <= m_size ) 
   {
      return;
   }

   uint newSizePow2 = 0;
   T* newArr = allocArray( newSize, newSizePow2 );

   for (unsigned int i = 0; i < m_elementsCount; ++i)
   {
      new ( &newArr[i] ) T( m_arr[i] );
   }

   destruct( m_arr, m_elementsCount );

   MemoryRouter& router = TSingleton< MemoryRouter >::getInstance();
   router.dealloc( m_arr, GET_ALLOC_MODE( T ) );

   m_arr = newArr;

   m_size = newSizePow2;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void Array< T >::resize(unsigned int newSize )
{
   allocate(newSize);

   if (newSize < m_elementsCount)
   {
      // we're decreasing the size - in which case we don't have to do a thing - the elements
      // will be there
   }
   else
   {
      // append new elements initialized to default values
      for (unsigned int i = m_elementsCount; i < newSize; ++i)
      {
         new ( &m_arr[i] ) T();
      }

   }
   m_elementsCount = newSize;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void Array< T >::resize(unsigned int newSize, const T& defaultVal )
{
   allocate(newSize);

   if (newSize < m_elementsCount)
   {
      // we're decreasing the size - in which case we don't have to do a thing - the elements
      // will be there
   }
   else
   {
      // append new elements initialized to default values
      for (unsigned int i = m_elementsCount; i < newSize; ++i)
      {
         new ( &m_arr[i] ) T( defaultVal );
      }

   }
   m_elementsCount = newSize;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void Array< T >::resizeWithoutInitializing( unsigned int newSize )
{
   allocate( newSize );
   m_elementsCount = newSize;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void Array< T >::broadcastValue( const T& value )
{
   memset( m_arr, value, sizeof( T ) * m_elementsCount );
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void Array< T >::push_back(const T& elem)
{
   unsigned int newElementsCount = m_elementsCount + 1;
   if (newElementsCount > m_size)
   {
      allocate( newElementsCount );
   }

   void* ptr = (void*)&m_arr[m_elementsCount];
   new ( ptr ) T( elem );

   m_elementsCount = newElementsCount;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void Array< T >::push_front( const T& elem )
{
   T* newArr = NULL;
   unsigned int newElementsCount = m_elementsCount + 1;
   if ( newElementsCount > m_size )
   {
      newArr = allocArray( newElementsCount, m_size );
   }
   else
   {
      newArr = allocArray( m_size, m_size );
   }

   copyElems( m_arr, newArr + 1, m_elementsCount );
   new ( newArr ) T( elem );

   destruct( m_arr, m_elementsCount );
   MemoryRouter& router = TSingleton< MemoryRouter >::getInstance();
   router.dealloc( m_arr, GET_ALLOC_MODE( T ) );

   m_arr = newArr;
   m_elementsCount = newElementsCount;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void Array< T >::insert( int idx, const T& elem )
{
   idx = clamp< int >( idx, 0, m_elementsCount );

   T* newArr = NULL;
   unsigned int newElementsCount = m_elementsCount + 1;
   if ( newElementsCount > m_size )
   {
      newArr = allocArray( newElementsCount, m_size );
   }
   else
   {
      newArr = allocArray( m_size, m_size );
   }

   copyElems( m_arr, newArr, idx );
   copyElems( m_arr + idx, newArr + idx + 1, m_elementsCount - idx );
   new ( newArr + idx ) T( elem );

   destruct( m_arr, m_elementsCount );
   MemoryRouter& router = TSingleton< MemoryRouter >::getInstance();
   router.dealloc( m_arr, GET_ALLOC_MODE( T ) );

   m_arr = newArr;
   m_elementsCount = newElementsCount;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
const T& Array< T >::back() const
{
   ASSERT_MSG(m_elementsCount > 0,  "array is empty"); 
   return m_arr[m_elementsCount - 1];
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
T& Array< T >::back()
{
   ASSERT_MSG(m_elementsCount > 0,  "array is empty"); 
   return m_arr[m_elementsCount - 1];
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
const T& Array< T >::front() const
{
   ASSERT_MSG(m_elementsCount > 0,  "array is empty"); 
   return m_arr[0];
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
T& Array< T >::front()
{
   ASSERT_MSG(m_elementsCount > 0,  "array is empty"); 
   return m_arr[0];
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void Array< T >::remove( uint idx )
{
   ASSERT_MSG(idx < m_elementsCount, "index out of array boundaries"); 

   for ( uint i = idx + 1; i < m_elementsCount; ++i )
   {
      new ( &m_arr[i - 1] ) T( m_arr[i] );
      destruct( (T*)&m_arr[i], 1 );
   }

   m_elementsCount--;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
T& Array< T >::at( uint idx )
{
   ASSERT_MSG(idx < m_elementsCount,  "index out of array boundaries"); 
   return m_arr[idx];
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
const T& Array< T >::at(unsigned int idx) const
{
   ASSERT_MSG(idx < m_elementsCount, "index out of array boundaries"); 
   return m_arr[idx];
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
unsigned int Array< T >::find(const T& elem, unsigned int startPos = 0) const
{
   if (startPos >= m_elementsCount)
   {
      return EOA;
   }

   for (unsigned int i = startPos; i < m_elementsCount; ++i)
   {
      if (m_arr[i] == elem)
      {
         return i;
      }
   }

   return EOA;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
Array< T >::operator T*()
{
   return m_arr;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
Array< T >::operator const T*() const
{
   return m_arr;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void Array< T >::destruct( T* firstElem, uint elementsCount )
{
   for ( uint i = 0; i < elementsCount; ++i )
   {
      static_cast< T* >(firstElem)[i].~T();
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void Array< T >::copyElems( const T* src, T* dest, uint elemsCount )
{
   for ( int i = elemsCount - 1; i >= 0; --i )
   {
      new ( &dest[i] ) T( src[i] );
   }
}

///////////////////////////////////////////////////////////////////////////////

#endif // _ARRAY_H
