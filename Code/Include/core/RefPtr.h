/// @file   core/RefPtr.h
/// @brief  reference counting pointer
#ifndef _REF_PTR_H
#define _REF_PTR_H

#include "core\MemoryRouter.h"
#include <string>


///////////////////////////////////////////////////////////////////////////////

class ReflectionObject;

///////////////////////////////////////////////////////////////////////////////

/**
 * An object of this class manages the lifetime of the specified
 * ReflectionObject by increasing and decreasing the number of references
 * to it.
 */
class RefPtr
{
   DECLARE_ALLOCATOR( RefPtr, AM_DEFAULT );

protected:
   ReflectionObject*       m_obj;

public:
   /**
    * Constructor.
    *
    * @param obj
    */
   RefPtr( ReflectionObject* obj = NULL );

   /**
    * Copy constructor.
    */
   RefPtr( const RefPtr& rhs );
   virtual ~RefPtr();

   /**
    * Checks if the pointer is NULL.
    */
   inline bool isNull() const { return m_obj == NULL; }

   /**
    * Checks if the pointer is not NULL.
    */
   inline bool isNotNull() const { return m_obj != NULL; }

   /**
    * Sets a new pointer value WITHOUT increasing its reference counter nor destroying the previous object.
    *
    * Used ONLY during serialization.
    *
    * @param obj
    */
   void setDuringSerialization( ReflectionObject* obj );

   // -------------------------------------------------------------------------
   // Operators
   // -------------------------------------------------------------------------
   RefPtr& operator=( const RefPtr& rhs );
   RefPtr& operator=( ReflectionObject* obj );
   bool operator==( const RefPtr& rhs ) const;
   bool operator==( const ReflectionObject* obj ) const;
   bool operator!=( const RefPtr& rhs ) const;
   bool operator!=( const ReflectionObject* obj ) const;
   bool operator<( const RefPtr& rhs ) const;
   bool operator<( const ReflectionObject* obj ) const;
   bool operator>( const RefPtr& rhs ) const;
   bool operator>( const ReflectionObject* obj ) const;
};

///////////////////////////////////////////////////////////////////////////////

template< typename T >
class TRefPtr : public RefPtr
{
public:
   /**
    * Constructor.
    *
    * @param obj
    */
   TRefPtr( T* obj = NULL );

   /**
    * Copy constructor.
    */
   TRefPtr( const TRefPtr& rhs );
   virtual ~TRefPtr();

   /**
    * Returns the raw version of stored pointer.
    */
   T* get() { return (T*)m_obj; }

   /**
    * Returns the raw version of stored pointer ( const version )
    */
   T* get() const { return (T*)m_obj; }

   // -------------------------------------------------------------------------
   // Operators
   // -------------------------------------------------------------------------
   TRefPtr< T >& operator=( const TRefPtr< T >& rhs );
   TRefPtr< T >& operator=( ReflectionObject* obj );

   // -------------------------------------------------------------------------
   // Access operators
   // -------------------------------------------------------------------------
   T* operator->()               { return (T*)m_obj; }
   T* operator->() const         { return (T*)m_obj; }
   T& operator*()                { return (T&)*m_obj; }
   T& operator*() const          { return (T&)*m_obj; }
};

///////////////////////////////////////////////////////////////////////////////

#include "core/RefPtr.inl"

///////////////////////////////////////////////////////////////////////////////

#endif // _REF_PTR_H
