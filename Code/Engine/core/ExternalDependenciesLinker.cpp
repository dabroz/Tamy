#include "core\ExternalDependenciesLinker.h"
#include "core\ReflectionSerializationMacros.h"
#include "core\ReflectionType.h"
#include "core\ReflectionObject.h"
#include "core\Resource.h"
#include <list>


///////////////////////////////////////////////////////////////////////////////

ExternalDependenciesLinker::ExternalDependenciesLinker( const std::vector< FilePath >& externalDependencies, FindResourceDelegate findResource )
   : m_externalDependencies( externalDependencies )
   , m_findResource( findResource )
{
}

///////////////////////////////////////////////////////////////////////////////

void ExternalDependenciesLinker::linkDependencies( const std::vector< ReflectionObject* >& objectsToProcess )
{
   std::list< const SerializableReflectionType* > reflectionTypesList;

   uint count = objectsToProcess.size();
   for ( uint i = 0; i < count; ++i )
   {
      ReflectionObject* object = objectsToProcess[i];
      const SerializableReflectionType& objType = object->getVirtualRTTI();

      // create a list of classes in the inheritance hierarchy and start restoring their dependencies
      reflectionTypesList.clear();
      objType.mapTypesHierarchy( reflectionTypesList );

      // restore the dependencies on particular subtypes
      Array< byte > tempSerializationDataBuf;
      while( !reflectionTypesList.empty() )
      {
         const SerializableReflectionType* nextType = reflectionTypesList.front();
         reflectionTypesList.pop_front();

         unsigned int membersCount = nextType->m_memberFields.size();
         for ( uint j = 0; j < membersCount; ++j )
         {
            ReflectionTypeComponent* member = nextType->m_memberFields[j];
            member->restoreDependencies( object, *this );
         }
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

ReflectionObject* ExternalDependenciesLinker::findDependency( uint dependencyIdx, bool& outWasFound ) const
{
   outWasFound = false;
   if ( IS_DEPENDENCY_NULL( dependencyIdx ) )
   {
      // this is a NULL reference
      return NULL;
   }
   else
   {
      bool isExternal = false;
      uint extractedDependencyIdx;    
      EXTRACT_DEPENDENCY_INDEX( dependencyIdx, extractedDependencyIdx, isExternal );

      // external dependencies mapping pass
      if ( isExternal )
      {
         // reach to the active ResourcesManager instance and query the resource indicated by the dependency
         const FilePath& resourcePath = m_externalDependencies[ extractedDependencyIdx ];

         Resource* res = m_findResource( resourcePath );
         outWasFound = true;
         return res;
      }
      else
      {
         // we're mapping the external dependencies only - internal dependencies already have the correct pointer defined
         // so we need to return the argument as a pointer WITHOUT altering it
         return ( ReflectionObject* )dependencyIdx;
      }
   }
}

///////////////////////////////////////////////////////////////////////////////
