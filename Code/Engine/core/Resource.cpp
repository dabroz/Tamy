#include "core.h"
#include "core\Resource.h"
#include "core\ResourcesDB.h"
#include "core\ReflectionTypesRegistry.h"
#include "core\Assert.h"


///////////////////////////////////////////////////////////////////////////////

namespace 
{
   struct ResourceDesc
   {
      std::string                typeName;
      std::ios_base::openmode    accessMode;

      ResourceDesc( const std::string& _typeName,
                    const std::ios_base::openmode& _accessMode )
         : typeName( _typeName )
         , accessMode( _accessMode )
      {}
   };

   // -------------------------------------------------------------------------

   typedef std::map< std::string, ResourceDesc > ResourceAccessModes;
   ResourceAccessModes* g_accessModesMap = NULL;

   static ResourceAccessModes& getResourceAccesModesMap()
   {
      if ( g_accessModesMap == NULL )
      {
         g_accessModesMap = new ResourceAccessModes();
      }
      return *g_accessModesMap;
   }
}

///////////////////////////////////////////////////////////////////////////////

BEGIN_ABSTRACT_OBJECT( Resource )
   PARENT( ReflectionObject);
END_OBJECT()

///////////////////////////////////////////////////////////////////////////////

Resource::Resource( const FilePath& filePath )
   : m_filePath( filePath )
   , m_host( NULL )
{
}

///////////////////////////////////////////////////////////////////////////////

Resource::~Resource()
{
   // notify attached handles that the resource is about to go out of scope
   uint count = m_referencingHandles.size();
   for ( uint i = 0; i < count; ++i )
   {
      ResourceHandle* handle = m_referencingHandles[i];
      handle->onResourceUnloaded( this );
   }

   // remove the resource from the resources manager
   if ( m_host )
   {
      m_host->getResourcesManager().unloadResource( m_filePath );
   }
}

///////////////////////////////////////////////////////////////////////////////

void Resource::setFilePath( const FilePath& path, ResourcesDB* host )
{ 
   if ( m_host != host )
   {
      ASSERT_MSG( false, "Only the host manager can move a resource to a different path" );
      return;
   }
   m_filePath = path;
}

///////////////////////////////////////////////////////////////////////////////

void Resource::saveResource()
{
   if ( m_host == NULL )
   {
      ASSERT_MSG( false, "Only managed resources can be saved using this method" );
      return;
   }

   ResourcesManager& resMgr = m_host->getResourcesManager();
   onResourceSave( resMgr );

   Filesystem& filesystem = resMgr.getFilesystem();
   ResourcesSerializationUtil::saveResource( filesystem, this, m_filePath );
}

///////////////////////////////////////////////////////////////////////////////

void Resource::setResourcesManager( ResourcesDB& hostDB )
{
   ASSERT_MSG( m_host == NULL, "This resource is already added to a resources manager" );
   m_host = &hostDB;
}

///////////////////////////////////////////////////////////////////////////////

void Resource::resetResourcesManager()
{
   m_host = NULL;
}

///////////////////////////////////////////////////////////////////////////////

bool Resource::isResource( const std::string& extension )
{
   ResourceAccessModes& accessModesMap = getResourceAccesModesMap();
   ResourceAccessModes::const_iterator it = accessModesMap.find( extension );

   return it != accessModesMap.end();
}

///////////////////////////////////////////////////////////////////////////////

std::ios_base::openmode Resource::getFileAccessMode( const std::string& extension )
{
   ResourceAccessModes& accessModesMap = getResourceAccesModesMap();
   ResourceAccessModes::const_iterator it = accessModesMap.find( extension );
   if ( it != accessModesMap.end() )
   {
      return it->second.accessMode;
   }
   else
   {
      return 0;
   }
}

///////////////////////////////////////////////////////////////////////////////

const SerializableReflectionType* Resource::findResourceClass( const std::string& extension )
{
   ResourceAccessModes& accessModesMap = getResourceAccesModesMap();
   ResourceAccessModes::const_iterator it = accessModesMap.find( extension );
   if ( it != accessModesMap.end() )
   {
      ReflectionTypesRegistry& typesRegistry = TSingleton< ReflectionTypesRegistry >::getInstance();
      return typesRegistry.findSerializable( ReflectionType::generateId( it->second.typeName ) );
   }
   else
   {
      return NULL;
   }
}

///////////////////////////////////////////////////////////////////////////////

void Resource::registerResource( const std::string& extension, 
                                 AccessMode accessMode, 
                                 const std::string& typeName )
{
   ResourceAccessModes& accessModesMap = getResourceAccesModesMap();
   ResourceAccessModes::const_iterator it = accessModesMap.find( extension );
   if ( it == accessModesMap.end() )
   {
      std::ios_base::openmode stlAccessMode = 0;
      if ( accessMode == AM_BINARY )
      {
         stlAccessMode = std::ios_base::binary;
      }

      accessModesMap.insert( std::make_pair( extension, ResourceDesc( typeName, stlAccessMode ) ) );
   }
   else
   {
      std::string errMsg = std::string( "Resource with extension ") + extension + " is already registered";
      ASSERT_MSG(false, errMsg.c_str() );
   }
}

///////////////////////////////////////////////////////////////////////////////

void Resource::finalizeResourceLoading()
{
   ASSERT_MSG( isManaged(), "Trying to access an object that was removed" );

   // inform the resource about the registered components
   onResourceLoaded( m_host->getResourcesManager() );
}

///////////////////////////////////////////////////////////////////////////////

void Resource::attachHandleListener( ResourceHandle& handle )
{
   m_referencingHandles.push_back( &handle );
}

///////////////////////////////////////////////////////////////////////////////

void Resource::detachHandleListener( ResourceHandle& handle )
{
   // find the handle entry and remove it
   uint count = m_referencingHandles.size();
   for ( uint i = 0; i < count; ++i )
   {
      if ( m_referencingHandles[i] == &handle )
      {
         m_referencingHandles.remove( i );
         break;
      }
   }
}

///////////////////////////////////////////////////////////////////////////////
