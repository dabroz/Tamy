#include "ext-2DGameLevel\GL2DVoxelPrefabsMap.h"
#include "core-MVC\Prefab.h"
#include "core\Filesystem.h"
#include "core\ResourcesManager.h"
#include "core\FilesystemScanner.h"


///////////////////////////////////////////////////////////////////////////////

namespace // anonymous
{
   class Scanner : public FilesystemScanner
   {
   private:
      GL2DVoxelPrefabsMap&    m_host;
      ResourcesManager&       m_resMgr;

   public:
      Scanner( GL2DVoxelPrefabsMap& host, ResourcesManager& resMgr )
         : m_host( host )
         , m_resMgr( resMgr )
      {
      }

      // ----------------------------------------------------------------------
      // FilesystemScanner implementation
      // ----------------------------------------------------------------------
      void onFile( const FilePath& name ) override
      {        
         Prefab* prefab = m_resMgr.create< Prefab >( name, true );
         if ( prefab )
         {
            m_host.addPrefab( prefab );
         }
      }
   };
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

GL2DVoxelPrefabsMap::GL2DVoxelPrefabsMap( const FilePath& prefabsRootDir )
{
   ResourcesManager& resMgr = TSingleton< ResourcesManager >::getInstance();
   Scanner scanner( *this, resMgr );
   resMgr.scan( prefabsRootDir, scanner );
}

///////////////////////////////////////////////////////////////////////////////

GL2DVoxelPrefabsMap::~GL2DVoxelPrefabsMap()
{
   const uint count = m_prefabs.size();
   for ( uint i = 0; i < count; ++i )
   {
      Prefab* prefab = m_prefabs[i];
   }
   m_prefabs.clear();
}

///////////////////////////////////////////////////////////////////////////////

void GL2DVoxelPrefabsMap::addPrefab( Prefab* prefab )
{
   if ( prefab )
   {
      m_prefabs.push_back( prefab );
   }
}

///////////////////////////////////////////////////////////////////////////////
