#include "core.h"
#include "core\OutFileStream.h"
#include "core\File.h"
#include "core\Assert.h"


///////////////////////////////////////////////////////////////////////////////

OutFileStream::OutFileStream( File* archive )
   : m_archive( archive )
{
   if ( m_archive == NULL )
   {
      ASSERT_MSG( false, "NULL pointer instead a File instance" );
   }
}

///////////////////////////////////////////////////////////////////////////////

OutFileStream::~OutFileStream()
{
   delete m_archive;
   m_archive = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void OutFileStream::save( const void* val, unsigned int dataSize )
{
   m_archive->write( (byte*)val, dataSize );
}

///////////////////////////////////////////////////////////////////////////////
