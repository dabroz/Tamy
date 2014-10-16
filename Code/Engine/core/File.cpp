#include "core.h"
#include "core\File.h"
#include "core\Assert.h"
#include "core\Filesystem.h"
#include "core\FilePath.h"
#include "core\Profiler.h"
#include <io.h>


///////////////////////////////////////////////////////////////////////////////

File::File( Filesystem& hostFS, const FilePath& name, const std::ios_base::open_mode openMode )
   : m_hostFS( hostFS )
   , m_name( name )
   , m_openMode( openMode )
   , m_file( NULL )
{
   // construct the attributes string
   char openModeStr[4];
   {
      char idx = 0;
   
      if ( ( ( openMode & std::ios_base::in ) == std::ios_base::in ) && ( ( openMode & std::ios_base::out ) == std::ios_base::out ) )
      {
         if ( ( openMode & std::ios_base::trunc ) == std::ios_base::trunc )
         {
            openModeStr[0] = 'w';
            openModeStr[1] = '+';
         }
         else
         {
            openModeStr[0] = 'a';
            openModeStr[1] = '+';
         }
         idx = 2;
      }
      else if ( ( openMode & std::ios_base::in ) == std::ios_base::in )
      {
         openModeStr[0] = 'r';
         idx = 1;
      }
      else if ( ( openMode & std::ios_base::out ) == std::ios_base::out )
      {
         openModeStr[0] = 'w';
         idx = 1;
      }

      if ( ( openMode & std::ios_base::binary ) == std::ios_base::binary )
      {
         openModeStr[idx] = 'b';
         ++idx;
      }
      openModeStr[idx] = 0;
   }

   // get the absolute path to the opened file
   char absoluteName[255];
   m_name.toAbsolutePath( m_hostFS, absoluteName );

   // open the file
   fopen_s( &m_file, absoluteName, openModeStr );
}

///////////////////////////////////////////////////////////////////////////////

File::~File()
{
   if ( m_file )
   {
      fclose( m_file );
   }
   m_file = NULL;

   // notify the filesystem that the file is close AFTER it's actually been closed
   m_hostFS.notifyFileClosed( m_name );
}

///////////////////////////////////////////////////////////////////////////////

void File::seek(DWORD offset, std::ios_base::seekdir dir)
{
   ASSERT_MSG( m_file, "File not opened" );

   int whence;
   switch(dir)
   {
   case std::ios_base::beg: 
      {
         whence = SEEK_SET; 
         break;
      }

   case std::ios_base::end: 
      {
         whence = SEEK_END; 
         break;
      }

   case std::ios_base::cur: 
   default:
      {
         whence = SEEK_CUR; 
         break;
      }
   }
   fseek(m_file, offset, whence);
}

///////////////////////////////////////////////////////////////////////////////

std::size_t File::tell() const
{
   ASSERT_MSG( m_file, "File not opened" );
   return ftell(m_file);
}

///////////////////////////////////////////////////////////////////////////////

std::size_t File::read(byte* buffer, std::size_t size)
{
   ASSERT_MSG( m_file, "File not opened" );
   std::size_t bytesRead = fread(buffer, 1, size, m_file);
   return bytesRead;
}

///////////////////////////////////////////////////////////////////////////////

std::size_t File::write(byte* buffer, std::size_t size)
{
   ASSERT_MSG( m_file, "File not opened" );
   std::size_t bytesWritten = fwrite(buffer, 1, size, m_file);
   return bytesWritten;
}

///////////////////////////////////////////////////////////////////////////////

void File::readString(char* outStrData, std::size_t size)
{
   ASSERT_MSG( m_file, "File not opened" );
   fgets(outStrData, size, m_file);
}

///////////////////////////////////////////////////////////////////////////////

void File::writeString(const char* strData)
{
   ASSERT_MSG( m_file, "File not opened" );

   if ( fputs(strData, m_file) == EOF )
   {
      ASSERT_MSG( false, "Can't write a string to a file" );
   }
}

///////////////////////////////////////////////////////////////////////////////

void File::flush()
{
   ASSERT_MSG( m_file, "File not opened" );
   fflush(m_file);
}

///////////////////////////////////////////////////////////////////////////////

bool File::eof() const
{
   ASSERT_MSG( m_file, "File not opened" );
   return feof(m_file) != 0;
}

///////////////////////////////////////////////////////////////////////////////

std::size_t File::size() const
{
   ASSERT_MSG( m_file, "File not opened" );
   return _filelength(m_file->_file); 
}

///////////////////////////////////////////////////////////////////////////////

void File::setSize(std::size_t newSize)
{
   ASSERT_MSG( m_file, "File not opened" );
   if ( _chsize(m_file->_file, newSize) != 0 )
   {
      ASSERT_MSG( false, "Couldn't resize a file" );
   }
}

///////////////////////////////////////////////////////////////////////////////
