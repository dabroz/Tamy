/// @file   core\InFileStream.h
/// @brief  stream that stores data in binary file archive
#pragma once

#include "core/InStream.h"
#include <string>


///////////////////////////////////////////////////////////////////////////////

class File;

///////////////////////////////////////////////////////////////////////////////
/**
 * This stream will persist data in a simple binary file archive.
 */
class InFileStream: public InStream
{
   DECLARE_ALLOCATOR( InFileStream, AM_DEFAULT );

private:
   File* m_archive;

public:
   /**
    * Constructor.
    *
    * @param archive    binary file archive
    */
   InFileStream( File* archive );
   ~InFileStream();

protected:
   // ----------------------------------------------------------------------
   // InStream implementation
   // ----------------------------------------------------------------------
   void load( void* val, unsigned int dataSize );

};

///////////////////////////////////////////////////////////////////////////////