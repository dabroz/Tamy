/// @file   core\OutFileStream.h
/// @brief  stream that stores data in binary file archive
#pragma once

#include "core/OutStream.h"
#include <string>


///////////////////////////////////////////////////////////////////////////////

class File;

///////////////////////////////////////////////////////////////////////////////
/**
 * This stream will persist data in a simple binary file archive.
 */
class OutFileStream: public OutStream
{
   DECLARE_ALLOCATOR( OutFileStream, AM_DEFAULT );

private:
   File* m_archive;

public:
   /**
    * Constructor.
    *
    * @param archive    binary file archive
    */
   OutFileStream( File* archive );
   ~OutFileStream();

protected:
   // ----------------------------------------------------------------------
   // OutStream implementation
   // ----------------------------------------------------------------------
   void save( const void* val, unsigned int dataSize );

};

///////////////////////////////////////////////////////////////////////////////