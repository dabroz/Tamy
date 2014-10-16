/// @file   core\ConstSizeArray.h
/// @brief  a fast array designed with fast objects reallocation in mind
#ifndef _FAST_RELEASE_ARRAY
#define _FAST_RELEASE_ARRAY


#include "core\MemoryRouter.h"
#include "core\Array.h"
#include "core\Stack.h"


///////////////////////////////////////////////////////////////////////////////

/**
 * A fast array designed with fast objects reallocation in mind.
 *
 * Instead of actually removing an element, gluing the split array back together,
 * it records an empty spot and records it for future use, skipping it as
 * we acces that particular element
 */
template< typename T >
class ConstSizeArray
{
   DECLARE_ALLOCATOR( ConstSizeArray, AM_DEFAULT );

public:
   class iterator
   {
      DECLARE_ALLOCATOR( iterator, AM_DEFAULT );

   private:
      const Array<T>* m_elements;
      const Array<bool>* m_isFree;
      unsigned int m_idx;

   public:
      iterator( const iterator& rhs );

      bool operator==( const iterator& rhs ) const;
      bool operator!=( const iterator& rhs ) const;

      iterator& operator=( const iterator& rhs );

      // prefix
      iterator& operator++();
      iterator& operator--();

      // postfix
      iterator operator++(int);
      iterator operator--(int);

      const T& operator*() const;

   private:
      iterator( const Array<T>& elements, const Array<bool>& isFree, unsigned int idx );

      uint getNext() const;
      uint getPrev() const;

      friend class ConstSizeArray;
   };

private:
   Array< T >*            m_elements;
   Array< bool >*         m_isFree;
   Stack< uint >*         m_freePos;

public:
   /**
    * Constructor.
    *
    * The parameter passed here preallocates the amount of memory necessary
    * for the array to store the passed number of elements.
    * It WILL NOT allocate these elements !!!
    *
    * @param size    initial array size (array will allocate memory to
    *                incorporate at least that many elements up front).
    */
   ConstSizeArray( uint size = 1 );
   ConstSizeArray( const ConstSizeArray& rhs );
   ~ConstSizeArray();

   /**
    * Clears the contents of the array.
    */
   void clear();

   /**
    * The method returns the number of elements in the array.
    */
   uint size() const;

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
   void resize( uint newSize, const T& defaultValue = 0 );

   /**
    * The method inserts a new element to the array, resizing it if necessary.
    *
    * @param elem    elements that's gonna be appended
    * @return        array index at which it was inserted
    */
   unsigned int insert( const T& elem );

   /**
    * The method removes an element at the given index.
    *
    * @param idx     index of an element that's gonna be removed
    */
   void remove( uint idx );

   /**
    * The method gives access to the specified element of the array.
    */
   T& at( uint idx );

   /**
    * The method gives access to the specified element of the array,
    * making sure it doesn't get modified though (read-only access)
    */
   const T& at( uint idx ) const;

   /**
    * The operator gives access to the specified element of the array.
    */
   T& operator[]( uint idx );

   /**
    * The operator gives access to the specified element of the array,
    * making sure it doesn't get modified though (read-only access)
    */
   const T& operator[]( uint idx ) const;

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
   unsigned int find( const T& elem, unsigned int startPos = 0 ) const;

   /**
    * Creates an iterator set at the beginning of the collection.
    *
    * @return  array iterator
    */
   iterator begin() const;

   /**
    * Creates an iterator set at the end of the collection.
    *
    * @return  array iterator
    */
   iterator end() const;
};

///////////////////////////////////////////////////////////////////////////////

#include "core\ConstSizeArray.inl"

///////////////////////////////////////////////////////////////////////////////

#endif // _FAST_RELEASE_ARRAY_H
