/// @file   core/InStream.h
/// @brief  base input stream
#ifndef _IN_STREAM_H
#define _IN_STREAM_H

#include <vector>
#include <string>
#include "core\MemoryRouter.h"
#include "core\types.h"
#include "core\Array.h"


///////////////////////////////////////////////////////////////////////////////

/**
 * Base input stream.
 */
class InStream
{
   DECLARE_ALLOCATOR( InStream, AM_DEFAULT );

public:
   virtual ~InStream() {}

   /**
    * Loads data from the stream.
    *
    * @param val     loaded value
    */
   template< typename T >
   InStream& operator>>( T& val );

   /**
    * Loads array of POD data from the stream.
    *
    * @param val     loaded array
    */
   template< typename T >
   InStream& operator>>( std::vector< T >& val );

   /**
    * Loads array of POD data from the stream.
    *
    * @param val     loaded array
    */
   template< typename T >
   InStream& operator>>( Array< T >& val );

   /**
    * Loads a string from the stream
    *
    * @param val     loaded string
    */
   InStream& operator>>( std::string& val );

   /**
    * Skips a certain part of the stream.
    *
    * @param sizeInBytes
    */
   void skip( uint sizeInBytes );

   /**
    * Loading implementation.
    *
    * @param val        pointer to the data buffer
    * @param dataSize   size ( in bytes ) of the data we want to load from the stream
    */
   virtual void load( void* val, unsigned int dataSize ) = 0;
};

///////////////////////////////////////////////////////////////////////////////

#include "core/InStream.inl"

///////////////////////////////////////////////////////////////////////////////

#endif // _IN_STREAM_H
