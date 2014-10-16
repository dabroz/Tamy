#include "core\PatchesDBSerializer.h"
#include "core\PatchesDB.h"
#include "core\PatchRecord.h"
#include "core\SerializedReflectionObject.h"
#include "core\InStream.h"
#include "core\OutStream.h"
#include "tinyxml\tinyxml.h"


///////////////////////////////////////////////////////////////////////////////

void PatchesDBSerializer::save( const PatchesDB& patchesDB, OutStream& stream )
{
}

///////////////////////////////////////////////////////////////////////////////

void PatchesDBSerializer::load( PatchesDB& patchesDB, const std::string& dbXMLDefinition )
{
   TiXmlDocument* document = new TiXmlDocument( "PatchesDB" );
   bool result = document->LoadFile( (char*)dbXMLDefinition.c_str(), dbXMLDefinition.length(), TIXML_DEFAULT_ENCODING );
   if ( !result )
   {
      ASSERT_MSG( result, "Couldn't load PatchesDB contents" );
      delete document;
      return;
   }

   TiXmlNode* patchesDBNode = document->FirstChild( "PatchesDB" );
   if ( !patchesDBNode )
   {
      delete document;
      return;
   }

   // load all patch entries
   for ( TiXmlElement* patchEntryElem = patchesDBNode->FirstChildElement( "PatchEntry" ); patchEntryElem != NULL; patchEntryElem = patchEntryElem->NextSiblingElement( "PatchEntry" ) ) 
   {
      loadPatchEntry( patchesDB, patchEntryElem );
   }

   // cleanup
   delete document;
}

///////////////////////////////////////////////////////////////////////////////

void PatchesDBSerializer::loadPatchEntry( PatchesDB& patchesDB, TiXmlElement* patchEntryElem )
{
   std::string oldTypeName = patchEntryElem->Attribute( "oldTypeName" );
   std::string newTypeName = patchEntryElem->Attribute( "newTypeName" );

   int oldVersionNo;
   int newVersionNo;
   patchEntryElem->Attribute( "oldVersionNo", &oldVersionNo );
   patchEntryElem->Attribute( "newVersionNo", &newVersionNo );

   PatchRecord& record = patchesDB.addPatch( oldTypeName, oldVersionNo, newTypeName, newVersionNo );

   // add fields
   for ( TiXmlElement* addFieldElem = patchEntryElem->FirstChildElement( "AddField" ); addFieldElem != NULL; addFieldElem = addFieldElem->NextSiblingElement( "AddField" ) ) 
   {
      std::string fieldTypeStr = addFieldElem->Attribute( "type" );
      std::string fieldName = addFieldElem->Attribute( "name" );
      SerializedFieldType fieldType = translateFieldType( fieldTypeStr );

      record.addField( fieldName, fieldType );
   }

   // remove fields
   for ( TiXmlElement* removeFieldElem = patchEntryElem->FirstChildElement( "RemoveField" ); removeFieldElem != NULL; removeFieldElem = removeFieldElem->NextSiblingElement( "RemoveField" ) ) 
   {
      std::string fieldName = removeFieldElem->Attribute( "name" );
      record.removeField( fieldName );
   }

   // change fields
   for ( TiXmlElement* changeFieldElem = patchEntryElem->FirstChildElement( "ChangeField" ); changeFieldElem != NULL; changeFieldElem = changeFieldElem->NextSiblingElement( "ChangeField" ) ) 
   {
      std::string oldFieldName = changeFieldElem->Attribute( "oldName" );
      std::string newFieldName = changeFieldElem->Attribute( "newName" );

      record.changeField( oldFieldName, newFieldName );
   }

   // add parents
   for ( TiXmlElement* addParentElem = patchEntryElem->FirstChildElement( "AddParent" ); addParentElem != NULL; addParentElem = addParentElem->NextSiblingElement( "AddParent" ) ) 
   {
      std::string typeName = addParentElem->Attribute( "type" );
      int versionNo;
      addParentElem->Attribute( "version", &versionNo );

      record.addParent( typeName, versionNo );
   }

   // remove parents
   for ( TiXmlElement* removeParentElem = patchEntryElem->FirstChildElement( "RemoveParent" ); removeParentElem != NULL; removeParentElem = removeParentElem->NextSiblingElement( "RemoveParent" ) ) 
   {
      std::string typeName = removeParentElem->Attribute( "type" );
      record.removeParent( typeName );
   }

   // set a patching function
   TiXmlElement* dataMigrationFuncElem = patchEntryElem->FirstChildElement( "DataMigrationFunction" );
   if ( dataMigrationFuncElem )
   {
      std::string funcName = dataMigrationFuncElem->Attribute( "name" );
      PatchDataMigrationFunc func = patchesDB.findDataMigrationFunc( funcName );
      record.defineMigrationFunc( func );
   }
}

///////////////////////////////////////////////////////////////////////////////

SerializedFieldType PatchesDBSerializer::translateFieldType( const std::string& typeName )
{
   if ( typeName == "SFT_Value" )
   {
      return SFT_Value;
   }
   else if ( typeName == "SFT_Pointer" )
   {
      return SFT_Pointer;
   }
   else if ( typeName == "SFT_ArrayOfValues" )
   {
      return SFT_ArrayOfValues;
   }
   else if ( typeName == "SFT_ArrayOfPointers" )
   {
      return SFT_ArrayOfPointers;
   }

   return SFT_Unknown;
}

///////////////////////////////////////////////////////////////////////////////
