/// @file   core/PatchesDB.h
/// @brief  a database of information about the patches of the registered serializable types
#ifndef _PATCHES_DB_H
#define _PATCHES_DB_H

#include <string>
#include <vector>
#include <map>
#include "core\MemoryRouter.h"
#include "core\types.h"
#include "core\PatchDataMigrationFunction.h"
#include "core\Singleton.h"


///////////////////////////////////////////////////////////////////////////////

class PatchRecord;
class SerializedReflectionObject;
class SerializableReflectionType;
class ReflectionTypesRegistry;

///////////////////////////////////////////////////////////////////////////////

class PatchesDB
{
   PRIORITY_SINGLETON( 3 );
   DECLARE_ALLOCATOR( PatchesDB, AM_DEFAULT );

private:
   std::vector< PatchRecord* >               m_patchRecords;

   typedef std::map< std::string, PatchDataMigrationFunc >  DataMigrationFunctionsMap;
   DataMigrationFunctionsMap                 m_migrationFunctions;

public:
   /**
    * Singleton constructor.
    */
   PatchesDB( const SingletonConstruct& );
   ~PatchesDB();

   // -------------------------------------------------------------------------
   // Types builder
   // -------------------------------------------------------------------------
   /**
    * Takes some type signature, looks up its latest version and builds it.
    *
    * @param typeId
    * @param typeVersion
    *
    * @return  typeId      id of the latest version of the type, one that can be looked up in the types registry
    */
   uint findLatestType( uint typeId, int typeVersion ) const;

   /**
    * Migrates data between two versions of an object.
    * 
    * It migrates the part of an object dedicated to the specified type - that said, if the object
    * is polymorphic ( has multiple base classes etc ), this method will only migrate the specified
    * part of it, dedicated to the selected type.
    *
    * @param oldTypeId
    * @param oldVersionNo
    * @param object
    *
    * @return           id of the latest version of the type
    */
   uint migrateData( uint oldTypeId, int oldVersionNo, SerializedReflectionObject& object ) const;

   // -------------------------------------------------------------------------
   // Records management
   // -------------------------------------------------------------------------

   /**
    * Clears the contents of the database.
    */
   void clear();

   /**
    * Creates a new patch record that patches one version of a class to another.
    *
    * @param oldTypeName
    * @param oldVersionNo
    * @param newTypeName      ( can be the same as the old one, if the type name didn't change )
    * @param newVersionNo
    */
   PatchRecord& addPatch( const std::string& oldTypeName, int oldVersionNo, const std::string& newTypeName, int newVersionNo );

   /**
    * Adds an initial patch record based on the specified type. The type will be treated as the 0-th version.
    *
    * @param registry      registry the type is registered in
    * @param type
    */
   void addInitialPatch( const ReflectionTypesRegistry& registry, const SerializableReflectionType& type );

   /**
    * Locates a patch record corresponding to the specified version of the specified type.
    *
    * @param typeId
    * @param versionNo
    * @param findRoot      'true' - the method returns the root of the list that contains the specified version, 
    *                      'false' - returns the record corresponding to the version
    * @return              pointer to a patch record, if one exists
    */
   const PatchRecord* findPatchRecord( uint typeId, int versionNo, bool findRoot ) const;

   /**
    * Registers a pointer to a custom data migration function.
    *
    * @param funcName
    * @param func          data migration function pointer
    */
   void registerMigrationFunc( const std::string& funcName, PatchDataMigrationFunc func );

   /**
    * Returns a pointer to the registered data migration function.
    *
    * @param funcName
    * @return              data migration function pointer
    */
   PatchDataMigrationFunc findDataMigrationFunc( const std::string& funcName ) const;
};

///////////////////////////////////////////////////////////////////////////////

#endif // _PATCHES_DB_H
