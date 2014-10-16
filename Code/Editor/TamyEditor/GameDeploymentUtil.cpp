#include "GameDeploymentUtil.h"
#include "GameDeploymentInfo.h"
#include "GameRunner.h"
#include "progressDialog.h"
#include "core.h"
#include "core-Renderer.h"
#include "core-AppFlow/GameConfig.h"
#include <windows.h>


///////////////////////////////////////////////////////////////////////////////

#define COLLECT_DIRECTORIES   1
#define COLLECT_FILES         0

namespace // anonymous
{
   class FSScanner : public FilesystemScanner
   {
   private:
      std::vector< FilePath >&         m_outPaths;
      bool                             m_collectDirectories;

   public:
      FSScanner( std::vector< FilePath >& outPaths, bool collectDirectories )
         : m_outPaths( outPaths )
         , m_collectDirectories( collectDirectories )
      {
      }

      void onDirectory( const FilePath& name ) override
      {
         if ( m_collectDirectories )
         {
            add( name );
         }
      }
      
      void onFile( const FilePath& name ) override
      {
         if ( !m_collectDirectories )
         {
            add( name );
         }
      }

   private:
      void add( const FilePath& path )
      {
         // check the uniqueness
         uint count = m_outPaths.size();
         for ( uint i = 0; i < count; ++i )
         {
            if ( m_outPaths[i] == path )
            {
               // not unique
               return;
            }
         }
         m_outPaths.push_back( path );
      }
   };

} // anonymous

///////////////////////////////////////////////////////////////////////////////

GameRunner* GameDeploymentUtil::deployGame( const GameDeploymentInfo& info, IProgressObserver& progressDialog )
{
   std::string DEPLOYMENT_RESOURCES_ROOT = info.m_targetDir + "/Data/";

   progressDialog.initialize( 4 );
   progressDialog.setStatus( "Creating directories structure" );
   bool directoryStructureCreated = recreateDirectoriesStructures( DEPLOYMENT_RESOURCES_ROOT, info.m_projectDirectories );
   if ( !directoryStructureCreated )
   {
      progressDialog.cancel();
      return NULL;
   }
   progressDialog.advance();

   progressDialog.setStatus( "Copying project files" );
   bool filesDeployed = copyProjectFiles( DEPLOYMENT_RESOURCES_ROOT, info.m_projectDirectories );
   if ( !filesDeployed )
   {
      progressDialog.cancel();
      return NULL;
   }
   progressDialog.advance();

   progressDialog.setStatus( "Creating game configuration" );
   bool configFileCreated = createGameConfig( DEPLOYMENT_RESOURCES_ROOT, info );
   if ( !configFileCreated )
   {
      progressDialog.cancel();
      return NULL;
   }
   progressDialog.advance();

   progressDialog.setStatus( "Building a game runner" );
   GameRunner* runner = createRunner( info );
   progressDialog.advance();

   return runner;
}

///////////////////////////////////////////////////////////////////////////////

bool GameDeploymentUtil::recreateDirectoriesStructures( const std::string& targetDir, const std::vector< FilePath >& projectDirectories )
{
   ResourcesManager& resMgr = TSingleton< ResourcesManager >::getInstance();
   Filesystem& fs = resMgr.getFilesystem();

   // analyze the directory structure
   std::vector< FilePath > directories;
   FSScanner directoryNamesScanner( directories, COLLECT_DIRECTORIES );

   uint directoriesCount = projectDirectories.size();
   for ( uint i = 0; i < directoriesCount; ++i )
   {
      // add the scanned directory as well
      directories.push_back( projectDirectories[i] );

      // scan both the filesystem and the resources manager
      resMgr.scan( projectDirectories[i], directoryNamesScanner, true );
   }

   // recreate the structure beneath the deployment directory
   directoriesCount = directories.size();
   for ( uint i = 0; i < directoriesCount; ++i )
   {
      bool result = createDirectory( targetDir + directories[i].c_str() );

      if ( !result )
      {
         return false;
      }
   }

   return true;
}

///////////////////////////////////////////////////////////////////////////////

bool GameDeploymentUtil::copyProjectFiles( const std::string& targetDir, const std::vector< FilePath >& projectDirectories )
{
   ResourcesManager& resMgr = TSingleton< ResourcesManager >::getInstance();
   Filesystem& fs = resMgr.getFilesystem();

   // analyze the directory structure
   std::vector< FilePath > files;
   FSScanner fileNamesScanner( files, COLLECT_FILES );

   uint directoriesCount = projectDirectories.size();
   for ( uint i = 0; i < directoriesCount; ++i )
   {
      // scan both the filesystem and the resources manager
      resMgr.scan( projectDirectories[i], fileNamesScanner, true );
   }

   // copy all files
   uint filesCount = files.size();
   for ( uint i = 0; i < filesCount; ++i )
   {
      std::string sourcePath = files[i].toAbsolutePath( fs );
      std::string targetPath = targetDir + files[i].c_str();

      // save the resource
      Resource* res = resMgr.findResource( files[i] );
      if ( res )
      {
         res->saveResource();
      }

      // copy it to the final folder
      bool result = CopyFileA( sourcePath.c_str(), targetPath.c_str(), false );

      if ( !result )
      {
         return false;
      }
   }

   return true;
}

///////////////////////////////////////////////////////////////////////////////

bool GameDeploymentUtil::createDirectory( const std::string& dir )
{
   Array< std::string > pathElements;
   StringUtils::tokenize( dir, "/\\", pathElements );

   std::string currentPath = pathElements[0];
   uint count = pathElements.size();
   for( uint i = 1; i < count; ++i )
   {
      currentPath += "\\" + pathElements[i];
      bool result = CreateDirectoryA( currentPath.c_str(), NULL );
      if ( !result )
      {
         DWORD errCode = GetLastError();
         if ( errCode != ERROR_ALREADY_EXISTS )
         {
            // the directory couldn't be created for some reason
            return false;
         }
      }
   }

   return true;
}

///////////////////////////////////////////////////////////////////////////////

bool GameDeploymentUtil::createGameConfig( const std::string& targetDir, const GameDeploymentInfo& info )
{
   ResourcesManager& resMgr = TSingleton< ResourcesManager >::getInstance();
   Filesystem& fs = resMgr.getFilesystem();


   GameConfig* config = new GameConfig();
   config->m_renderingPipelinePath = info.m_renderingPipelinePath;
   config->m_worldModelPath = info.m_worldModelPath;

   // save it in this filesystem
   FilePath gameConfigPath( "game.gcf" );
   ReflectionSerializationUtil::saveObject( config, gameConfigPath );

   // destroy the config instance
   delete config;

   // move it to the destination
   std::string gameConfigSourcePath = gameConfigPath.toAbsolutePath( fs );
   std::string gameConfigDestPath = targetDir + "/" + gameConfigPath.c_str();
   bool deploymentSuccessful = CopyFileA( gameConfigSourcePath.c_str(), gameConfigDestPath.c_str(), false ); 

   // delete the temporary file
   fs.remove( gameConfigPath );

   return deploymentSuccessful;
}

///////////////////////////////////////////////////////////////////////////////

GameRunner* GameDeploymentUtil::createRunner( const GameDeploymentInfo& info )
{
   GameRunner* runner = NULL;

   switch( info.m_platform )
   {
   case GDP_WINDOWS_OPENGL: // fallthrough
   case GDP_WINDOWS_DX9: // fallthrough
   default:
      {
         runner = deployWindowsGame( info );
         break;
      }
   }

   return runner;
}

///////////////////////////////////////////////////////////////////////////////

GameRunner* GameDeploymentUtil::deployWindowsGame( const GameDeploymentInfo& info )
{
   ResourcesManager& resMgr = TSingleton< ResourcesManager >::getInstance();
   Filesystem& fs = resMgr.getFilesystem();

   std::string gameExePath = info.m_targetDir + "/game.exe";
   {
      std::string binSourcePath;
      switch( info.m_platform )
      {
      case GDP_WINDOWS_OPENGL:
         {
            binSourcePath = fs.getCurrRoot() + "/Deployment/GameWinOpenGL/game.exe";
            break;
         }

      case GDP_WINDOWS_DX9: // fallthrough
      default:
         {
            binSourcePath = fs.getCurrRoot() + "/Deployment/GameWinDX9/game.exe";
            break;
         }
      }

      bool deploymentSuccessful = CopyFileA( binSourcePath.c_str(), gameExePath.c_str(), false );

      if ( !deploymentSuccessful )
      {
         // something broke down - bail
         return NULL;
      }
   }

   WindowsGameRunner* runner = new WindowsGameRunner( gameExePath );
   return runner;
}

///////////////////////////////////////////////////////////////////////////////

   /*
   // WHAT HAPPENS WHEN MORE PLATFORMS ARE IMPLEMENTED?
   // A lot's gonna change around here when Android, iPhone and other platforms appear, but for now let's keep this block in this method.
   // What's not gonna change is the fact the method's gonna return a GameRunner instance.


   if ( info.m_targetDir.empty() )
   {
      // cant' deploy to an unspecified directory
      return NULL;
   }

   ResourcesManager& resMgr = TSingleton< ResourcesManager >::getInstance();
   Filesystem& fs = resMgr.getFilesystem();
   std::string DEPLOYMENT_RESOURCES_ROOT = info.m_targetDir + "/Data/";


   // get the rendering pipeline, 'cause it's one of the elements we're gonna be saving
   const FilePath& renderingPipelinePath = m_sceneWidget->getRenderingPipeline();
   RenderingPipeline* pipeline = resMgr.findResource< RenderingPipeline >( renderingPipelinePath );
   if ( !pipeline )
   {
      // the pipeline wasn't loaded - that's weird ( since we have the scene widget initialized and all ).
      // Investigate, but don't deploy anything.
      ASSERT_MSG( pipeline != NULL, "Rendering pipeline not defined" );

      return NULL;
   }

   // save the resources ( we want to save the state most up to date )
   {
      m_scene.saveResource();
      pipeline->saveResource();
   }

   // create the resources map that needs to be copied
   std::vector< FilePath > externalDependencies;
   {
      ReflectionSerializationUtil::collectExternalDependencies( &m_scene, externalDependencies );
      ReflectionSerializationUtil::collectExternalDependencies( pipeline, externalDependencies );
   }

   progressDialog->initialize( "Game deployment", externalDependencies.size() );

   // copy the resources to the "Data" directory
   {
      uint count = externalDependencies.size();
      bool deploymentSuccessful = true;
      for ( uint i = 0; i < count; ++i )
      {
         std::string sourcePath = externalDependencies[i].toAbsolutePath( fs );
         std::string targetPath = DEPLOYMENT_RESOURCES_ROOT + externalDependencies[i].c_str();
         deploymentSuccessful = copyFileEnsurePath( sourcePath.c_str(), targetPath.c_str() );

         // advance the progress dialog
         progressDialog->advance();
      }

      if ( !deploymentSuccessful )
      {
         // something broke down - bail
         return NULL;
      }
   }


   // copy the selected game binary 
   std::string gameExePath = info.m_targetDir + "/game.exe";
   {
      std::string binSourcePath;
      switch( info.m_platform )
      {
      case GDP_WINDOWS_OPENGL:
         {
            binSourcePath = fs.getCurrRoot() + "/Deployment/GameWinOpenGL/game.exe";
            break;
         }

      case GDP_WINDOWS_DX9: // fallthrough
      default:
         {
            binSourcePath = fs.getCurrRoot() + "/Deployment/GameWinDX9/game.exe";
            break;
         }
      }

      bool deploymentSuccessful = copyFileEnsurePath( binSourcePath.c_str(), gameExePath.c_str() );
      // advance the progress dialog
      progressDialog->advance();

      if ( !deploymentSuccessful )
      {
         // something broke down - bail
         return NULL;
      }
   }

   // save the configuration resource
   {
      GameConfig* config = new GameConfig();
      config->m_renderingPipelinePath = renderingPipelinePath;
      config->m_worldModelPath = m_scene.getFilePath();

      // save it in this filesystem
      FilePath gameConfigPath( "game.gcf" );
      ReflectionSerializationUtil::saveObject( config, gameConfigPath );

      // destroy the config instance
      delete config;

      // move it to the destination
      std::string gameConfigSourcePath = gameConfigPath.toAbsolutePath( fs );
      std::string gameConfigDestPath = DEPLOYMENT_RESOURCES_ROOT + "/" + gameConfigPath.c_str();
      bool deploymentSuccessful = copyFileEnsurePath( gameConfigSourcePath, gameConfigDestPath ); 

      // delete the temporary file
      fs.remove( gameConfigPath );

      if ( !deploymentSuccessful )
      {
         // something broke down - bail
         return NULL;
      }

   }

   // create the runner
   GameRunner* runner = NULL;
   switch( info.m_platform )
   {
   case GDP_WINDOWS_OPENGL:
      {
         runner = new WindowsGameRunner( gameExePath );
         break;
      }

   case GDP_WINDOWS_DX9: // fallthrough
   default:
      {
         runner = new WindowsGameRunner( gameExePath );
         break;
      }
   }

   return runner;
}
*/

///////////////////////////////////////////////////////////////////////////////

#undef COLLECT_DIRECTORIES
#undef COLLECT_FILES

///////////////////////////////////////////////////////////////////////////////
