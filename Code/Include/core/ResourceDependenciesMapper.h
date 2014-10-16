/// @file      core/ResourceDepenenciesMapper.h
/// @brief     a tool that maps the external dependencies of objects
#pragma once

#include "core/ReflectionDependenciesCallback.h"
#include "core/FilePath.h"
#include <vector>


///////////////////////////////////////////////////////////////////////////////

/**
 * A tool that maps the external dependencies of objects.
 */
class ResourceDepenenciesMapper : public ReflectionDependencyMapperCallback
{
private:
   std::vector< FilePath >&                  m_outExternalDependencies;

   // runtime data
   std::vector< const ReflectionObject* >    m_objectsToCheck;

public:
   /**
    * Constructor.
    *
    * @param externalDependencies
    */
   ResourceDepenenciesMapper( std::vector< FilePath >& outExternalDependencies );
   virtual ~ResourceDepenenciesMapper() {}

   /**
    * Finds all external dependencies of the specified object.
    *
    * @param objectToProcess
    */
   void mapDependencies( const ReflectionObject* objectToProcess );

   // -------------------------------------------------------------------------
   // ReflectionDependenciesCallback implementation
   // -------------------------------------------------------------------------
   void addDependency( const ReflectionObject* dependency );
   uint findDependency( const ReflectionObject* dependency ) const { return 0; }

private:
   void analyzeDependency( const ReflectionObject* object );
   void analyzeObjectInternals( const ReflectionObject* object );
};

///////////////////////////////////////////////////////////////////////////////
