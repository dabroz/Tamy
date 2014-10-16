/// @file   core\BestResultsList.h
/// @brief  this collection picks out n-items of the lowest value in the sample thrown at it
#ifndef _BEST_RESULTS_LIST_H
#define _BEST_RESULTS_LIST_H

#include "core\MemoryRouter.h"
#include "core\List.h"


///////////////////////////////////////////////////////////////////////////////

template< typename TWeight, typename TValue >
class BestResultsList
{
   DECLARE_ALLOCATOR( BestResultsList, AM_DEFAULT );

private:
   struct Entry
   {
      DECLARE_ALLOCATOR( Entry, AM_DEFAULT );

      TWeight              m_weight;
      TValue               m_value;

      Entry( const TWeight& weight, const TValue& value )
         : m_weight( weight )
         , m_value( value )
      {
      }

      // ----------------------------------------------------------------------
      // Comparator API
      // ----------------------------------------------------------------------
      static bool isLesser( const Entry& lhs, const Entry& rhs )
      {
         return lhs.m_weight < rhs.m_weight;
      }

      static bool isGreater( const Entry& lhs, const Entry& rhs )
      {
         return lhs.m_weight > rhs.m_weight;
      }

   };

   uint                 m_maxEntries;
   List< Entry >        m_entries;

public:
   /**
   * Iterator for this list.
   */
   class const_iterator
   {
      DECLARE_ALLOCATOR( const_iterator, AM_DEFAULT );

   private:
      typename List< Entry >::const_iterator       m_it;

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
      const_iterator& operator++( );
      const_iterator& operator--( );
      const TValue& operator*( ) const;
      const TValue& operator->( ) const;

   private:
      /**
      * Constructor.
      *
      * @param it
      */
      friend class BestResultsList< TWeight, TValue >;
      const_iterator( const typename List< Entry >::const_iterator& it );
   };


public:
   /**
    * Initializes the list.
    *
    * @param maxResults
    */
   void init( uint maxResults );

   /**
    * Inserts a new sample.
    *
    * @param weight
    * @param value
    */
   void insert( const TWeight& weight, const TValue& value );

   /**
    * Creates an iterator set to the beginning of the list.
    */
   const_iterator begin() const;

   /**
    * Checks if the list is empty.
    */
   inline bool empty() const {
      return m_entries.empty();
   }

   /**
    * Returns the head entry.
    */
   inline TValue& front() {
      return m_entries.front().m_value;
   }

   /**
    * Returns the number of entries in the list.
    */
   inline uint size() const {
      return m_entries.size();
   }

};

///////////////////////////////////////////////////////////////////////////////

#include "core\BestResultsList.inl"

///////////////////////////////////////////////////////////////////////////////

#endif // _BEST_RESULTS_LIST_H
