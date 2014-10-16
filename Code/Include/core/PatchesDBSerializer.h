/// @file   core/PatchesDBSerializer.h
/// @brief  PatchesDB serialization tool
#pragma once

#include <string>


///////////////////////////////////////////////////////////////////////////////

class TiXmlElement;
enum SerializedFieldType;
class PatchesDB;
class OutStream;
class InStream;

///////////////////////////////////////////////////////////////////////////////

/**
 * PatchesDB serialization tool
 */
class PatchesDBSerializer
{
public:
   /**
    * Saves database contents to the specified stream.
    *
    * @param patchesDB
    * @param stream
    */
   static void save( const PatchesDB& patchesDB, OutStream& stream );

   /**
    * Loads database contents from the specified stream.
    *
    * @param patchesDB
    * @param dbXMLDefinition
    */
   static void load( PatchesDB& patchesDB, const std::string& dbXMLDefinition );

private:
   static void loadPatchEntry( PatchesDB& patchesDB, TiXmlElement* patchEntryElem );

   static SerializedFieldType translateFieldType( const std::string& typeName );
};

///////////////////////////////////////////////////////////////////////////////
