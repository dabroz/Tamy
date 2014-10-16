#include "core.h"
#include "core/ReflectionSaver.h"
#include "core/OutStream.h"
#include "core/ReflectionObject.h"
#include "core/Resource.h"
#include "core/ReflectionSerializationMacros.h"


///////////////////////////////////////////////////////////////////////////////

ReflectionSaver::ReflectionSaver( OutStream& outStream ) 
   : m_outStream( outStream )
{
}

///////////////////////////////////////////////////////////////////////////////

ReflectionSaver::~ReflectionSaver()
{
   flush();
}

///////////////////////////////////////////////////////////////////////////////

void ReflectionSaver::save( const ReflectionObject* object )
{
   // Check if the object isn't already stored somewhere in the dependencies map.
   // If it is, there's no point in serializing it again.
   uint count = m_dependencies.size();
   for ( uint i = 0; i < count; ++i )
   {
      if ( m_dependencies[i] == object )
      {
         // it was already serialized - just add its dependency index to the saved objects list
         m_serializedObjectsIndices.push_back( i );
         return;
      }
   }

   // Create a map of all pointers we're about to serialize.
   // In other words - map the dependencies the saved object has

   // ...and the very first dependency is our object itself - memorize its index so that we know
   // which object on the long list of serialized objects was the serialized one
   m_objectsToMap.clear();
   m_serializedObjectsIndices.push_back( m_dependencies.size() );
   addInternalDependency( object );

   // map its dependencies
   while( !m_objectsToMap.empty() )
   {
      const ReflectionObject* objectToMap = m_objectsToMap.back(); m_objectsToMap.pop_back();
      ASSERT( objectToMap != NULL );
      mapDependencies( objectToMap );
   }
}

///////////////////////////////////////////////////////////////////////////////

void ReflectionSaver::addDependency( const ReflectionObject* dependency )
{
   if ( !dependency )
   {
      return;
   }

   if ( dependency->isA< Resource >() )
   {
      addExternalDependency( dependency );
   }
   else
   {
      addInternalDependency( dependency );
   }
}

///////////////////////////////////////////////////////////////////////////////

void ReflectionSaver::addExternalDependency( const ReflectionObject* dependency )
{
   // check the uniqueness of the dependency
   uint count = m_externalDependencies.size();
   for ( uint i = 0; i < count; ++i )
   {
      if ( m_externalDependencies[i] == dependency )
      {
         // this external dependency is already known
         return;
      }
   }

   // don't save a resource, flag it as an external dependency instead
   const Resource* res = static_cast< const Resource* >( dependency );
   m_externalDependencies.push_back( res );
}

///////////////////////////////////////////////////////////////////////////////

void ReflectionSaver::addInternalDependency( const ReflectionObject* dependency )
{
   // check the uniqueness of the dependency
   uint count = m_dependencies.size();
   for ( uint i = 0; i < count; ++i )
   {
      if ( m_dependencies[i] == dependency )
      {
         // this dependency is already known
         return;
      }
   }

   // notify the object that it's about to be serialized
   const_cast< ReflectionObject* >( dependency )->onObjectPreSave();

   // this is a new dependency
   m_dependencies.push_back( dependency );
   m_objectsToMap.push_back( dependency );
}

///////////////////////////////////////////////////////////////////////////////

void ReflectionSaver::mapDependencies( const ReflectionObject* object )
{
   const SerializableReflectionType& objType = object->getVirtualRTTI();

   // create a list of classes in the inheritance hierarchy and start mapping their dependencies
   std::list< const SerializableReflectionType* > reflectionTypesList;
   objType.mapTypesHierarchy( reflectionTypesList );

   // map the dependencies on particular subtypes
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

void ReflectionSaver::flush()
{
   // 1. save the archive magic number
   m_outStream << TAMY_ARCHIVE_MAGIC_NO;

   // 2. serialize the external dependencies
   saveExternalDependencies( m_outStream );

   // 3. we need to serialize the dependencies map.
   saveInternalDependencies( m_outStream );

   // 4. now serialize the indices of the saved objects
   {
      uint savedObjectsCount = m_serializedObjectsIndices.size();
      m_outStream << savedObjectsCount;
      for ( uint i = 0; i < savedObjectsCount; ++i )
      {
         m_outStream << m_serializedObjectsIndices[i];
      }

      // clear the saved objects indices list
      m_serializedObjectsIndices.clear();
   }
}

///////////////////////////////////////////////////////////////////////////////

void ReflectionSaver::saveExternalDependencies( OutStream& outStream )
{
   // use a temporary stream, 'cause we need to write a skip size for this piece of data
   Array< byte > tmpDataBuf;
   OutArrayStream tmpDataStream( tmpDataBuf );

   // number of stored dependencies
   {
      const uint dependenciesCount = m_externalDependencies.size();
      tmpDataStream << dependenciesCount;

      // dependencies themselves
      for ( uint i = 0; i < dependenciesCount; ++i )
      {
         tmpDataStream << m_externalDependencies[i]->getFilePath();
      }
   }

   // store the size of the data
   const uint dataSize = tmpDataBuf.size();
   outStream << dataSize;

   // and then the data itself
   outStream.save( (byte*)tmpDataBuf, dataSize );
}

///////////////////////////////////////////////////////////////////////////////

void ReflectionSaver::saveInternalDependencies( OutStream& outStream )
{
   // number of stored dependencies
   const uint dependenciesCount = m_dependencies.size();
   outStream << dependenciesCount;

   // dependencies themselves
   for ( uint i = 0; i < dependenciesCount; ++i )
   {
      // store the unique id of the type so that we can look it up quickly afterwards
      outStream << m_dependencies[i]->m_uniqueId;

      // save the data to a temp buffer, 'cause we'll be needing a skip size of the data
      // so that if we manage to find this instance in our records during deserialization ( providing
      // that we keep track of course ), we can just skip reading it
      {
         Array< byte > tmpDataBuf;
         OutArrayStream tmpDataStream( tmpDataBuf );

         const SerializableReflectionType& depTypeInfo = m_dependencies[i]->getVirtualRTTI();
         depTypeInfo.save( *m_dependencies[i], *this, tmpDataStream );

         // store the size of the data
         const uint dataSize = tmpDataBuf.size();
         outStream << dataSize;

         // and then the data itself
         outStream.save( (byte*)tmpDataBuf, dataSize );
      }
   }

   // clear the dependencies list
   m_dependencies.clear();
}

///////////////////////////////////////////////////////////////////////////////

uint ReflectionSaver::findDependency( const ReflectionObject* dependency ) const
{
   if ( dependency->isA< Resource >() )
   {
      const Resource* resDep = static_cast< const Resource* >( dependency );

      // it's an external dependency
      uint dependenciesCount = m_externalDependencies.size();
      for ( uint i = 0; i < dependenciesCount; ++i )
      {
         if ( m_externalDependencies[i] == resDep )
         {
            // found it - mark that's an external dependency by setting the 
            return CREATE_EXTERNAL_DEPENDENCY_INDEX( i );
         }
      }
   }
   else
   {
      // it's an internal dependency
      uint dependenciesCount = m_dependencies.size();
      for ( uint i = 0; i < dependenciesCount; ++i )
      {
         if ( m_dependencies[i] == dependency )
         {
            // found it
            return CREATE_INTERNAL_DEPENDENCY_INDEX( i );
         }
      }
   }

   // we didn't map such a dependency - how did it got queried for then?
   ASSERT_MSG( false, "Unmapped dependency queried - check the dependency mapping related code" );
   return 0;
}

///////////////////////////////////////////////////////////////////////////////

void ReflectionSaver::collectExternalDependencies( std::vector< const Resource* >& outDependencies ) const
{
   uint count = m_externalDependencies.size();
   for ( uint i = 0; i < count; ++i )
   {
      const Resource* newDependency = m_externalDependencies[i];
      bool isUnique = true;
      uint currResourcesCount = outDependencies.size();
      for ( uint j = 0; j < currResourcesCount; ++j )
      {
         if ( outDependencies[j] == newDependency )
         {
            isUnique = false;
            break;
         }
      }

      if ( isUnique )
      {
         outDependencies.push_back( newDependency );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////
