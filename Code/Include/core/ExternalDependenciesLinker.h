/// @file   core/ExternalDependenciesLinker.h
/// @brief  external dependencies linker
#pragma once

#include "core\Delegate.h"
#include "core\MemoryRouter.h"
#include "core\ReflectionDependenciesCallback.h"
#include <vector>


///////////////////////////////////////////////////////////////////////////////

class Resource;
class FilePath;

///////////////////////////////////////////////////////////////////////////////

typedef Delegate< Resource* ( const FilePath& ) > FindResourceDelegate;

///////////////////////////////////////////////////////////////////////////////

/**
 * A tool that links the external dependencies between the deserialized objects.
 */
class ExternalDependenciesLinker : public ReflectionDependencyLinkerCallback
{
   DECLARE_ALLOCATOR( ExternalDependenciesLinker, AM_DEFAULT );

private:
   const std::vector< FilePath >&         m_externalDependencies;
   FindResourceDelegate                   m_findResource;

public:
   /**
    * Constructor.
    *
    * @param externalDependencies
    * @param resourcesStorage
    */
   ExternalDependenciesLinker( const std::vector< FilePath >& externalDependencies, FindResourceDelegate findResource );

   /**
    * Fills the pointers to Resource objects that weren't loaded during the standard loading process.
    *
    * In order to do that, it queries the active instance of ResourcesManager for the resources from the 'externalDependencies' array
    * whenever it finds that one of the objects from the array of 'objectsToProcess' references one.
    *
    * @param objectsToProcess
    */
   void linkDependencies( const std::vector< ReflectionObject* >& objectsToProcess );

   // -------------------------------------------------------------------------
   // ReflectionDependenciesCallback implementation
   // -------------------------------------------------------------------------
   ReflectionObject* findDependency( uint dependencyIdx, bool& outWasFound ) const;
};

///////////////////////////////////////////////////////////////////////////////