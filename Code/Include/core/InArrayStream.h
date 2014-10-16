/// @file   core/InArrayStream.h
/// @brief  an input stream that loads data from a memory buffer
#pragma once

#include "core/InStream.h"
#include "core/Array.h"
#include "core/types.h"


///////////////////////////////////////////////////////////////////////////////

/**
 * An input stream that loads data from a memory buffer.
 */
class InArrayStream : public InStream
{
   DECLARE_ALLOCATOR( InArrayStream, AM_DEFAULT );

private:
   const Array< byte >&    m_buffer;
   int                     m_offset;

public:
   /**
    * Constructor.
    *
    * @param buffer
    */
   InArrayStream( const Array< byte >& buffer );

   // ----------------------------------------------------------------------
   // InStream implementation
   // ----------------------------------------------------------------------
   void load( void* val, unsigned int dataSize );
};

///////////////////////////////////////////////////////////////////////////////
