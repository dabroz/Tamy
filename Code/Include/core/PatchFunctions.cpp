#include "core\PatchesDBMacros.h"
#include "core\SerializedReflectionObject.h"
#include "core\FilePath.h"


///////////////////////////////////////////////////////////////////////////////

void patch_ResourceHandle_0_1( SerializedReflectionObject& migratedObject )
{
   SerializedValue* oldPathElem = migratedObject.getValue( "m_resourcePath" );
   SerializedValue* newPathElem = migratedObject.getValue( "m_path" );
   if ( !oldPathElem || !newPathElem )
   {
      return;
   }

   std::string oldPath;
   oldPathElem->initialize< std::string >( oldPath );

   FilePath newPath( oldPath );
   newPathElem->set< FilePath >( newPath );
}
REGISTER_PATCH_FUNCTION( patch_ResourceHandle_0_1 );

///////////////////////////////////////////////////////////////////////////////
