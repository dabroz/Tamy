/// @file   core\Array.h
/// @brief  a reusable array container
#ifndef _ARRAY_H
#define _ARRAY_H

#include "core\MemoryRouter.h"
#include "core\Assert.h"
#include "core\DefaultAllocator.h"
#include <stdio.h>


///////////////////////////////////////////////////////////////////////////////

/** 
 * A marker used to indicate that the element is not there in the array.
 */

#define EOA (unsigned int)-1

///////////////////////////////////////////////////////////////////////////////

/**
 * A plain c-style array turned into object oriented gizmo - has the speed
 * of c-style array, and the powers of object oriented containers.
 */
template< typename T >
class Array
{
   DECLARE_ALLOCATOR( Array, AM_DEFAULT );

public:
   unsigned int         m_size;
   unsigned int         m_elementsCount;
   T*                   m_arr;

   MemoryAllocator*     m_allocator;

public:
   /**
    * Constructor.
    *
    * The parameter passed here preallocates the amount of memory necessary
    * for the array to store the passed number of elements.
    * It WILL NOT allocate these elements !!!
    *
    * @param size       initial array size (array will allocate memory to
    *                   incorporate at least that many elements up front).
    * @param allocator  custom memory allocator
    */
   Array( uint size = 1, MemoryAllocator* allocator = NULL );

   /**
    * Copy constructor.
    *
    * @param rhs
    */
   Array( const Array& rhs );
   virtual ~Array();

   /**
    * Assignment operator
    *
    * @param rhs
    */
   void operator=( const Array& rhs );

   /**
    * Clears the contents of the array.
    */
   void clear();

   /**
    * Checks if the container is empty.
    */
   bool empty() const; 

   /**
    * The method returns the number of elements that can be put
    * in the array without it being resized.
    */
   uint containerSize() const;

   /**
    * The method returns the number of elements in the array.
    */
   uint size() const;

   /**
    * This method copies the contents of the array passed in as a parameter
    * and appends it to the back of its own array.
    *
    * @param rhs  array from which we want to copy elements
    */
   void copyFrom( const Array<T>& rhs );

   /**
    * The method reallocates th memory the array is using, preparing it 
    * for accumulation of a number of elements without a need to resize 
    * in the process.
    *
    * CAUTION: An array can't be downsized
    *
    * @param newSize    how many elements we want the array to allocate
    *                   memory for.
    */
   void allocate( uint newSize );

   /**
    * The method resizes the array, creating new elements
    * as necessary.
    *
    * CAUTION: An array can't be downsized - the number of
    * reserved elems will remain the same - however the overflow
    * of the elements will be released.
    *
    * @param newSize       how many elements we want to have in the array
    *                      after this method completes
    * @param defaultValue  value for the automatically created
    *                      elements
    */
   void resize( uint newSize );

   /**
    * The method resizes the array, creating new elements
    * as necessary.
    *
    * CAUTION: An array can't be downsized - the number of
    * reserved elems will remain the same - however the overflow
    * of the elements will be released.
    *
    * @param newSize       how many elements we want to have in the array
    *                      after this method completes
    * @param defaultValue  value for the automatically created
    *                      elements
    */
   void resize( uint newSize, const T& defaultVal );

   /**
    * Resizes an array without initializing new members.
    *
    * Perfect for resizing an array of complex types.
    *
    * @param newSize
    */
   void resizeWithoutInitializing( uint newSize );

   /**
    * Sets all elements in the array to the specified value.
    *
    * CAUTION: Under the hood we're doing a simple 'memset', so don't
    * use this method to initialize arrays of non-POD types.
    *
    * @param value
    */
   void broadcastValue( const T& value );

   /**
    * The method appends a new element at the end of the array, 
    * resizing it if necessary.
    *
    * @param elem    elements that's gonna be appended
    */
   void push_back( const T& elem );

   /**
    * Accesses the last element in the array (providing there is one).
    * (const version)
    */
   const T& back() const;

   /**
    * Accesses the last element in the array (providing there is one).
    */
   T& back();

   /**
    * The method inserts a new element at the beginning of the array, 
    * resizing it if necessary.
    *
    * CAUTION: This operation is VERY expensive in comparison to push_back, as all
    * data the array contains need to be moved one step forward.
    *
    * @param elem    elements that's gonna be inserted
    */
   void push_front( const T& elem );

   /**
    * Accesses the first element in the array (providing there is one).
    * (const version)
    */
   const T& front() const;

   /**
    * Accesses the first element in the array (providing there is one).
    */
   T& front();

   /**
    * The method removes an element at the given index.
    *
    * @param idx     index of an element that's gonna be removed
    */
   void remove( uint idx );

   /**
    * Inserts an element AT the specified position.
    */
   void insert( int idx, const T& elem );

   /**
    * The method allows to access the given element of the array.
    */
   T& at( uint idx );

   /**
    * The method allows to access the given element of the array,
    * making sure it doesn't get modified though (read-only access)
    */
   const T& at( uint idx ) const;

   /**
    * The method locates an element in the array (using the operator==)
    * and returns the position it's on (0-based).
    *
    * EOA is returned if the element is not found.
    *
    * The method allows to start the search from an arbitrary index
    * in order to allow the user to look for multiple elements having
    * the same value.
    */
   unsigned int find( const T& elem, uint startPos = 0 ) const;

   /**
    * Cast operator that allows to use the container as a plain vanilla 
    * c++ standard array.
    */
   operator T*();

   /**
    * Cast operator that allows to use the container as a plain vanilla 
    * c++ standard array. (in a read-only way)
    */
   operator const T*() const;

private:
   /**
    * Destroys a number of elements
    */
   static void destruct( T* firstElem, uint elementsCount );

   /**
    * Allocates a new array and returns it.
    * The array doesn't contain any allocated elements - that's up to you.
    *
    * @param newSize             requested new size
    * @param outNewSizePow2      size fo the allocated array, rounded up to a power of 2
    * @return                    pointer to the new array
    */
   T* allocArray( uint newSize, uint& outNewSizePow2 ) const;

   /**
    * Copies elements between two arrays
    * @param src
    * @param dest
    * @param elemsCount
    */
   static void copyElems( const T* src, T* dest, uint elemsCount );
};

///////////////////////////////////////////////////////////////////////////////

#include "core\Array.inl"

///////////////////////////////////////////////////////////////////////////////

#endif // _ARRAY_H
