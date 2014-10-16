#include "core.h"
#include "core\PatchesDB.h"
#include "core\ReflectionType.h"
#include "core\PatchRecord.h"
#include "core\SerializedReflectionObject.h"


///////////////////////////////////////////////////////////////////////////////

INIT_SINGLETON( PatchesDB );

///////////////////////////////////////////////////////////////////////////////

PatchesDB::PatchesDB( const SingletonConstruct& )
{
}

///////////////////////////////////////////////////////////////////////////////

PatchesDB::~PatchesDB()
{
   clear();
}

///////////////////////////////////////////////////////////////////////////////

void PatchesDB::clear()
{
   uint count = m_patchRecords.size();
   for ( uint i = 0; i < count; ++i )
   {
      delete m_patchRecords[i];
   }
   m_patchRecords.clear();
}

///////////////////////////////////////////////////////////////////////////////

PatchRecord& PatchesDB::addPatch( const std::string& oldTypeName, int oldVersionNo, const std::string& newTypeName, int newVersionNo )
{
   ASSERT_MSG( oldVersionNo < newVersionNo, "Old version number must be lower than the new version number" );

   uint oldTypeId = ReflectionType::generateId( oldTypeName );

   // find a good append point
   PatchRecord* appendPoint = NULL;
   uint count = m_patchRecords.size();
   for( uint i = 0; i < count; ++i )
   {
      PatchRecord* checkedRootRecord = m_patchRecords[i];
      PatchRecord* checkedRecord = checkedRootRecord;
      for( PatchRecord* checkedRecord = checkedRootRecord; checkedRecord != NULL && appendPoint == NULL; checkedRecord = checkedRecord->m_nextRecord )
      {
         // check if this is a good append point
         if ( checkedRecord->m_newTypeId == oldTypeId && checkedRecord->m_newVersionNo == oldVersionNo )
         {
            // verify that it's not a duplicate nor an invalid record that's supposed to replace a different patch record
            if ( checkedRecord->m_nextRecord != NULL )
            {
               ASSERT_MSG( checkedRecord->m_nextRecord == NULL, "Another patch record already exists in place of the one you're trying to append." );

               // delete previously appended record
               delete checkedRecord->m_nextRecord;
               checkedRecord->m_nextRecord = NULL;
            }
            
            // found it
            appendPoint = checkedRecord;
            break;
         }
      }
   }

   // this is a new record
   PatchRecord* newRecord = new PatchRecord( oldTypeId, oldVersionNo, newTypeName, newVersionNo );

   if ( appendPoint )
   {
      // append the record
      appendPoint->m_nextRecord = newRecord;
   }
   else
   {
      // this is a new root record
      m_patchRecords.push_back( newRecord );
   }

   return *newRecord;
}

///////////////////////////////////////////////////////////////////////////////

const PatchRecord* PatchesDB::findPatchRecord( uint typeId, int versionNo, bool findRoot ) const
{
   uint count = m_patchRecords.size();
   for( uint i = 0; i < count; ++i )
   {
      PatchRecord* checkedRootRecord = m_patchRecords[i];
      PatchRecord* checkedRecord = checkedRootRecord;
      for( PatchRecord* checkedRecord = checkedRootRecord; checkedRecord != NULL; checkedRecord = checkedRecord->m_nextRecord )
      {
         if ( checkedRecord->m_newTypeId == typeId && checkedRecord->m_newVersionNo == versionNo )
         {
            // found it
            if ( findRoot )
            {
               return checkedRootRecord;
            }
            else
            {
               return checkedRecord;
            }
         }
      }
   }

   return NULL;
}

///////////////////////////////////////////////////////////////////////////////

uint PatchesDB::findLatestType( uint typeId, int typeVersion ) const
{
   const PatchRecord* patchRecord = findPatchRecord( typeId, typeVersion, true );

   uint latestTypeId = 0;
   for ( ; patchRecord != NULL; patchRecord = patchRecord->m_nextRecord )
   {
      latestTypeId = patchRecord->m_newTypeId;
   }

   return latestTypeId;
}

///////////////////////////////////////////////////////////////////////////////

uint PatchesDB::migrateData( uint oldTypeId, int oldVersionNo, SerializedReflectionObject& object ) const
{
   // get type parents
   const PatchRecord* rootPatchRecord = findPatchRecord( oldTypeId, oldVersionNo, false );
   if ( rootPatchRecord == NULL )
   {
      // corresponding record type not found - skip it
      return 0;
   }

   uint latestTypeId = rootPatchRecord->m_newTypeId;
   const PatchRecord* record = rootPatchRecord->m_nextRecord;
   for ( ; record != NULL; record = record->m_nextRecord )
   {
      // next - migrate the data from the previous type version
      record->migrateData( object );

      // memorize the latest type id
   }

   return latestTypeId;
}

///////////////////////////////////////////////////////////////////////////////

void PatchesDB::addInitialPatch( const ReflectionTypesRegistry& registry, const SerializableReflectionType& type )
{
   // check if the type hasn't already been registered
   const PatchRecord* existingRecord = findPatchRecord( type.m_id, type.m_versionNo, true );
   if ( existingRecord )
   {
      // a patch for this type is already registered
      return;
   }

   // create a new record
   PatchRecord* newRecord = new PatchRecord( 0, -1, type.m_name, 0 );
   m_patchRecords.push_back( newRecord );

   // fill it in
   uint parentsCount = type.m_baseTypesIds.size();
   for ( uint i = 0; i < parentsCount; ++i )
   {
      // get the version of that type
      uint parentTypeId = type.m_baseTypesIds[i].m_id;
      uint parentTypeVer = type.m_baseTypesIds[i].m_version;
      newRecord->addParent( parentTypeId, parentTypeVer );
   }

   uint membersCount = type.m_memberFields.size();
   for ( uint i = 0; i < membersCount; ++i )
   {
      const ReflectionTypeComponent* memberField = type.m_memberFields[i];
      memberField->addToPatchRecord( *newRecord );
   }
}

///////////////////////////////////////////////////////////////////////////////

void PatchesDB::registerMigrationFunc( const std::string& funcName, PatchDataMigrationFunc func )
{
   DataMigrationFunctionsMap::const_iterator it = m_migrationFunctions.find( funcName );
   if ( it != m_migrationFunctions.end() )
   {
      // such a function is already registered
      ASSERT_MSG( it == m_migrationFunctions.end(), "Trying to redefine a registered data migration function" );
      return;
   }

   m_migrationFunctions.insert( std::make_pair( funcName, func ) );
}

///////////////////////////////////////////////////////////////////////////////

PatchDataMigrationFunc PatchesDB::findDataMigrationFunc( const std::string& funcName ) const
{
   DataMigrationFunctionsMap::const_iterator it = m_migrationFunctions.find( funcName );
   if ( it != m_migrationFunctions.end() )
   {
      return it->second;
   }
   else
   {
      return NULL;
   }
}

///////////////////////////////////////////////////////////////////////////////
