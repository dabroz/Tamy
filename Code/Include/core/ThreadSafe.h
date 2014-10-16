/// @file   core/ThreadSafe.h
/// @brief  a wrapper around a value of some sort that provides a thread-safe access to its value
#ifndef _THREAD_SAFE_H
#define _THREAD_SAFE_H


///////////////////////////////////////////////////////////////////////////////

class CriticalSection;

///////////////////////////////////////////////////////////////////////////////

/**
 * A wrapper around a value of some sort that provides a thread-safe access to its value.
 */
template< typename T >
class TThreadSafe
{
private:
   T                    m_val;
   CriticalSection*     m_lock;

public:
   /**
    * Default constructor.
    */
   TThreadSafe();

   /**
    * Init constructor.
    *
    * @param rhs
    */
   TThreadSafe( const T& rhs );

   /**
    * Copy constructor.
    *
    * @param rhs
    */
   TThreadSafe( typename const TThreadSafe< T >& rhs );
   ~TThreadSafe();

   // -------------------------------------------------------------------------
   // Operators
   // -------------------------------------------------------------------------
   void operator=( const T& rhs );
   bool operator==( const T& rhs ) const;
   bool operator!=( const T& rhs ) const;
   bool operator==( typename const TThreadSafe< T >& rhs ) const;
   bool operator!=( typename const TThreadSafe< T >& rhs ) const;

   /**
    * Data accessor.
    */
   T get() const;
};

///////////////////////////////////////////////////////////////////////////////

#include "core\ThreadSafe.inl"

///////////////////////////////////////////////////////////////////////////////

#endif // _THREAD_SAFE_H
