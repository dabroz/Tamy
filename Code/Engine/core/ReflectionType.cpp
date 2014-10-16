#include "core.h"
#include "core\ReflectionType.h"
#include "core\ReflectionTypeComponent.h"
#include "core\Assert.h"
#include "core\List.h"
#include "core\TSContinuousMemoryPool.h"
#include "core\ThreadSystem.h"
#include "core\Thread.h"


///////////////////////////////////////////////////////////////////////////////

ReflectionType::ReflectionType( const std::string& name ) 
   : m_name( name )
   , m_versionNo( 0 )
{
   m_id = generateId( m_name );
}

///////////////////////////////////////////////////////////////////////////////

void ReflectionType::setTypeName( const std::string& name )
{
   m_name = name;
   m_id = generateId( m_name );
}

///////////////////////////////////////////////////////////////////////////////

bool ReflectionType::isA( const ReflectionType& referenceType ) const
{
   return isExactlyA( referenceType );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

SerializableReflectionType::SerializableReflectionType( const std::string& name ) 
   : ReflectionType( name )
   , m_instantiator( NULL )
   , m_searchMemPool( new TSContinuousMemoryPool( 256 ) )
{
}

///////////////////////////////////////////////////////////////////////////////

SerializableReflectionType::~SerializableReflectionType()
{
   delete m_instantiator; m_instantiator = NULL;

   unsigned int count = m_memberFields.size();
   for ( unsigned int i = 0; i < count; ++i )
   {
      delete m_memberFields[i];
   }
   m_memberFields.clear();

   delete m_searchMemPool;
   m_searchMemPool = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void SerializableReflectionType::setupInstantiator( SerializableTypeInstantiator* instantiator )
{
   delete m_instantiator;
   m_instantiator = instantiator;
}

///////////////////////////////////////////////////////////////////////////////

bool SerializableReflectionType::isA( const ReflectionType& referenceType ) const
{
   // we'll perform these operations in the current thread's constant memory pool, for performance reasons
   ThreadSystem& threadSystem = TSingleton< ThreadSystem >::getInstance();
   Thread& thread = threadSystem.getCurrentThread();

   // run a search on the type's inheritance tree
   ReflectionTypesRegistry& typesRegistry = TSingleton< ReflectionTypesRegistry >::getInstance();
   List< const SerializableReflectionType* > bfs( thread.m_sharedMemoryPool );
   bfs.pushBack( this );
   while ( !bfs.empty() )
   {
      const SerializableReflectionType* currType = bfs.front();
      bfs.popFront();

      if ( currType->m_id == referenceType.m_id )
      {
         return true;
      }

      // gather the base types
      uint childrenCount = currType->m_baseTypesIds.size();
      for ( uint i = 0; i < childrenCount; ++i )
      {
         const SerializableReflectionType* parentType = typesRegistry.findSerializable( currType->m_baseTypesIds[i].m_id );
         if ( parentType )
         {
            bfs.pushBack( parentType );
         }
      }
   }

   return false;
}

///////////////////////////////////////////////////////////////////////////////

void SerializableReflectionType::addBaseType( const std::string& baseTypeName, int versionNo )
{
   uint typeId = generateId( baseTypeName );
   addBaseType( typeId, versionNo );
}

///////////////////////////////////////////////////////////////////////////////

void SerializableReflectionType::addBaseType( uint baseTypeId, int versionNo )
{
   // make sure the added id is unique
   uint count = m_baseTypesIds.size();
   for ( uint i = 0; i < count; ++i )
   {
      if ( m_baseTypesIds[i].m_id == baseTypeId )
      {
         ASSERT_MSG( m_baseTypesIds[i].m_id != baseTypeId, "Trying to redefine a base type" );
         return;
      }
   }

   m_baseTypesIds.push_back( ParentTypeDesc( baseTypeId, versionNo ) );
}

///////////////////////////////////////////////////////////////////////////////

void SerializableReflectionType::removeBaseType( uint baseTypeId )
{
   // make sure the added id is unique
   uint count = m_baseTypesIds.size();
   for ( uint i = 0; i < count; ++i )
   {
      if ( m_baseTypesIds[i].m_id == baseTypeId )
      {
         m_baseTypesIds.erase( m_baseTypesIds.begin() + i );
         return;
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void SerializableReflectionType::addMemberField( ReflectionTypeComponent* member )
{
   if ( member )
   {
      m_memberFields.push_back( member );
   }
}

///////////////////////////////////////////////////////////////////////////////

void SerializableReflectionType::removeMemberField( uint memberId )
{
   uint count = m_memberFields.size();
   for ( uint i = 0; i < count; ++i )
   {
      ReflectionTypeComponent* member = m_memberFields[i];

      if ( member->m_id == memberId )
      {
         delete member;
         m_memberFields.erase( m_memberFields.begin() + i );
         return;
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

ReflectionTypeComponent* SerializableReflectionType::findMemberField( const std::string& memberName ) const
{
   uint memberId = ReflectionTypeComponent::generateId( memberName );
   ReflectionTypeComponent* foundComponent = findMemberField( memberId );
   return foundComponent;
}

///////////////////////////////////////////////////////////////////////////////

ReflectionTypeComponent* SerializableReflectionType::findMemberField( uint memberId ) const
{
   // find the member with a matching id
   uint count = m_memberFields.size();
   for ( uint i = 0; i < count; ++i )
   {
      if ( m_memberFields[i]->m_id == memberId )
      {
         // found it
         return m_memberFields[i];
      }
   }

   // a member with such id is not defined
   return NULL;
}

///////////////////////////////////////////////////////////////////////////////

void SerializableReflectionType::collectMemberFields( List< const ReflectionTypeComponent* >& outFields, byte traits ) const
{
   LocalList< const SerializableReflectionType* > types;
   types.pushBack( this );

   while( !types.empty() )
   {
      const SerializableReflectionType* analyzedType = types.front();
      types.popFront();

      // collect fields
      uint fieldsCount = analyzedType->m_memberFields.size();
      for ( uint i = 0; i < fieldsCount; ++i )
      {
         const ReflectionTypeComponent* field = analyzedType->m_memberFields[i];
         if ( ( field->m_traits & traits ) == traits )
         {
            // traits are a match - that's the field we're looking for
            outFields.pushBack( field );
         }
      }

      // collect parent types for further analysis
      analyzedType->collectParents( types );
   }
}

///////////////////////////////////////////////////////////////////////////////

void SerializableReflectionType::mapTypesHierarchy( std::list< const SerializableReflectionType* >& outReflectionTypesList ) const
{
   ReflectionTypesRegistry& typesRegistry = TSingleton< ReflectionTypesRegistry >::getInstance();
   std::vector< const SerializableReflectionType* > dfs;
   dfs.push_back( this );
   while( !dfs.empty() )
   {
      const SerializableReflectionType* nextType = dfs.back();
      dfs.pop_back();
      outReflectionTypesList.push_back( nextType );

      // go through the parent types
      uint parentTypesCount = nextType->m_baseTypesIds.size();
      for ( uint i = 0; i < parentTypesCount; ++i )
      {
         const SerializableReflectionType* parentType = typesRegistry.findSerializable( nextType->m_baseTypesIds[i].m_id );
         if ( parentType )
         {
            dfs.push_back( parentType );
         }
         else
         {
            // we have a base type listed that's not registered - report that
            ASSERT_MSG( parentType != NULL, "Parent type that's not registered is listed" );
         }
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void SerializableReflectionType::collectProperties( void* hostObj, std::vector< ReflectionProperty* >& outProperties ) const
{
   // get all types in the hierarchy
   std::list< const SerializableReflectionType* > reflectionTypesList;
   mapTypesHierarchy( reflectionTypesList );

   while( !reflectionTypesList.empty() )
   {
      // go through each one and query their members for properties
      const SerializableReflectionType* nextType = reflectionTypesList.front();
      reflectionTypesList.pop_front();

      int membersCount = nextType->m_memberFields.size();
      for ( int i = 0; i < membersCount; ++i )
      {
         const ReflectionTypeComponent& field = *( nextType->m_memberFields[i] );

         ReflectionProperty* property = field.createProperty( hostObj );

         // copy the field flags
         if ( property != NULL )
         {
            outProperties.push_back( property );
         }
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void SerializableReflectionType::collectParents( List< const SerializableReflectionType* >& outParentTypes ) const
{
   const ReflectionTypesRegistry& registry = TSingleton< ReflectionTypesRegistry >::getInstance();
   
   uint count = m_baseTypesIds.size();
   for ( uint i = 0; i < count; ++i )
   {
      const SerializableReflectionType* parent = registry.findSerializable( m_baseTypesIds[i].m_id );
      if ( parent != NULL )
      {
         outParentTypes.pushBack( parent );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void SerializableReflectionType::collectImplementations( std::vector< const SerializableReflectionType* >& outDerivedTypes, bool includeAbstractTypes ) const
{
   const ReflectionTypesRegistry& registry = TSingleton< ReflectionTypesRegistry >::getInstance();

   std::vector< const SerializableReflectionType* > types;
   registry.getMatchingSerializableTypes( m_id, types, includeAbstractTypes );

   // remove self from the list
   uint count = types.size();
   for ( uint i = 0; i < count; ++i )
   {
      if ( types[i]->m_id == m_id )
      {
         types.erase( types.begin() + i );
         break;
      }
   }

   outDerivedTypes.insert( outDerivedTypes.end(), types.begin(), types.end() );
}

///////////////////////////////////////////////////////////////////////////////

