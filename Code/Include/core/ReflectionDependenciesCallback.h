/// @file   core/ReflectionDependenciesCallback.h
/// @brief  two interfaces using during 2-way dependency mapping operations
#pragma once

#include "core/types.h"


///////////////////////////////////////////////////////////////////////////////

class ReflectionObject;

///////////////////////////////////////////////////////////////////////////////

/**
 * An interface for a reflection dependency mapping query callback.
 */
class ReflectionDependencyMapperCallback
{
public:
   /**
    * Adds a dependency.
    *
    * @param   dependency
    */
   virtual void addDependency( const ReflectionObject* dependency ) = 0;

   /**
    * Returns an index of the specified dependency.
    *
    * @param   reference to a dependency
    * @return  index of the dependency
    */
   virtual uint findDependency( const ReflectionObject* dependency ) const = 0;
};

///////////////////////////////////////////////////////////////////////////////

/**
 * An interface for a reflection dependency link query callback.
 */
class ReflectionDependencyLinkerCallback
{
public:
   virtual ~ReflectionDependencyLinkerCallback() {}

   /**
    * Returns a pointer to a dependency with the specified index.
    *
    * @param   dependencyIdx
    * @param   outWasFound       tells if the dependency was successfully resolved or not
    * @return  dependency
    */
   virtual ReflectionObject* findDependency( uint dependencyIdx, bool& outWasFound ) const = 0;
};

///////////////////////////////////////////////////////////////////////////////
