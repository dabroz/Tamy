#include "core\ResourceUtils.h"
#include "core\ResourcesManager.h"
#include "core\Resource.h"
#include "core\File.h"
#include "core\Log.h"
#include "core\InFileStream.h"
#include "core\OutFileStream.h"
#include "core\OutArrayStream.h"
#include "core\ReflectionSerializationMacros.h"
#include "core\StreamBuffer.h"
#include "core\ArrayUtils.h"


///////////////////////////////////////////////////////////////////////////////

void ResourceUtils::resaveRepository( ResourcesManager& resMgr )
{
   Filesystem& fs = resMgr.getFilesystem();

   // scan the filesystem in search of all available resources and resave them
   FilePath rootDir( "/" );
   ResourcesResaveScanner scanner( resMgr );
   fs.scan( rootDir, scanner );
}

///////////////////////////////////////////////////////////////////////////////

void ResourceUtils::updateResourceName( Filesystem& fs, const FilePath& oldPath, const FilePath& newPath )
{
   // scan the filesystem in search of all available resources and resave them
   FilePath rootDir( "/" );
   ResourceRenameScanner scanner( fs, oldPath, newPath );
   fs.scan( rootDir, scanner );
}

///////////////////////////////////////////////////////////////////////////////

void ResourceUtils::getNonResourceDependencies( Filesystem& fs, const FilePath& resourcePath, List< FilePath >& outDependenciesPaths )
{
   // load the file's contents and check if it's a Tamy resource
   const std::string extension = resourcePath.extractExtension();
   if ( !Resource::isResource( extension ) )
   {
      // this file's not a Tamy resource
      return;
   }
   const std::ios_base::openmode accessMode = Resource::getFileAccessMode( extension );

   File* file = fs.open( resourcePath, std::ios_base::in | accessMode );
   if ( !file )
   {
      LOG( "ResourceUtils: Can't open the file '%s', even though it turned up during the filesystem scan", resourcePath.c_str() );
      return;
   }

   StreamBuffer<byte> fileContents( *file );
   Array< byte > fileContentsBuf;
   ArrayUtils::createFromRawArray( fileContents.getBuffer(), fileContents.size(), fileContentsBuf );

   // scan the file in search of file paths
   Array< byte > pattern;
   pattern.push_back( ( byte )'/' );

   const int minPathLen = 4; // the path can constist at minimum of 4 characters - /a.b
   const int maxPathLen = 255; // the path name can't be longer than 255 characters.
   int startIdx = 0;
   char fileName[maxPathLen + 1];
   while ( true )
   {
      int idx = ArrayUtils::findStr( fileContentsBuf, pattern, startIdx );
      if ( idx < 4 )
      {
         break;
      }

      int stringLen = fileContentsBuf[idx-4] + ( fileContentsBuf[idx-3] << 8 ) + ( fileContentsBuf[idx-2] << 16 ) + ( fileContentsBuf[idx-1] << 24 );
      if ( stringLen >= minPathLen && stringLen < maxPathLen )
      {
         const char* srcStr = ( const char* ) ( ( byte* ) fileContentsBuf ) + idx;
         strncpy_s( fileName, srcStr, stringLen );
         fileName[stringLen] = 0;
         
         const int effectiveStrLen = strlen(fileName);
         if ( effectiveStrLen == stringLen )
         {
            FilePath filePath( fileName );

            if ( !Resource::isResource( filePath.extractExtension() ) )
            {
               outDependenciesPaths.pushBack( filePath );
            }
         }
         
         startIdx += effectiveStrLen;
      }
      else
      {
         startIdx = idx + 1;
      }
   }

   delete file;
}

///////////////////////////////////////////////////////////////////////////////

void ResourceUtils::findAvailableDefaultName( Filesystem& fs, const FilePath& parentDir, Resource* resource, FilePath& outResourcePath )
{
   const std::string extension = resource->getVirtualExtension();
   const std::string resourceType = resource->getVirtualRTTI().m_name;

   WIN32_FIND_DATA findFileData;
   char tmpFileName[128];
   for ( uint i = 0; i < 0xffff; ++i )
   {
      sprintf_s( tmpFileName, "new%s_%d.%s", resourceType.c_str(), i, extension.c_str() );
      outResourcePath = parentDir + FilePath( tmpFileName );
      std::string absPath = outResourcePath.toAbsolutePath( fs );

      HANDLE hFind = FindFirstFile( absPath.c_str(), &findFileData );
      if ( hFind  == INVALID_HANDLE_VALUE )
      {
         // found it
         return;
      }
      else
      {
         FindClose( hFind );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

ResourcesResaveScanner::ResourcesResaveScanner( ResourcesManager& resMgr )
   : m_resMgr( resMgr )
{
   m_resMgr.collectLoadedResourcesPaths( m_loadedResources );
}

///////////////////////////////////////////////////////////////////////////////

void ResourcesResaveScanner::onDirectory( const FilePath& name )
{
}

///////////////////////////////////////////////////////////////////////////////

void ResourcesResaveScanner::onFile( const FilePath& name )
{
   Resource* resource = m_resMgr.create( name, true );
   if ( resource )
   {
      resource->saveResource();
   }

   // free temporarily loaded resources whenever their number exceeds a predefined number
   uint loadedResourcesCount = m_resMgr.getResourcesCount();
   uint temporaryResourcesCount = loadedResourcesCount - m_loadedResources.size();
   if ( temporaryResourcesCount > m_maxTemporaryResources )
   {
      unloadTemporaryResources();
   }

}

///////////////////////////////////////////////////////////////////////////////

void ResourcesResaveScanner::unloadTemporaryResources()
{
   std::set< FilePath > currentlyLoadedResources;
   m_resMgr.collectLoadedResourcesPaths( currentlyLoadedResources );

   for ( std::set< FilePath >::const_iterator it = currentlyLoadedResources.begin(); it != currentlyLoadedResources.end(); ++it )
   {
      const FilePath& resPath = *it;
      bool resourceWaJustLoaded = ( m_loadedResources.find( resPath ) == m_loadedResources.end() );
      if ( resourceWaJustLoaded )
      {
         m_resMgr.unloadResource( resPath );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

ResourceRenameScanner::ResourceRenameScanner( Filesystem& fs, const FilePath& oldPath, const FilePath& newPath )
   : m_fs( fs )
   , m_oldPath( oldPath )
   , m_newPath( newPath )
{
}

///////////////////////////////////////////////////////////////////////////////

void ResourceRenameScanner::onDirectory( const FilePath& name )
{
   // nothing to do here
}

///////////////////////////////////////////////////////////////////////////////

void ResourceRenameScanner::onFile( const FilePath& name )
{
   // load the file's contents and check if it's a Tamy resource
   const std::string extension = name.extractExtension();
   const std::ios_base::openmode accessMode = Resource::getFileAccessMode( extension );

   File* file = m_fs.open( name, std::ios_base::in | accessMode );
   if ( !file )
   {
      LOG( "ResourceRenameScanner: Can't open the file '%s', even though it turned up during the filesystem scan", name.c_str() );
      return;
   }

   InStream* inStream = new InFileStream( file );

   // The following code depends on the layout of the resource file. The layout
   // is established by the ReflectionLoader and ReflectionSaver classes, so make sure
   // to keep this class up to date when any of the above changes

   // 1. check if it's indeed one of our resources and it's version number
   uint magicNo;
   *inStream >> magicNo;
   if ( magicNo != TAMY_ARCHIVE_MAGIC_NO )
   {
      delete inStream;
      return;
   }

   // load the skip size first - if no array was specified for the results, we'll just skip this section of the stream
   Array< FilePath > resourcesPaths;
   bool fileTouched = loadExternalDependencies( *inStream, resourcesPaths );
   if ( !fileTouched )
   {
      // the file doesn't need to be updated
      delete inStream;
      return;
   }

   // load the remaining contents of the file
   Array< byte > fileContentsBuf;
   loadFileRemainder( file, fileContentsBuf );

   // close the input stream and open an output stream - we're gonna resave the file
   delete inStream;

   file = m_fs.open( name, std::ios_base::out | accessMode );
   OutStream* outStream = new OutFileStream( file );

   *outStream << TAMY_ARCHIVE_MAGIC_NO;

   // save the names of external resources
   saveExternalDependencies( *outStream, resourcesPaths );   

   // save the remaining contents of the file
   saveFileRemainder( file, fileContentsBuf );

   delete outStream;
}

///////////////////////////////////////////////////////////////////////////////

bool ResourceRenameScanner::loadExternalDependencies( InStream& inStream, Array< FilePath >& outResourcesPath ) const
{
   bool fileTouched = false;

   uint skipSize = 0;
   inStream >> skipSize;

   // deserialize the dependencies
   uint dependenciesCount = 0;
   inStream >> dependenciesCount;

   outResourcesPath.resize( dependenciesCount, FilePath() );
   for ( uint i = 0; i < dependenciesCount; ++i )
   {
      inStream >> outResourcesPath[i];

      // if it matches the old path, change it
      if ( outResourcesPath[i] == m_oldPath )
      {
         outResourcesPath[i] = m_newPath;
         fileTouched = true;
      }
   }

   return fileTouched;
}

///////////////////////////////////////////////////////////////////////////////

void ResourceRenameScanner::saveExternalDependencies( OutStream& outStream, const Array< FilePath >& resourcesPath ) const
{
   // use a temporary stream, 'cause we need to write a skip size for this piece of data
   Array< byte > tmpDataBuf;
   OutArrayStream tmpDataStream( tmpDataBuf );

   // number of stored dependencies
   {
      const uint dependenciesCount = resourcesPath.size();
      tmpDataStream << dependenciesCount;

      // dependencies themselves
      for ( uint i = 0; i < dependenciesCount; ++i )
      {
         tmpDataStream << resourcesPath[i];
      }
   }

   // store the size of the data
   uint dataSize = tmpDataBuf.size();
   outStream << dataSize;

   // and then the data itself
   outStream.save( ( byte* ) tmpDataBuf, dataSize );
}

///////////////////////////////////////////////////////////////////////////////

void ResourceRenameScanner::loadFileRemainder( File* file, Array< byte >& outFileContentsBuf ) const
{
   size_t remainingFileSize = file->size() - file->tell();
   outFileContentsBuf.resizeWithoutInitializing( remainingFileSize );
   file->read( ( byte* ) outFileContentsBuf, remainingFileSize );
}

///////////////////////////////////////////////////////////////////////////////

void ResourceRenameScanner::saveFileRemainder( File* file, Array< byte >& fileContentsBuf ) const
{
   file->write( ( byte* ) fileContentsBuf, fileContentsBuf.size() );
}

///////////////////////////////////////////////////////////////////////////////
