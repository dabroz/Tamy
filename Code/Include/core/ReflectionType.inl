#ifndef _REFLECTION_TYPE_H
#error "This file can only be included from ReflectionType.h"
#else

#include "core/ReflectionTypesRegistry.h"
#include "core/ReflectionTypeComponent.h"
#include "core/ReflectionSaver.h"
#include "core/ReflectionLoader.h"
#include "core/Array.h"
#include "core/OutArrayStream.h"
#include "core/StringUtils.h"
#include "core/DefaultAllocator.h"
#include "core/SerializedReflectionObject.h"
#include "core/ThreadSystem.h"
#include "core/PatchesDB.h"


///////////////////////////////////////////////////////////////////////////////

uint ReflectionType::generateId( const std::string& typeName )
{
   return StringUtils::calculateHash( typeName );
}

///////////////////////////////////////////////////////////////////////////////

bool ReflectionType::isExactlyA( const ReflectionType& referenceType ) const
{
   return m_id == referenceType.m_id;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

template< typename T >
T* SerializableReflectionType::instantiate() const
{
   if ( m_instantiator )
   {
      return reinterpret_cast< T* >( m_instantiator->instantiate() );
   }
   else
   {
      ASSERT_MSG( false, "You're trying to instantiate an abstract type" );
      return reinterpret_cast< T* >( NULL );
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
T* SerializableReflectionType::instantiate( const T& rhs ) const
{
   if ( m_instantiator )
   {
      return reinterpret_cast< T* >( m_instantiator->instantiate( &rhs ) );
   }
   else
   {
      ASSERT_MSG( false, "You're trying to copy an abstract type" );
      return reinterpret_cast< T* >( NULL );
   }
}

///////////////////////////////////////////////////////////////////////////////

bool SerializableReflectionType::isAbstract() const
{
   return m_instantiator == NULL;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void SerializableReflectionType::save( const T& object, const ReflectionSaver& dependenciesMapper, OutStream& stream ) const
{
   // save the type info - so that during deserialization we can 
   // instantiate a proper type instance and have it deserialize
   // its data
   stream << m_id;
   stream << m_versionNo;

   // serialize the unique id assigned to the object
   stream << object.m_uniqueId;

   // create a list of classes in the inheritance hierarchy and start serializing them one by one
   std::list< const SerializableReflectionType* > reflectionTypesList;
   mapTypesHierarchy( reflectionTypesList );

   // serialize the hierarchy of types
   // first - save the number of sub types
   stream << reflectionTypesList.size();

   // next - the types data itself
   while( !reflectionTypesList.empty() )
   {
      const SerializableReflectionType* nextType = reflectionTypesList.front();
      reflectionTypesList.pop_front();
      // once again serialize the type id, so that we can look it up during
      // deserialization and deserialize the proper data members
      stream << nextType->m_id;
      stream << nextType->m_versionNo;

      SerializedReflectionObject savedObject;
      nextType->saveMemberFields( &object, dependenciesMapper, savedObject );

      // ok - serialize the chunk size
      stream << savedObject;
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void SerializableReflectionType::saveMemberFields( const T* object, const ReflectionSaver& dependenciesMapper, SerializedReflectionObject& savedObject ) const
{
   unsigned int membersCount = m_memberFields.size();
   for ( uint i = 0; i < membersCount; ++i )
   {
      ReflectionTypeComponent* member = m_memberFields[i];
      member->save( object, dependenciesMapper, savedObject );
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
T* SerializableReflectionType::load( InStream& stream )
{
   PatchesDB& patchesDB = TSingleton< PatchesDB >::getInstance();
   ReflectionTypesRegistry& typesRegistry = TSingleton< ReflectionTypesRegistry >::getInstance();

   // deserialize and instantiate the serialized object
   T* object = NULL;
   {
      uint id = -1;
      uint versionNo = 0;
      ReflectionObject::UniqueId objectUniqueId; 
      stream >> id;
      stream >> versionNo;
      stream >> objectUniqueId;

      // find the corresponding type information
      uint latestTypeId = patchesDB.findLatestType( id, versionNo );
      SerializableReflectionType* deserializedType = typesRegistry.findSerializable( latestTypeId );
      if ( deserializedType )
      {
         // instantiate object of the deserialized type
         object = deserializedType->instantiate< T >();
         

         if ( object )
         {
            object->m_uniqueId = objectUniqueId;
         }
      }
   }

   if ( !object )
   {
      // this object can't be created
      return NULL;
   }


   // Now start deserializing its properties - whether we managed to deserialize the object or not.
   // we need to be able to read it.
   Array< byte > tempDataBuf;
   uint hierarchyTypesCount = 0;
   stream >> hierarchyTypesCount;
   for ( uint i = 0; i < hierarchyTypesCount; ++i )
   {
      // get the sub type id
      uint hierarchyTypeId = -1;
      uint hierarchyTypeVersionNo = 0;
      stream >> hierarchyTypeId;
      stream >> hierarchyTypeVersionNo;

      // load the object
      SerializedReflectionObject savedObject;
      stream >> savedObject;

      // migrate it to the latest version
      uint latestHierarchyTypeId = patchesDB.migrateData( hierarchyTypeId, hierarchyTypeVersionNo, savedObject );

      // initialize the created object with its contents
      SerializableReflectionType* deserializedHierarchyType = typesRegistry.findSerializable( latestHierarchyTypeId );
      if ( deserializedHierarchyType )
      {
         deserializedHierarchyType->loadMemberFields( object, savedObject );
      }
   }

   return object;
}

///////////////////////////////////////////////////////////////////////////////

void SerializableReflectionType::loadMemberFields( void* object, const SerializedReflectionObject& savedObject ) const
{
   unsigned int membersCount = m_memberFields.size();
   for ( uint i = 0; i < membersCount; ++i )
   {
      const ReflectionTypeComponent* member = m_memberFields[i];
      member->load( object, savedObject );
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename FieldType >
void SerializableReflectionType::getFieldValue( const std::string& fieldName, const void* object, FieldType& outValue  ) const
{
   ReflectionTypeComponent* field = findMemberField( fieldName );
   if ( !field )
   {
      return;
   }

   outValue = *reinterpret_cast< FieldType* >( ( (char*)object + field->m_dataOffset ) );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

template< typename T >
void* TSerializableTypeInstantiator< T >::instantiate() const
{
   return new T();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

template< typename T >
void* TSerializableTypeInstantiator< T >::instantiate( const void* pRhs ) const
{
   const T& rhs = *((const T*)pRhs);
   return new T( rhs );
}

///////////////////////////////////////////////////////////////////////////////

#endif // _REFLECTION_TYPE_H

