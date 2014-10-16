/// @file core\InRawArrayStream.h
/// @brief  an input stream that loads data from a raw memory buffer
#pragma once

#include "core/InStream.h"
#include "core/Array.h"
#include "core/types.h"


///////////////////////////////////////////////////////////////////////////////

/**
 * An input stream that loads data from a raw memory buffer.
 */
class InRawArrayStream : public InStream
{
   DECLARE_ALLOCATOR( InRawArrayStream, AM_DEFAULT );

private:
   const byte*             m_buffer;
   uint                    m_bufferSize;
   int                     m_offset;

public:
   /**
    * Constructor.
    *
    * @param buffer
    * @param bufferSize
    */
   InRawArrayStream( const byte* buffer, uint bufferSize );

   // ----------------------------------------------------------------------
   // InStream implementation
   // ----------------------------------------------------------------------
   void load( void* val, unsigned int dataSize );
};

///////////////////////////////////////////////////////////////////////////////
