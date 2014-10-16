#include "core.h"
#include "core/ReflectionTypesRegistry.h"
#include "core/ReflectionType.h"
#include "core/ReflectionEnum.h"
#include "core/types.h"
#include "core/PatchesDB.h"


///////////////////////////////////////////////////////////////////////////////

INIT_SINGLETON( ReflectionTypesRegistry );

///////////////////////////////////////////////////////////////////////////////

ReflectionTypesRegistry::ReflectionTypesRegistry( const SingletonConstruct& )
   : m_genericEnumType( NULL )
{
   m_genericEnumType = new ReflectionEnum( "ReflectionEnum" );
}

///////////////////////////////////////////////////////////////////////////////

ReflectionTypesRegistry::~ReflectionTypesRegistry()
{
   clear();

   delete m_genericEnumType;
   m_genericEnumType = NULL;
}

///////////////////////////////////////////////////////////////////////////////

ReflectionType* ReflectionTypesRegistry::find( unsigned int id ) const
{
   // browse through external types first
   BaseTypesMap::const_iterator externIt = m_externalTypesMap.find( id );
   if ( externIt != m_externalTypesMap.end() )
   {
      return externIt->second;
   }
   
   // not an external type - browse through the serializable types

   SerializableTypesMap::const_iterator it = m_serializableTypesMap.find( id );
   if ( it != m_serializableTypesMap.end() )
   {
      return it->second;
   }
   
   // unknown type
   return NULL;
}

///////////////////////////////////////////////////////////////////////////////

ReflectionType* ReflectionTypesRegistry::find( const std::string& typeName ) const
{
   return find( ReflectionType::generateId( typeName ) );
}

///////////////////////////////////////////////////////////////////////////////

SerializableReflectionType* ReflectionTypesRegistry::findSerializable( unsigned int id ) const
{
   SerializableTypesMap::const_iterator it = m_serializableTypesMap.find( id );
   if ( it != m_serializableTypesMap.end() )
   {
      return it->second;
   }
   else
   {
      return NULL;
   }
}

///////////////////////////////////////////////////////////////////////////////

SerializableReflectionType* ReflectionTypesRegistry::findSerializable( const std::string& typeName ) const
{
   return findSerializable( ReflectionType::generateId( typeName ) );
}

///////////////////////////////////////////////////////////////////////////////

void ReflectionTypesRegistry::getMatchingSerializableTypes( uint id, std::vector< const SerializableReflectionType* >& outTypes, bool includeAbstractTypes ) const
{
   const SerializableReflectionType* baseType = findSerializable( id );

   if ( baseType == NULL )
   {
      return;
   }

   for ( SerializableTypesMap::const_iterator it = m_serializableTypesMap.begin(); it != m_serializableTypesMap.end(); ++it )
   {
      if ( !includeAbstractTypes && it->second->isAbstract() )
      {
         // we currently are not looking for the abstract relatives of this type
         continue;
      }

      if ( it->second->isA( *baseType ) )
      {
         outTypes.push_back( it->second );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void ReflectionTypesRegistry::clear()
{
   uint count = m_allTypes.size();
   for ( uint i = 0; i < count; ++i )
   {
      delete m_allTypes[i];
   }
   m_allTypes.clear();
   m_externalTypesMap.clear();
   m_serializableTypesMap.clear();
}

///////////////////////////////////////////////////////////////////////////////

void ReflectionTypesRegistry::build( PatchesDB& patchesDB )
{
   resolveTypeVersions();

   createPatchInfo( patchesDB );
}

///////////////////////////////////////////////////////////////////////////////

void ReflectionTypesRegistry::resolveTypeVersions()
{
   for ( SerializableTypesMap::iterator typeIt = m_serializableTypesMap.begin(); typeIt != m_serializableTypesMap.end(); ++typeIt )
   {
      SerializableReflectionType* analyzedType = typeIt->second;
      
      uint parentsCount = analyzedType->m_baseTypesIds.size();
      for ( uint i = 0; i < parentsCount; ++i )
      {
         if ( analyzedType->m_baseTypesIds[i].m_version < 0 )
         {
            SerializableReflectionType* resolvedParentType = findSerializable( analyzedType->m_baseTypesIds[i].m_id );
            if ( resolvedParentType != NULL )
            {
               analyzedType->m_baseTypesIds[i].m_version = resolvedParentType->m_versionNo;
            }
         }
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void ReflectionTypesRegistry::createPatchInfo( PatchesDB& patchesDB ) const
{
   for ( SerializableTypesMap::const_iterator it = m_serializableTypesMap.begin(); it != m_serializableTypesMap.end(); ++it )
   {
      const SerializableReflectionType* type = it->second;
      patchesDB.addInitialPatch( *this, *type );
   }
}

///////////////////////////////////////////////////////////////////////////////