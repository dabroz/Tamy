#include "core.h"
#include "core/FilePath.h"
#include "core/InStream.h"
#include "core/OutStream.h"
#include "core/Filesystem.h"
#include "core/FilesystemUtils.h"
#include "core/StringUtils.h"
#include "core/Profiler.h"


///////////////////////////////////////////////////////////////////////////////

FilePath::FilePath()
{
}

///////////////////////////////////////////////////////////////////////////////

FilePath::~FilePath()
{
}

///////////////////////////////////////////////////////////////////////////////

FilePath::FilePath( const std::string& path )
{
   FilesystemUtils::normalize( path, m_relativePath );
}

///////////////////////////////////////////////////////////////////////////////

FilePath::FilePath( const FilePath& rhs )
{
   m_relativePath = rhs.m_relativePath;
}

///////////////////////////////////////////////////////////////////////////////

void FilePath::set( const std::string& path )
{
   FilesystemUtils::normalize( path, m_relativePath );
}

///////////////////////////////////////////////////////////////////////////////

bool FilePath::isLocatedInDir( const FilePath& path ) const
{
   return ( m_relativePath.find( path.m_relativePath ) != std::string::npos );
}

///////////////////////////////////////////////////////////////////////////////

FilePath FilePath::operator+( const FilePath& rhs ) const
{
   FilePath newPath;
   std::string path = m_relativePath + rhs.getRelativePath();
   FilesystemUtils::normalize( path, newPath.m_relativePath );

   return newPath;
}

///////////////////////////////////////////////////////////////////////////////

FilePath& FilePath::operator+=( const FilePath& rhs )
{
   std::string path = m_relativePath + rhs.getRelativePath();
   FilesystemUtils::normalize( path, m_relativePath );

   return *this;
}

///////////////////////////////////////////////////////////////////////////////

std::string operator+( const char* str, const FilePath& path )
{
   return str + path.m_relativePath;
}

///////////////////////////////////////////////////////////////////////////////

std::string FilePath::extractExtension() const
{
   return FilesystemUtils::extractExtension( m_relativePath );
}

///////////////////////////////////////////////////////////////////////////////

void FilePath::extractDir( FilePath& outDir ) const
{
   outDir.m_relativePath = FilesystemUtils::extractDir( m_relativePath );
}

///////////////////////////////////////////////////////////////////////////////

void FilePath::leaveDir(  unsigned int levels, FilePath& outDirectory ) const
{
   FilesystemUtils::leaveDir( m_relativePath, levels, outDirectory.m_relativePath );
}

///////////////////////////////////////////////////////////////////////////////

std::string FilePath::extractNodeName() const
{
   return FilesystemUtils::extractNodeName( m_relativePath );
}

///////////////////////////////////////////////////////////////////////////////

void FilePath::changeFileExtension( const std::string& newExtension, FilePath& outPath ) const
{
   if ( newExtension.empty() )
   {
      // nothing to change
      outPath.m_relativePath = m_relativePath;
      return;
   }

   std::size_t pos = m_relativePath.find_last_of( "." );
   if ( pos != std::string::npos )
   {
      outPath.m_relativePath = m_relativePath.substr( 0, pos + 1 ) + newExtension;
   }
   else
   {
      outPath.m_relativePath = m_relativePath + "." + newExtension;
   }
}

///////////////////////////////////////////////////////////////////////////////

void FilePath::changeFileName( const std::string& newName, FilePath& outPath ) const
{
   FilePath dir;
   extractDir( dir );
   std::string ext = extractExtension();
   char tmpPath[512];
   sprintf_s( tmpPath, "%s%s.%s", dir.c_str(), newName.c_str(), ext.c_str() );

   outPath.set( tmpPath );
}

///////////////////////////////////////////////////////////////////////////////

void FilePath::swapPart( const std::string& oldPart, const std::string& newPart, FilePath& outPath ) const
{
   int pos = m_relativePath.find( oldPart );
   if ( pos == std::string::npos )
   {
      // nothing to swap
      outPath = m_relativePath;
      return;
   }

   std::string newPath = m_relativePath.substr( 0, pos );
   newPath += newPart;
   newPath += m_relativePath.substr( pos + oldPart.length() );

   std::string normalizedPath;
   FilesystemUtils::normalize( newPath, normalizedPath );
   outPath.set( normalizedPath );
}

///////////////////////////////////////////////////////////////////////////////

void FilePath::appendPostfix( const std::string& posfix, FilePath& outPath ) const
{
   if ( m_relativePath.empty() || m_relativePath == "/" )
   {
      // an empty path can't be altered
      return;
   }

   if ( posfix.empty() )
   {
      // nothing to change
      outPath.m_relativePath = m_relativePath;
      return;
   }

   std::size_t pos = m_relativePath.find_last_of( "." );
   if ( pos != std::string::npos )
   {
      outPath.m_relativePath = m_relativePath.substr( 0, pos ) + posfix + m_relativePath.substr( pos );
   }
   else
   {
      outPath.m_relativePath = m_relativePath + posfix;
   }
}

///////////////////////////////////////////////////////////////////////////////

std::string FilePath::toAbsolutePath( const Filesystem& fs ) const
{
   return fs.getCurrRoot() + m_relativePath;
}

///////////////////////////////////////////////////////////////////////////////

void FilePath::toAbsolutePath( const Filesystem& fs, char* outAbsPath ) const
{
   int firstPartLen = fs.getCurrRoot().length();
   int secondPartLen = m_relativePath.length();

   const char* firstPathPart = fs.getCurrRoot().c_str();
   const char* secondPathPart = m_relativePath.c_str();
   memcpy( outAbsPath, firstPathPart, firstPartLen );
   memcpy( outAbsPath + firstPartLen, secondPathPart, secondPartLen );
   outAbsPath[firstPartLen + secondPartLen ] = 0;
}

///////////////////////////////////////////////////////////////////////////////

OutStream& operator<<( OutStream& serializer, const FilePath& path )
{
   serializer << path.m_relativePath;
   return serializer;
}

///////////////////////////////////////////////////////////////////////////////

InStream& operator>>( InStream& serializer, FilePath& path )
{
   serializer >> path.m_relativePath;
   return serializer;
}

///////////////////////////////////////////////////////////////////////////////

void FilePath::getElements( Array< std::string >& outPathElements ) const
{
   StringUtils::tokenize( m_relativePath, "/", outPathElements );
}

///////////////////////////////////////////////////////////////////////////////

void FilePath::getElements( List< std::string >& outPathElements ) const
{
   StringUtils::tokenize( m_relativePath, "/", outPathElements );
}

///////////////////////////////////////////////////////////////////////////////
