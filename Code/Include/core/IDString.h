/// @file   core\IDString.h
/// @brief  a pool of string-like IDs and the ID class
#pragma once

#include <string>
#include "core\MemoryRouter.h"
#include "core\types.h"
#include "core\Array.h"
#include "core\Singleton.h"


///////////////////////////////////////////////////////////////////////////////

/**
 * An IDString should be used in performance-sensitive places, where we can't
 * afford string copying and management.
 *
 * The class provides a functionality similar to that of a string, but it's much
 * faster and cheaper, because instead of holding an actual string, it simply
 * holds a reference to a single string stored in a singleton instance of a pool
 * of strings ( IDStringsPool class instance that is ).
 */
class IDString
{
   DECLARE_ALLOCATOR( IDString, AM_DEFAULT );

private:
   uint           m_stringId;

public:
   /**
    * Constructor
    *
    * @param stringId         id of a string that's already in the ids pool
    */
   IDString( uint stringId = 0 );

   /**
    * Constructor.
    *
    * @param str           string we want to create an ID for
    */
   IDString( const char* str );

   /**
    * Constructor
    *
    * @param str           string we want to create an ID for
    */
   IDString( const std::string& str );

   /**
    * Copy constructor.
    */
   IDString( const IDString& rhs );

   // -------------------------------------------------------------------------
   // Common operators
   // -------------------------------------------------------------------------
   bool operator==( const IDString& rhs ) const;
   bool operator!=( const IDString& rhs ) const;
   IDString& operator=( const IDString& rhs );
   IDString& operator=( const char* str );
   IDString& operator=( const std::string& str );

   /**
    * Returns the actual string this ID represents.
    */
   const char* c_str() const;

   /**
    * Returns an id of the string in the pool.
    */
   inline uint getId() const { return m_stringId; }
};

///////////////////////////////////////////////////////////////////////////////

/**
 * A singleton repository of all string referenced by IDString instances.
 */
class IDStringsPool
{
   PRIORITY_SINGLETON( 5 );

private:
   Array< char* >             m_strings;

public:
   /**
    * Constructor.
    */
   IDStringsPool();

   /**
    * Singleton constructor.
    */
   IDStringsPool( const SingletonConstruct& );
   ~IDStringsPool();

   /**
    * Registers a string and assigns it a unique id.
    *
    * @param str
    */
   uint registerString( const char* str );

   /**
    * Returns a string corresponding to the specified id.
    *
    * @para stringId
    */
   const char* getString( uint stringId ) const;
};

///////////////////////////////////////////////////////////////////////////////
