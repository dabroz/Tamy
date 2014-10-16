/// @file   core/OutRawArrayStream.h
/// @brief  an output stream that saves data to a raw memory buffer
#pragma once

#include "core/OutStream.h"
#include "core/Array.h"
#include "core/types.h"


///////////////////////////////////////////////////////////////////////////////

/**
 * An output stream that loads data from a memory buffer.
 */
class OutRawArrayStream : public OutStream
{
   DECLARE_ALLOCATOR( OutRawArrayStream, AM_DEFAULT );

private:
   byte*                   m_buffer;
   uint                    m_bufferSize;
   uint                    m_offset;

public:
   /**
    * Constructor.
    *
    * @param buffer
    * @param bufferSize
    */
   OutRawArrayStream( byte* buffer, uint bufferSize );

   // ----------------------------------------------------------------------
   // OutStream implementation
   // ----------------------------------------------------------------------
   void save( const void* val, unsigned int dataSize );
};

///////////////////////////////////////////////////////////////////////////////
