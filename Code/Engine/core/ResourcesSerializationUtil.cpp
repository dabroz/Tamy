#include "core/ResourcesSerializationUtil.h"
#include "core/FilePath.h"
#include "core/File.h"
#include "core/InFileStream.h"
#include "core/OutFileStream.h"
#include "core/ReflectionLoader.h"
#include "core/ReflectionSaver.h"
#include "core/Resource.h"
#include "core/Filesystem.h"
#include "core/ReflectionObject.h"
#include "core/IProgressObserver.h"
#include "core/ResourceDependenciesMapper.h"
#include "core/ExternalDependenciesLinker.h"
#include "core/Assert.h"




///////////////////////////////////////////////////////////////////////////////

void ResourcesSerializationUtil::saveResource( Filesystem& filesystem, const Resource* savedResource, const FilePath& differentSavePath )
{
   if ( !savedResource )
   {
      return;
   }

   // see if the user doesn't want to save the resource under a different name
   FilePath savedResourcePath;
   if ( !differentSavePath.empty() )
   {
      savedResourcePath = differentSavePath;
   }
   else
   {
      savedResourcePath = savedResource->getFilePath();
   }

   std::string extension = savedResourcePath.extractExtension();
   std::ios_base::openmode accessMode = Resource::getFileAccessMode( extension );
   File* file = filesystem.open( savedResourcePath, std::ios_base::out | accessMode );

   if ( !file )
   {
      return;
   }

   // save the resource
   OutFileStream outStream( file );
   ReflectionSaver saver( outStream );
   saver.save( savedResource );
}

///////////////////////////////////////////////////////////////////////////////

void ResourcesSerializationUtil::collectExternalDependencies( const ReflectionObject* objectToMap, std::vector< FilePath >& outDependenciesPaths )
{
   // it was successful - map inter-resource dependencies on all loaded objects
   ResourceDepenenciesMapper mapper( outDependenciesPaths );
   mapper.mapDependencies( objectToMap );
}

///////////////////////////////////////////////////////////////////////////////
