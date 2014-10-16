#include "core.h"
#include "core\PatchRecord.h"
#include "core\ReflectionType.h"
#include "core\ReflectionTypeComponent.h"
#include "core\Assert.h"
#include <map>


///////////////////////////////////////////////////////////////////////////////

PatchRecord::PatchRecord( uint oldTypeId, int oldVersionNo, const std::string& newTypeName, int newVersionNo )
   : m_nextRecord( NULL )
   , m_oldTypeId( oldTypeId )
   , m_oldVersionNo( oldVersionNo )
   , m_newTypeId( ReflectionType::generateId( newTypeName ) )
   , m_newVersionNo( newVersionNo )
   , m_newTypeName( newTypeName )
   , m_dataMigrationFunc( NULL )
{
}

///////////////////////////////////////////////////////////////////////////////

PatchRecord::~PatchRecord()
{
   delete m_nextRecord;
   m_nextRecord = NULL;

   m_dataMigrationFunc = NULL;
}

///////////////////////////////////////////////////////////////////////////////

bool PatchRecord::operator==( const PatchRecord& rhs ) const
{
   return ( m_oldTypeId == rhs.m_oldTypeId ) && ( m_oldVersionNo == rhs.m_oldVersionNo ) && ( m_oldTypeId == rhs. m_oldTypeId ) && ( m_newVersionNo == rhs.m_newVersionNo );
}

///////////////////////////////////////////////////////////////////////////////

bool PatchRecord::operator!=( const PatchRecord& rhs ) const
{
   return ( m_oldTypeId != rhs.m_oldTypeId ) || ( m_oldVersionNo != rhs.m_oldVersionNo ) || ( m_oldTypeId != rhs. m_oldTypeId ) || ( m_newVersionNo != rhs.m_newVersionNo );

}

///////////////////////////////////////////////////////////////////////////////

void PatchRecord::migrateData( SerializedReflectionObject& object ) const
{
   // change existing fields
   for ( std::list< FieldChanger >::const_iterator it = m_fieldsToChange.begin(); it != m_fieldsToChange.end(); ++it )
   {
      const FieldChanger& changer = *it;
      object.changeFieldId( changer.m_oldFieldId, changer.m_newFieldId );
   }

   // add new fields
   for ( std::list< FieldCreator >::const_iterator it = m_fieldsToAdd.begin(); it != m_fieldsToAdd.end(); ++it )
   {
      const FieldCreator& creator = *it;
      object.addField( creator.m_fieldId, creator.m_fieldType );
   }

   // manually migrate data
   if ( m_dataMigrationFunc != NULL )
   {
      m_dataMigrationFunc( object );
   }

   // remove obsolete fields
   for ( std::list< uint >::const_iterator it = m_fieldsToRemove.begin(); it != m_fieldsToRemove.end(); ++it )
   {
      uint fieldId = *it;
      object.removeField( fieldId );
   }
}

///////////////////////////////////////////////////////////////////////////////

void PatchRecord::addParent( const std::string& parentTypeName, int versionNo )
{
   uint typeId = ReflectionType::generateId( parentTypeName );
   addParent( typeId, versionNo );
}

///////////////////////////////////////////////////////////////////////////////

void PatchRecord::addParent( uint typeId, int versionNo )
{
   // when comparing types, we'll be checking only the type id for one, simple reason - you can't have two
   // parents of the same type, even if they have different versions

   // check if we didn't previously declare that we want to remove this parent,
   // in which case simply remove it from the other list and do nothing
   for ( std::list< uint >::const_iterator it = m_parentsToRemove.begin(); it != m_parentsToRemove.end(); ++it )
   {
      if ( *it == typeId )
      {
         m_parentsToRemove.erase( it );
         return;
      }
   }

   // check if this isn't a duplicate
   for ( std::list< ParentTypeDesc >::const_iterator it = m_parentsToAdd.begin(); it != m_parentsToAdd.end(); ++it )
   {
      if ( it->m_id == typeId )
      {
         return;
      }
   }

   // this is a new definition
   m_parentsToAdd.push_back( ParentTypeDesc( typeId, versionNo ) );
}

///////////////////////////////////////////////////////////////////////////////

void PatchRecord::removeParent( const std::string& parentTypeName )
{
   uint typeId = ReflectionType::generateId( parentTypeName );

   // check if we didn't previously declare that we want to add this parent,
   // in which case simply remove it from the other list and do nothing
   for ( std::list< ParentTypeDesc >::const_iterator it = m_parentsToAdd.begin(); it != m_parentsToAdd.end(); ++it )
   {
      if ( it->m_id == typeId )
      {
         m_parentsToAdd.erase( it );
         return;
      }
   }

   // check if this isn't a duplicate
   for ( std::list< uint >::const_iterator it = m_parentsToRemove.begin(); it != m_parentsToRemove.end(); ++it )
   {
      if ( *it == typeId )
      {
         return;
      }
   }

   // this is a new definition
   m_parentsToRemove.push_back( typeId );
}

///////////////////////////////////////////////////////////////////////////////

void PatchRecord::addField( const std::string& fieldName, SerializedFieldType fieldType )
{
   uint fieldId = ReflectionType::generateId( fieldName );

   // check if we didn't previously declare that we want to remove this field,
   // in which case simply remove it from the other list and do nothing
   for ( std::list< uint >::const_iterator it = m_fieldsToRemove.begin(); it != m_fieldsToRemove.end(); ++it )
   {
      if ( *it == fieldId )
      {
         m_fieldsToRemove.erase( it );
         return;
      }
   }

   // check if this isn't a duplicate
   for ( std::list< FieldCreator >::const_iterator it = m_fieldsToAdd.begin(); it != m_fieldsToAdd.end(); ++it )
   {
      if ( it->m_fieldId == fieldId )
      {
         return;
      }
   }

   // this is a new definition
   m_fieldsToAdd.push_back( FieldCreator( fieldId, fieldType ) );
}

///////////////////////////////////////////////////////////////////////////////

void PatchRecord::removeField( const std::string& fieldName )
{
   uint fieldId = ReflectionType::generateId( fieldName );

   // check if we didn't previously declare that we want to add this field,
   // in which case simply remove it from the other list and do nothing
   for ( std::list< FieldCreator >::const_iterator it = m_fieldsToAdd.begin(); it != m_fieldsToAdd.end(); ++it )
   {
      if ( it->m_fieldId == fieldId )
      {
         m_fieldsToAdd.erase( it );
         return;
      }
   }

   // check if this isn't a duplicate
   for ( std::list< uint >::const_iterator it = m_fieldsToRemove.begin(); it != m_fieldsToRemove.end(); ++it )
   {
      if ( *it == fieldId )
      {
         return;
      }
   }

   // this is a new definition
   m_fieldsToRemove.push_back( fieldId );
}

///////////////////////////////////////////////////////////////////////////////

void PatchRecord::changeField( const std::string& oldFieldName, const std::string& newFieldName )
{
   uint oldFieldId = ReflectionType::generateId( oldFieldName );
   for ( std::list< FieldChanger >::const_iterator it = m_fieldsToChange.begin(); it != m_fieldsToChange.end(); ++it )
   {
      const FieldChanger& changer = *it;
      if ( changer.m_oldFieldId == oldFieldId )
      {
         // an entry that migrates this field is already defined
         return;
      }
   }

   uint newFieldId = ReflectionType::generateId( newFieldName );
   m_fieldsToChange.push_back( FieldChanger( oldFieldId, newFieldId ) );
}

///////////////////////////////////////////////////////////////////////////////

void PatchRecord::defineMigrationFunc( PatchDataMigrationFunc func )
{
   m_dataMigrationFunc = func;
}

///////////////////////////////////////////////////////////////////////////////
