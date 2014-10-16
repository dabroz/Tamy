#include "core.h"
#include "core/ResourceDependenciesMapper.h"
#include "core/ReflectionType.h"
#include "core/ReflectionTypeComponent.h"
#include "core/ReflectionObject.h"
#include "core/Resource.h"


///////////////////////////////////////////////////////////////////////////////

ResourceDepenenciesMapper::ResourceDepenenciesMapper( std::vector< FilePath >& outExternalDependencies )
   : m_outExternalDependencies( outExternalDependencies )
{
}

///////////////////////////////////////////////////////////////////////////////

void ResourceDepenenciesMapper::mapDependencies( const ReflectionObject* objectToProcess )
{
   if ( !objectToProcess )
   {
      return;
   }

   
   m_objectsToCheck.push_back( objectToProcess );

   // keep checking the size with each iteration - the array is going to grow
   for( uint i = 0; i < m_objectsToCheck.size(); ++i )
   {
      const ReflectionObject* object = m_objectsToCheck[i];

      // analyze the object in terms of being a dependency
      analyzeDependency( object );

      // now analyze its 'internals'
      analyzeObjectInternals( object );
   }
}

///////////////////////////////////////////////////////////////////////////////

void ResourceDepenenciesMapper::analyzeDependency( const ReflectionObject* object )
{
   if ( object->isA< Resource >() )
   {
      const Resource* resource = static_cast< const Resource* >( object );
      const FilePath& resourcePath = resource->getFilePath();

      // verify that the path's not on the list already
      uint count = m_outExternalDependencies.size();
      for ( uint i = 0; i < count; ++i )
      {
         if ( m_outExternalDependencies[i] == resourcePath )
         {
            // it's already on the list
            return;
         }
      }

      // it's not on the list - add it
      m_outExternalDependencies.push_back( resourcePath );
   }
}

///////////////////////////////////////////////////////////////////////////////

void ResourceDepenenciesMapper::analyzeObjectInternals( const ReflectionObject* object )
{
   const SerializableReflectionType& objType = object->getVirtualRTTI();

   // create a list of classes in the inheritance hierarchy and start restoring their dependencies
   std::list< const SerializableReflectionType* > reflectionTypesList;
   objType.mapTypesHierarchy( reflectionTypesList );

   // restore the dependencies on particular subtypes
   Array< byte > tempSerializationDataBuf;
   while( !reflectionTypesList.empty() )
   {
      const SerializableReflectionType* nextType = reflectionTypesList.front();
      reflectionTypesList.pop_front();

      unsigned int membersCount = nextType->m_memberFields.size();
      for ( uint i = 0; i < membersCount; ++i )
      {
         ReflectionTypeComponent* member = nextType->m_memberFields[i];
         member->mapDependencies( object, *this );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void ResourceDepenenciesMapper::addDependency( const ReflectionObject* dependency )
{
   if ( !dependency )
   {
      return;
   }

   // check if this dependency was already discovered
   uint count = m_objectsToCheck.size();
   for ( uint i = 0; i < count; ++i )
   {
      if ( m_objectsToCheck[i] == dependency )
      {
         // yup - it's already been encountered
         return;
      }
   }

   m_objectsToCheck.push_back( dependency );
}

///////////////////////////////////////////////////////////////////////////////
