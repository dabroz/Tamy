#include "core/ReflectionSerializationUtil.h"
#include "core/FilePath.h"
#include "core/InFileStream.h"
#include "core/OutFileStream.h"
#include "core/ReflectionLoader.h"
#include "core/ReflectionSaver.h"
#include "core/Resource.h"
#include "core/ResourcesManager.h"
#include "core/ReflectionObject.h"
#include "core/IProgressObserver.h"
#include "core/ResourceDependenciesMapper.h"
#include "core/Assert.h"


///////////////////////////////////////////////////////////////////////////////

void ReflectionSerializationUtil::saveObject( const ReflectionObject* object, const FilePath& savePath, IProgressObserver* progressObserver )
{
   std::vector< const ReflectionObject* > objects;
   objects.push_back( object );
   saveObjects( objects, savePath, progressObserver );
}

///////////////////////////////////////////////////////////////////////////////

void ReflectionSerializationUtil::saveObjects( std::vector< const ReflectionObject* >& objects, const FilePath& savePath, IProgressObserver* progressObserver )
{
   // open a file to which the objects will be saved and create a saver
   ResourcesManager& resMgr = TSingleton< ResourcesManager >::getInstance();
   Filesystem& filesystem = resMgr.getFilesystem();

   File* file = filesystem.open( savePath, std::ios_base::out | std::ios_base::binary );
   OutFileStream stream( file );
   ReflectionSaver saver( stream );

   // notify about the serialization progress
   std::vector< const Resource* > externalDependencies;
   uint objectsCount = objects.size();
   if ( progressObserver ) 
   {
      progressObserver->initialize( objectsCount );
      progressObserver->setStatus( "Saving objects" );
   }

   // serialize the objects
   for ( uint i = 0; i < objectsCount; ++i )
   {
      saver.save( objects[i] );

      // safety measures - check if there were any external dependencies in the saved object
      saver.collectExternalDependencies( externalDependencies );
      ASSERT_MSG( externalDependencies.empty(), "The serialized object contains external dependencies and therefore will be incomplete after deserialization" );

      if ( progressObserver )
      {
         progressObserver->advance();
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

ReflectionObject* ReflectionSerializationUtil::loadObject( const FilePath& loadPath, IProgressObserver* progressObserver )
{
   std::vector< ReflectionObject* > loadedObjects;

   loadObjects( loadPath, loadedObjects, progressObserver );

   return loadedObjects.empty() ? NULL : loadedObjects[0];
}

///////////////////////////////////////////////////////////////////////////////

void ReflectionSerializationUtil::loadObjects( const FilePath& loadPath, std::vector< ReflectionObject* >& outObjects, IProgressObserver* progressObserver )
{
   // open a file to which the objects will be saved and create a saver
   ResourcesManager& resMgr = TSingleton< ResourcesManager >::getInstance();
   Filesystem& filesystem = resMgr.getFilesystem();

   File* file = filesystem.open( loadPath, std::ios_base::in | std::ios_base::binary );
   InFileStream stream( file );

   std::vector< FilePath > resourcesToLoad;
   std::vector< FilePath > resourcesMap;
   ReflectionLoader loader;
   loader.deserialize( stream, &resourcesToLoad, &resourcesMap );

   ASSERT_MSG( resourcesToLoad.empty(), "The archive contains references to external Resources, which won't be loaded by this method. Use 'loadResources' instead." );
   if ( !resourcesToLoad.empty() )
   {
      return;
   }

   // notify about the serialization progress
   uint objectsCount = loader.m_loadedObjects.size();
   std::vector< FilePath > externalDependencies;
   if ( progressObserver ) 
   {
      progressObserver->initialize( objectsCount );
      progressObserver->setStatus( "Loading objects" );
   }

   // serialize the objects
   for ( uint i = 0; i < objectsCount; ++i )
   {
      ReflectionObject* loadedObject = loader.getNextObject< ReflectionObject >();
      outObjects.push_back( loadedObject );
      if ( progressObserver )
      {
         progressObserver->advance();
      }
   }

   // last but not least, make sure that all loaded objects are informed that they were loaded
   for ( std::list< ReflectionObject* >::const_iterator it = loader.m_allLoadedObjects.begin(); it != loader.m_allLoadedObjects.end(); ++it )
   {
      ReflectionObject* object = *it;
      if ( object )
      {
         object->notifyObjectLoaded();
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void ReflectionSerializationUtil::collectExternalDependencies( const ReflectionObject* objectToMap, std::vector< FilePath >& outDependenciesPaths )
{
   // it was successful - map inter-resource dependencies on all loaded objects
   ResourceDepenenciesMapper mapper( outDependenciesPaths );
   mapper.mapDependencies( objectToMap );
}

///////////////////////////////////////////////////////////////////////////////
