/// @file   core/ResourcesSerializationUtil.h
/// @brief  utility for loading and saving resources
#pragma once

#include <vector>
#include <map>
#include "core/FilePath.h"


///////////////////////////////////////////////////////////////////////////////

class Filesystem;
class FilePath;
class ReflectionObject;
class Resource;
class IProgressObserver;

///////////////////////////////////////////////////////////////////////////////

/**
 * Utility for loading and saving resources.
 */
class ResourcesSerializationUtil
{
public:
   /**
    * Saves a resource into a dedicated file.
    *
    * @param filesystem
    * @param resource
    * @param differentSavePath (optional )
    */
   static void saveResource( Filesystem& filesystem, const Resource* resource, const FilePath& differentSavePath = FilePath() );

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
