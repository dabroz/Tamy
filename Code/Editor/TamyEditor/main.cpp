#include "tamyeditor.h"
#include <QtWidgets\QApplication>
#include <QtWidgets\QMessageBox>
#include <QtCore\QResource>
#include "assertDialog.h"
#include "core\Assert.h"
#include "AssertHandler.h"
#include "core\ResourceUtils.h"
#include "Defines.h"

// renderer implementation
#include "gl-Renderer.h"


///////////////////////////////////////////////////////////////////////////////

// type includes
#include "core/TypesRegistry.cpp"
#include "core-MVC/TypesRegistry.cpp"
#include "core-Renderer/TypesRegistry.cpp"
#include "core-AI/TypesRegistry.cpp"
#include "core-AppFlow/TypesRegistry.cpp"
#include "core-Physics/TypesRegistry.cpp"
#include "ext-StoryTeller/TypesRegistry.cpp"
#include "ext-RenderingPipeline/TypesRegistry.cpp"
#include "ext-2DGameLevel/TypesRegistry.cpp"
#include "ext-ProceduralAnimation/TypesRegistry.cpp"

// patches
#include "core/PatchFunctions.cpp"

///////////////////////////////////////////////////////////////////////////////

class TamyEditorApp : public QApplication
{
public:
   TamyEditorApp(int argc, char* argv[])
   : QApplication(argc, argv)
   {}

protected:
   bool notify(QObject* receiver, QEvent* event )
   {
      bool result = false;
      try
      {
         result = QApplication::notify(receiver, event);
      }
      catch (std::exception& ex)
      {
         QMessageBox::warning(NULL, tr("Tamy editor error"),
            QString(ex.what()),
            QMessageBox::Ok);
      }

      return result;
   }
};

///////////////////////////////////////////////////////////////////////////////

//#define RESAVE_RESOURCES

int main(int argc, char *argv[])
{
   TamyEditorApp a(argc, argv);

   // register editor resources
   {
      bool wereResourcesRegistered = QResource::registerResource("tamyeditor.rcc");
      ASSERT( wereResourcesRegistered );
   }

   // initialize the thread system
   TSingleton< ThreadSystem >::initialize();

   // the application is started from Build\$(ProjectName)\$(Configuration)\ subdir - we have to escape
   // from there, and there should be our Assets directory
   std::string assetsDir;
   {
      // get the directory the application was started from and acquire path to the data repository
      char szDirectory[ MAX_PATH ];
      if( !::GetCurrentDirectoryA( sizeof( szDirectory ) - 1, szDirectory ) )
      {
         return -1;
      }

      FilesystemUtils::normalize( szDirectory, assetsDir ); 
      FilesystemUtils::leaveDir( assetsDir, 3, assetsDir ); 
      assetsDir += "/Assets/";

      TSingleton< Filesystem >::initialize().changeRootDir( assetsDir );

      TSingleton< ResourcesManager >::initialize();
      ResourcesManager& resMgr = TSingleton< ResourcesManager >::getInstance();
   }


   // build the types registry 
   {
      // load patching definitions
      Filesystem& fs = TSingleton< ResourcesManager >::getInstance().getFilesystem();
      FilePath patchesDBDefPath( "/Renderer/patchesDB.xml" );
      File* patchesDBFile = fs.open( patchesDBDefPath, std::ios_base::in );
      StreamBuffer< char > fileBuf( *patchesDBFile );
      std::string patchesDBContents = fileBuf.getBuffer();
      delete patchesDBFile;

      PatchesDB& patchesDB = TSingleton< PatchesDB >::getInstance();
      PatchesDBSerializer::load( patchesDB, patchesDBContents );

      TSingleton< ReflectionTypesRegistry >::getInstance().build( patchesDB );
   }

   // register an assertion callback
   registerAssertCallback( &assertionCallback );


#ifdef RESAVE_RESOURCES

   // resave assets
   ResourceUtils::resaveRepository( resMgr );

   // deinitialize
   SingletonsManager::deinitialize();

   return true;

#else

   // initialize the physics system
   TSingleton< PhysicsSystem >::initialize();

   // create the editor
   TamyEditor::createInstance( a );
   TamyEditor& w = TamyEditor::getInstance();

   // start the application
   w.show();
   bool result = a.exec();

   // deinitialize
   TamyEditor::destroyInstance();
   SingletonsManager::deinitialize();

   return result;

#endif
}

///////////////////////////////////////////////////////////////////////////////
