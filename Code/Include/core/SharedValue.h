/// @file   core/SharedValue.h
/// @brief  a value that's shared among multiple instances
#ifndef _SHARED_VALUE_H
#define _SHARED_VALUE_H

#include "core/MemoryRouter.h"


///////////////////////////////////////////////////////////////////////////////

/**
 * A value that's shared among multiple instances.
 *
 * This class is NOT THREAD SAFE.
 */
template< typename T >
class TShared
{
   DECLARE_ALLOCATOR( TShared, AM_ALIGNED_16 );

private:
   T*                   m_val;

   uint*                m_refCounter;

public:
   /**
    * Assignment constructor.
    */
   TShared( const T& rhs );

   /**
    * Copy constructor.
    */
   TShared( const TShared< T >& rhs );
   ~TShared();

   /**
    * Returns the stored value.
    */
   const T& get() const;

   /**
    * Gives access to the value.
    */
   T& get();

   /**
    * Returns a pointer to the stored value..
    */
   const T* getPtr() const;

   /**
    * Gives access to the pointer to value.
    */
   T* getPtr();

   // -------------------------------------------------------------------------
   // Operators
   // -------------------------------------------------------------------------
   void operator=( const T& rhs );
   void operator=( const TShared< T >& rhs );
   void operator==( const T& rhs );
   void operator==( const TShared< T >& rhs );
   void operator!=( const T& rhs );
   void operator!=( const TShared< T >& rhs );

private:
   void decreaseRefCounter();
};

///////////////////////////////////////////////////////////////////////////////

#include "core\SharedValue.inl"

///////////////////////////////////////////////////////////////////////////////

#endif // _SHARED_VALUE
