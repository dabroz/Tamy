/// @file   core/PatchRecord.h
/// @brief  a single patch record that contains information describing how to patch one version of a given type to another
#ifndef _PATCH_RECORD_H
#define _PATCH_RECORD_H

#include <string>
#include <list>
#include "core\types.h"
#include "core\PatchDataMigrationFunction.h"
#include "core\MemoryRouter.h"


///////////////////////////////////////////////////////////////////////////////

class SerializableReflectionType;
class ReflectionTypeComponent;
enum SerializedFieldType;

///////////////////////////////////////////////////////////////////////////////

/**
 * A single patch record that contains information describing how to patch one version of a given type to another.
 */
class PatchRecord
{
   DECLARE_ALLOCATOR( PatchRecord, AM_DEFAULT );

private:

   // -------------------------------------------------------------------------

   struct FieldCreator
   {
      DECLARE_ALLOCATOR( FieldCreator, AM_DEFAULT );

      uint                          m_fieldId;
      SerializedFieldType           m_fieldType;

      FieldCreator( uint fieldId, SerializedFieldType fieldType ) 
         : m_fieldId( fieldId )
         , m_fieldType( fieldType )
      {}
   };

   // -------------------------------------------------------------------------

   struct FieldChanger
   {
      DECLARE_ALLOCATOR( FieldChanger, AM_DEFAULT );

      uint           m_oldFieldId;
      uint           m_newFieldId;

      FieldChanger( uint oldFieldId, uint newFieldId )
         : m_oldFieldId( oldFieldId )
         , m_newFieldId( newFieldId )
      {}
   };

   // -------------------------------------------------------------------------

   struct ParentTypeDesc
   {
      DECLARE_ALLOCATOR( ParentTypeDesc, AM_DEFAULT );

      uint     m_id;
      int      m_version;

      ParentTypeDesc( uint id, int version ) 
         : m_id( id )
         , m_version( version ) 
      {}
   };

public:
   PatchRecord*                  m_nextRecord;

   uint                          m_oldTypeId;
   int                           m_oldVersionNo;
   uint                          m_newTypeId;
   int                           m_newVersionNo;

   std::string                   m_newTypeName;

private:

   std::list< uint >             m_fieldsToRemove;
   std::list< FieldCreator >     m_fieldsToAdd;
   std::list< FieldChanger >     m_fieldsToChange;

   std::list< uint >             m_parentsToRemove;
   std::list< ParentTypeDesc >   m_parentsToAdd;

   PatchDataMigrationFunc        m_dataMigrationFunc;

public:
   /**
    * Constructor.
    *
    * @param oldTypeId        old type id ( generated using ReflectionType::generateId )
    * @param oldVersionNo
    * @param newTypeName      new type name
    * @param newVersionNo
    */
   PatchRecord( uint oldTypeId, int oldVersionNo, const std::string& newTypeName, int newVersionNo );
   ~PatchRecord();

   // -------------------------------------------------------------------------
   // Comparison operators
   // -------------------------------------------------------------------------
   bool operator==( const PatchRecord& rhs ) const;
   bool operator!=( const PatchRecord& rhs ) const;

   // -------------------------------------------------------------------------
   // Types builder
   // -------------------------------------------------------------------------
   /**
    * Migrates data according to the patch description.
    *
    * @param object
    */
   void migrateData( SerializedReflectionObject& object ) const;

   // -------------------------------------------------------------------------
   // Patch description
   // -------------------------------------------------------------------------

   /**
    * Defines a parent of the type.
    *
    * @param parentTypeName
    * @param versionNo
    */
   void addParent( const std::string& parentTypeName, int versionNo );

   /**
    * Defines a parent of the type.
    *
    * @param typeId
    * @param versionNo
    */
   void addParent( uint typeId, int versionNo );

   /**
    * Disowns this type from the specified parent.
    *
    * @param parentTypeName
    */
   void removeParent( const std::string& parentTypeName );

   /**
    * Adds a new field.
    *
    * @param fieldName
    * @param fieldType
    */
   void addField( const std::string& fieldName, SerializedFieldType fieldType );

   /**
    * Removes a member field.
    *
    * @param fieldName
    */
   void removeField( const std::string& fieldName );

   /**
    * Changes the name and type of the field.
    *
    * @param oldFieldName
    * @param newFieldName
    */
   void changeField( const std::string& oldFieldName, const std::string& newFieldName );

   /**
    * Defines a migration function that should be used when this patch is applied.
    *
    * @param func
    */
   void defineMigrationFunc( PatchDataMigrationFunc func );
};


///////////////////////////////////////////////////////////////////////////////

#include "core\PatchRecord.inl"

///////////////////////////////////////////////////////////////////////////////

#endif // _PATCH_RECORD_H
