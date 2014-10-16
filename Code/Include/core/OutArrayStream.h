/// @file   core/OutArrayStream.h
/// @brief  an output stream that saves data to a memory buffer
#pragma once

#include "core/OutStream.h"
#include "core/Array.h"
#include "core/types.h"


///////////////////////////////////////////////////////////////////////////////

/**
 * An output stream that loads data from a memory buffer.
 */
class OutArrayStream : public OutStream
{
   DECLARE_ALLOCATOR( OutArrayStream, AM_DEFAULT );

private:
   Array< byte >&          m_buffer;

public:
   /**
    * Constructor.
    *
    * @param buffer
    */
   OutArrayStream( Array< byte >& buffer );

   // ----------------------------------------------------------------------
   // OutStream implementation
   // ----------------------------------------------------------------------
   void save( const void* val, unsigned int dataSize );
};

///////////////////////////////////////////////////////////////////////////////
