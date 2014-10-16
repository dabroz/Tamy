#include "ext-2DGameLevel\GL2DVoxelPrefabsMap.h"
#include "ext-2DGameLevel\GL2DLevelVoxels.h"
#include "ext-2DGameLevel\GL2DRoomTheme.h"
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
      uint                    m_rootDirPathOffset;
      GL2DVoxelPrefabsMap&    m_host;
      ResourcesManager&       m_resMgr;

   public:
      Scanner( uint rootDirPathOffset, GL2DVoxelPrefabsMap& host, ResourcesManager& resMgr )
         : m_rootDirPathOffset( rootDirPathOffset )
         , m_host( host )
         , m_resMgr( resMgr )
      {
      }

      // ----------------------------------------------------------------------
      // FilesystemScanner implementation
      // ----------------------------------------------------------------------
      void onFile( const FilePath& name ) override
      {
         std::string path = name.getRelativePath().substr( m_rootDirPathOffset );

         Array< std::string > dirs;
         StringUtils::tokenize( path, "/\\", dirs );

         // we expect the remaining path to be divided into three parts, that correspond
         // to the predefined directory structure:
         // /RoomType/PrefabType/prefabFileName
         if ( dirs.size() != 3 )
         {
            // this file's of no interest to us
            return;
         }

         const std::string& roomTypeName = dirs[0];
         const std::string& prefabTypeName = dirs[1];
         const std::string& prefabFileName = dirs[2];

         GL2DRoomTheme* roomType = m_host.findTheme( roomTypeName );
         if ( !roomType )
         {
            roomType = m_host.addTheme( roomTypeName );
         }

         Prefab* prefab = m_resMgr.create< Prefab >( name );
         if ( prefabTypeName == "Core" || prefabTypeName == "Corridor" )
         {
            roomType->addCorePrefab( prefab );
         }
         else
         {
            roomType->addDecorativePrefab( prefab );
         }
      }
   };
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

GL2DVoxelPrefabsMap::GL2DVoxelPrefabsMap( const FilePath& prefabsRootDir )
   : m_rootDirPathOffset( prefabsRootDir.getRelativePath().length() )
{
   ResourcesManager& resMgr = TSingleton< ResourcesManager >::getInstance();
   Scanner scanner( m_rootDirPathOffset, *this, resMgr );
   resMgr.scan( prefabsRootDir, scanner );
}

///////////////////////////////////////////////////////////////////////////////

GL2DVoxelPrefabsMap::~GL2DVoxelPrefabsMap()
{
   const uint count = m_themes.size();
   for ( uint i = 0; i < count; ++i )
   {
      delete m_themes[i];
   }
   m_themes.clear();
}

///////////////////////////////////////////////////////////////////////////////

GL2DRoomTheme* GL2DVoxelPrefabsMap::findTheme( const std::string& themeName )
{
   const uint count = m_themes.size();
   for ( uint i = 0; i < count; ++i )
   {
      GL2DRoomTheme* theme = m_themes[i];
      if ( theme->m_name == themeName )
      {
         return theme;
      }
   }

   return NULL;
}

///////////////////////////////////////////////////////////////////////////////

GL2DRoomTheme* GL2DVoxelPrefabsMap::addTheme( const std::string& themeName )
{
   GL2DRoomTheme* theme = new GL2DRoomTheme( themeName );
   m_themes.push_back( theme );
   
   return theme;
}

///////////////////////////////////////////////////////////////////////////////

GL2DRoomTheme* GL2DVoxelPrefabsMap::getRandomTheme() const
{
   const uint themeIdx = rand() % m_themes.size();
   return m_themes[themeIdx];
}

///////////////////////////////////////////////////////////////////////////////
