/// @file   core/OutStream.h
/// @brief  base output stream
#ifndef _OUT_STREAM_H
#define _OUT_STREAM_H

#include <vector>
#include "core\MemoryRouter.h"
#include "core\Array.h"


///////////////////////////////////////////////////////////////////////////////

/**
 * Base output stream.
 */
class OutStream
{
   DECLARE_ALLOCATOR( OutStream, AM_DEFAULT );

public:
   virtual ~OutStream() {}

   /**
    * Saves data to the stream.
    *
    * @param val     saved value
    */
   template< typename T >
   OutStream& operator<<( const T& val );

   /**
    * Saves data to the stream.
    *
    * @param val     saved value
    */
   template< typename T >
   OutStream& operator<<( const std::vector< T >& val );

   /**
    * Saves data to the stream.
    *
    * @param val     saved value
    */
   template< typename T >
   OutStream& operator<<( const Array< T >& val );

   /**
    * Saves a string to the stream.
    *
    * @param val     saved string
    */
   OutStream& operator<<( const std::string& val );

   /**
    * Saving implementation.
    *
    * @param val        pointer to the data buffer
    * @param dataSize   size ( in bytes ) of the data we want to save to the stream
    */
   virtual void save( const void* val, unsigned int dataSize ) = 0;
};

///////////////////////////////////////////////////////////////////////////////

#include "core/OutStream.inl"

///////////////////////////////////////////////////////////////////////////////

#endif // _OUT_STREAM_H
