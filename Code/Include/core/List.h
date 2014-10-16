/// @file   core/List.h
/// @brief  double-linked list implementation
#ifndef _LIST_H
#define _LIST_H

#include "core/MemoryRouter.h"
#include "core/types.h"


///////////////////////////////////////////////////////////////////////////////

class MemoryAllocator;

///////////////////////////////////////////////////////////////////////////////

/**
 * Double-linked list implementation.
 */
template< typename T >
class List
{
   DECLARE_ALLOCATOR( List, AM_DEFAULT );

private:
   struct Elem
   {
      DECLARE_ALLOCATOR( Elem, AM_DEFAULT );

      T        m_element;
      Elem*    m_nextElem;
      Elem*    m_prevElem;

      Elem( const T& element ) : m_element( element ), m_nextElem( NULL ), m_prevElem( NULL ) {}
      Elem( const Elem& rhs ) : m_element( rhs.m_element ), m_nextElem( NULL ), m_prevElem( NULL ) {}
      ~Elem() { m_nextElem = NULL; m_prevElem = NULL; }
   };

   struct ElemForRemoval
   {
      DECLARE_ALLOCATOR( ElemForRemoval, AM_DEFAULT );

      Elem*                m_elem;
      ElemForRemoval*      m_next;

      ElemForRemoval( Elem* elem, ElemForRemoval* next )
         : m_elem( elem )
         , m_next( next )
      {
      }
   };

public:
   /**
    * Iterator for this list.
    */
   class iterator
   {
      DECLARE_ALLOCATOR( iterator, AM_DEFAULT );

   private:
      List*                   m_hostList;
      Elem*                   m_currElement;

      ElemForRemoval*         m_elemForRemoval;

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

      /**
       * Marks currently selected element for removal.
       *
       * All marked elements will be removed when the iterator goes out of scope.
       */
      void markForRemoval();

      /**
       * Removes all elements that have been marked for removal prior to this call.
       */
      void processElementsRemoval();

      // ----------------------------------------------------------------------
      // Operators
      // ----------------------------------------------------------------------
      void operator=( const iterator& rhs );
      bool operator==( const iterator& rhs ) const;
      bool operator!=( const iterator& rhs ) const;

      iterator& operator++();       // prefix ++
      iterator operator++( int );   // postfix ++

      iterator& operator--();       // prefix --
      iterator operator--( int );   // postfix --

      // dereferencing operators
      T& operator*(); 
      const T& operator*() const;
      T& operator->( );
      const T& operator->( ) const;

   private:
      /**
       * Constructor.
       *
       * @param hostList
       * @param startElem
       */
      friend class List< T >;
      iterator( List* hostList, Elem* startElem );
   };

   /**
    * Constant iterator for this list.
    */
   class const_iterator
   {
      DECLARE_ALLOCATOR( const_iterator, AM_DEFAULT );

   private:
      const List*             m_hostList;
      Elem*                   m_currElement;

      ElemForRemoval*         m_elemForRemoval;

   public:
      /**
       * Copy constructor.
       *
       * @param rhs
       */
      const_iterator( const const_iterator& rhs );
      ~const_iterator();

      /**
       * Checks if the iterator's reached either end of the queue
       */
      bool isEnd() const;

      // ----------------------------------------------------------------------
      // Operators
      // ----------------------------------------------------------------------
      void operator=( const const_iterator& rhs );
      bool operator==( const const_iterator& rhs ) const;
      bool operator!=( const const_iterator& rhs ) const;

      const_iterator& operator++();       // prefix ++
      const_iterator operator++( int );   // postfix ++

      const_iterator& operator--();       // prefix --
      const_iterator operator--( int );   // postfix --

      // dereferencing operators
      const T& operator*() const;
      const T& operator->() const;

   private:
      /**
       * Constructor.
       *
       * @param hostList
       * @param startElem
       */
      friend class List< T >;
      const_iterator( const List* hostList, Elem* startElem );
   };

private:

   MemoryAllocator*     m_allocator;

   Elem*                m_head;
   Elem*                m_tail;

   uint                 m_elementsCount;

public:
   /**
    * Constructor.
    *
    * @param allocator     memory allocator to use with the list
    */
   List( MemoryAllocator* allocator = NULL );

   /**
    * Copy constructor.
    *
    * @param rhs
    */
   List( const List& rhs );
   ~List();

   /**
    * Returns the size of a single allocated element.
    * Use this if you want to create a custom allocator that deals in fragmented memory chunks.
    */
   static uint getElemSize();

   /**
    * Checks if the list stores no elements.
    */
   bool empty() const;

   /**
    * Appends an element at the back of the list.
    *
    * @param elem
    */
   void pushBack( const T& elem );

   /**
    * Removes the last element on the list.
    */
   void popBack();

   /**
    * Inserts an element at the front of the list.
    *
    * @param elem
    */
   void pushFront( const T& elem );

   /**
    * Removes the first element on the list.
    */
   void popFront();

   /**
    * Inserts an element before the specified position.
    *
    * ie. If we have a list 1-->2-->3 and an iterator 'it' that currently points at 2,
    *     and we call insert( it, 4 ), we're gonna end up with a list 1-->4-->2-->3
    *
    * Because the insertion works this way, you CAN'T continue iterating using this iterator,
    * and it will be invalidated.
    *
    * @param it
    * @param insertedElem
    */
   void insert( iterator& it, const T& insertedElem );

   /**
    * Accesses the last element on the list ( const version ).
    */
   const T& back() const;

   /**
    * Accesses the last element on the list ( non-const version ).
    */
   T& back();

   /**
    * Accesses the first element on the list ( const version ).
    */
   const T& front() const;

   /**
    * Accesses the first element on the list ( non-const version ).
    */
   T& front();

   /**
    * Removes element pointed to by the specified iterator.
    *
    * @param it
    */
   void remove( iterator& it );

   /**
    * Returns the number of elements in the list.
    */
   uint size() const;

   /**
    * Clears the list contents.
    */
   void clear();

   // -------------------------------------------------------------------------
   // iterator support
   // -------------------------------------------------------------------------
   /**
    * Returns an iterator that points at the head of the list.
    */
   iterator begin();

   /**
    * Returns an iterator that points at the tail of the list.
    */
   iterator end();

   /**
    * Returns an iterator that points at the head of the list ( const version ).
    */
   const_iterator begin() const;

   /**
    * Returns an iterator that points at the tail of the list  ( const version ).
    */
   const_iterator end() const;

   // -------------------------------------------------------------------------
   // Operators
   // -------------------------------------------------------------------------
   void operator=( const List& rhs );

private:
   /**
    * Removes the specified element.
    *
    * @param element
    */
   void remove( Elem* element );
};

///////////////////////////////////////////////////////////////////////////////

#include "core/List.inl"

///////////////////////////////////////////////////////////////////////////////

#endif // _LIST_H
