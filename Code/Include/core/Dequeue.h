/// @file   core/Dequeue.h
/// @brief  a LIFO queue
#ifndef _DEQUEUE_H
#define _DEQUEUE_H

#include "core\MemoryRouter.h"


///////////////////////////////////////////////////////////////////////////////

/**
 * A queue works a lot like a Stack, with one difference being that 
 * the element you append first will come out last.
 *
 * There's one more difference however - very often you will need a collection
 * that operates on a constant memory pool.
 * By using its custom constructor that allows to specify the number of elements
 * the queue should be preallocated to, you're constraining the queue to use a constant
 * memory pool.
 * And due to nature of the collection, as long as you don't exceed the specified
 * number of elements stored at once, you're virtually never going to run out of
 * storage capacity.
 */
template< typename T >
class Dequeue
{
   DECLARE_ALLOCATOR( Dequeue, AM_DEFAULT );

private:
   struct Elem
   {
      DECLARE_ALLOCATOR( Elem, AM_DEFAULT );

      T        m_val;
      Elem*    m_next;

      Elem( const T& val )
         : m_val( val )
         , m_next( NULL )
      {}
   };

private:
   MemoryAllocator*        m_allocator;

   Elem*                   m_head;
   Elem*                   m_tail;
   uint                    m_elementsCount;

public:
   /**
    * Default constructor.
    *
    * Creates a LIFO queue that's not constrained to a number of elements stored,
    * but also operates on global memory heap.
    */
   Dequeue();

   /**
    * Constant memory constructor.
    *
    * Creates a LIFO queue that's constrained to a number of elements stored,
    * but operates on a constant memory pool.
    */
   Dequeue( uint elemsCount );
   ~Dequeue();

   /**
    * Appends a new element to the back of the queue.
    */
   void push( const T& elem );

   /**
    * Removes an element from the front of the queue.
    */
   T pop();

   /**
    * Allows to peek at the first element in the queue ( const version )
    */
   const T& front() const;

   /**
    * Allows to peek at the first element in the queue ( non-const version )
    */
   T& front();

   /**
    * Tells if the stack is empty.
    */
   bool empty() const;

   /**
    * Empties the queue.
    */
   void clear();

   /**
    * Returns the number of elements in the queue.
    */
   uint size() const;
};

///////////////////////////////////////////////////////////////////////////////

#include "core/Dequeue.inl"

///////////////////////////////////////////////////////////////////////////////

#endif // _DEQUEUE_H
