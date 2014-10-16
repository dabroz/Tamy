/// @file   core/ReflectionSerializationUtil.h
/// @brief  utility for loading and saving standalone objects
#pragma once

#include <vector>


///////////////////////////////////////////////////////////////////////////////

class FilePath;
class ReflectionObject;
class Resource;
class IProgressObserver;

///////////////////////////////////////////////////////////////////////////////

/**
 * Utility for loading and saving standalone objects.
 */
class ReflectionSerializationUtil
{
public:
   // -------------------------------------------------------------------------
   // Standalone objects
   // -------------------------------------------------------------------------
   /**
    * Saves a single reflection object. Use this method to serialize
    * a standalone object that DOESN'T depend on any Resource instances.
    *
    * CAUTION: an assertion will be thrown if the object depends on a Resource.
    *
    * @param object
    * @param savePath
    * @param progressObserver ( optional )
    */
   static void saveObject( const ReflectionObject* object, const FilePath& savePath, IProgressObserver* progressObserver = NULL );

   /**
    * Saves a group of reflection objects in one file. Use this method to serialize
    * standalone objects that DON'T depend on any Resource instances.
    *
    * CAUTION: an assertion will be thrown if any of the objects depends on Resource instances.
    *
    * @param objects
    * @param savePath
    * @param progressObserver ( optional )
    */
   static void saveObjects( std::vector< const ReflectionObject* >& objects, const FilePath& savePath, IProgressObserver* progressObserver = NULL );

   /**
    * A counterpart of the 'saveObject' method that saves a single object. Loads a single object
    * from the specified file.
    *
    * CAUTION: an assertion will be thrown if it turns out that the file was serialized sing 'saveObjects'
    * method or if the objects have any dependencies on the resources.
    *
    * @param loadPath
    * @param progressObserver ( optional )
    * @return  a pointer to the loaded object
    */
   static ReflectionObject* loadObject( const FilePath& loadPath, IProgressObserver* progressObserver = NULL );

   /**
    * A counterpart of the 'saveObjects' method that saves a single object. Loads all objects that were
    * serialized into the specified file. Can be also used for the files created using the 'saveObject' method.
    *
    * CAUTION: an assertion will be thrown if any of the objects has any dependencies on any resources.
    *
    * @param loadPath
    * @param outObjects 
    * @param progressObserver ( optional )
    */
   static void loadObjects( const FilePath& loadPath, std::vector< ReflectionObject* >& outObjects, IProgressObserver* progressObserver = NULL );

   // -------------------------------------------------------------------------
   // Tools
   // -------------------------------------------------------------------------

   /**
    * Collects paths to resources the specified object references ( hierarchically - this means
    * that it's gonna map the resources the immediately referenced resources reference and so on... )
    *
    * @param objectToMap
    * @param outDependenciesPaths
    */
   static void collectExternalDependencies( const ReflectionObject* objectToMap, std::vector< FilePath >& outDependenciesPaths ); 
};

///////////////////////////////////////////////////////////////////////////////
