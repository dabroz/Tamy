/// @file   core\Stack
/// @brief  simple stack implementation
#ifndef _STACK_H
#define _STACK_H

#include <stdio.h>
#include "core/MemoryRouter.h"


///////////////////////////////////////////////////////////////////////////////

class MemoryAllocator;

///////////////////////////////////////////////////////////////////////////////

/**
 * This is an extremely simple implementation of a stack.
 */
template< typename T >
struct Stack
{
   DECLARE_ALLOCATOR( Stack, AM_DEFAULT );

private:
   struct StackSeg
   {
      DECLARE_ALLOCATOR( StackSeg, AM_DEFAULT );

      StackSeg*   m_prev;
      T           m_elem;

      /**
       * Constructor.
       *
       * @param prev
       * @param elem
       */
      StackSeg( StackSeg* prev, T elem );
      ~StackSeg();
   };

public:
   /**
   * Iterator for this list.
   */
   class iterator
   {
      DECLARE_ALLOCATOR( iterator, AM_DEFAULT );

   private:
      Stack*                     m_hostStack;
      StackSeg*                  m_currElement;

   public:
      /**
      * Copy constructor.
      *
      * @param rhs
      */
      iterator( const iterator& rhs );
      ~iterator();

      /**
      * Checks if the iterator's reached either end of the queue
      */
      bool isEnd() const;

      // ----------------------------------------------------------------------
      // Operators
      // ----------------------------------------------------------------------
      void operator=( const iterator& rhs );
      bool operator==( const iterator& rhs ) const;
      bool operator!=( const iterator& rhs ) const;

      iterator& operator++( );       // prefix ++
      iterator operator++( int );   // postfix ++

      // dereferencing operators
      T& operator*( );
      const T& operator*( ) const;
      T& operator->( );
      const T& operator->( ) const;

   private:
      /**
       * Constructor.
       *
       * @param hostList
       * @param startElem
       */
      friend struct Stack< T >;
      iterator( Stack* hostStack, StackSeg* startElem );
   };

private:
   StackSeg*            m_top;

   MemoryAllocator*     m_allocator;

public:
   /**
    * Constructor.
    *
    * @param allocator     custom memory allocator
    */
   Stack( MemoryAllocator* allocator = NULL );
   virtual ~Stack();

   /**
    * Pushes a new element on top of the stack
    */
   void push( T elem );

   /**
    * Pops an element from the top of the stack
    */
   T pop();

   /**
    * Allows to peek at the element that's at the top of the stack ( const version )
    */
   const T& top() const;

   /**
    * Allows to peek at the element that's at the top of the stack ( non-const version )
    */
   T& top();

   /**
    * Tells if the stack is empty.
    */
   bool empty() const;

   // -------------------------------------------------------------------------
   // iterator support
   // -------------------------------------------------------------------------
   /**
    * Returns an iterator that points at the head of the list.
    */
   iterator begin();
};

///////////////////////////////////////////////////////////////////////////////

#include "core/Stack.inl"

///////////////////////////////////////////////////////////////////////////////

#endif // _STACK_H
