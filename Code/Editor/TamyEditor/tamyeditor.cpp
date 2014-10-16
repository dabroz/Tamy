#include "tamyeditor.h"
#include "core-AppFlow.h"
#include "core-Renderer.h"
#include "core-MVC.h"
#include "core-AI.h"
#include "ext-StoryTeller.h"
#include "ext-2DGameLevel\GL2DSystem.h"
#include "ext-RenderingPipeline.h"
#include "core-Physics\PhysicsSystem.h"
#include "core-Physics\PhysicsMaterial.h"
#include "progressdialog.h"
#include <QtCore\QTimer>
#include <QtCore\QSettings>
#include <QtGui\QCloseEvent>
#include <QtWidgets\QDockWidget>
#include <QtWidgets\QTreeWidget>
#include <QtWidgets\QTabWidget>
#include <QtWidgets\QSplitter>
#include <QtWidgets\QFrame>
#include <QtWidgets\QVBoxLayout>
#include <QtGui\QPalette>
#include <QtWidgets\QProgressBar>
#include "ResourcesBrowser.h"
#include "MainEditorPanel.h"
#include "UISerializationUtil.h"
#include "ToolsWindow.h"

// editors
#include "SceneEditor.h"
#include "PrefabEditor.h"
#include "ShaderEditor.h"
#include "TextureEditor.h"
#include "CubeTextureEditor.h"
#include "MaterialEditor.h"
#include "ProjectEditor.h"
#include "StoryEditor.h"
#include "BehaviorTreeEditor.h"
#include "BlendTreeEditor.h"
#include "PhysicsMaterialEditor.h"

// resources
#include "ResourceManagementUtil.h"
#include "Project.h"

// tools
#include "core\Profiler.h"
#include "assertDialog.h"

// events
#include "EventAssert.h"


///////////////////////////////////////////////////////////////////////////////

TamyEditor* TamyEditor::s_theInstance = NULL;

#define TIME_STEP ( 1.f/60.f )

///////////////////////////////////////////////////////////////////////////////

void TamyEditor::createInstance( QApplication& app, QWidget *parent )
{
   delete s_theInstance;
   s_theInstance = new TamyEditor( app, parent );
   s_theInstance->initialize();

   // serialize UI layout
   UISerializationUtil::serialize( s_theInstance, s_theInstance->m_editorsTabs, false );
}

///////////////////////////////////////////////////////////////////////////////

void TamyEditor::destroyInstance()
{
   // deserialize UI layout
   UISerializationUtil::serialize( s_theInstance, s_theInstance->m_editorsTabs, true );

   delete s_theInstance;
   s_theInstance = NULL;
}

///////////////////////////////////////////////////////////////////////////////

TamyEditor::TamyEditor( QApplication& app, QWidget *parent )
   : QMainWindow( parent )
   , m_app( app )
   , m_mainTime( new CTimer() )
   , m_resourcesBrowser( NULL )
   , m_activeProject( NULL )
   , m_toolsWindow( NULL )
   , m_timeAccumulator( 0.f )

{
   ui.setupUi( this );
   setAutoFillBackground( true );
}

///////////////////////////////////////////////////////////////////////////////

void TamyEditor::initialize()
{
   // create the main subsystems
   m_timeController = &TSingleton< TimeController >::getInstance();

   // setup the file system - it's paramount this works before anything in the application gets set up
   Filesystem& fs = TSingleton< Filesystem >::getInstance();
   fs.attach( *this );
   const char* fsRoot = fs.getCurrRoot().c_str();

   // create the timer
   m_mainTimeSlot = new QTimer( this );
   connect( m_mainTimeSlot, SIGNAL( timeout() ), this, SLOT( updateMain() ) );
   m_mainTimeSlot->start( 1 );

   // load the settings file
   m_editorSettings = new QSettings( ( std::string( fsRoot ) + "/Editor/Settings.ini" ).c_str(), QSettings::IniFormat );

   // associate resources with their respective editors
   associate< Model, SceneEditor >();
   associate< Prefab, PrefabEditor >();
   associate< Shader, ShaderEditor >();
   associate< Material, MaterialEditor >();
   associate< Texture, TextureEditor >();
   associate< CubeTexture, CubeTextureEditor >();
   associate< Project, ProjectEditor >();
   associate< Story, StoryEditor >();
   associate< BehaviorTree, BehaviorTreeEditor >();
   associate< BlendTree, BlendTreeEditor >();
   associate< PhysicsMaterial, PhysicsMaterialEditor >();

   // populate view contents
   {
      QAction* toggleStatusWindowAction = new QAction( "Status Window", ui.menuView );
      connect( toggleStatusWindowAction, SIGNAL( triggered() ), this, SLOT( toggleStatusWindow() ) );
      ui.menuView->addAction( toggleStatusWindowAction );

      QAction* startProfilerAction = new QAction( "Profiler", ui.menuView );
      connect( startProfilerAction, SIGNAL( triggered() ), this, SLOT( startProfiler() ) );
      ui.menuView->addAction( startProfilerAction );
   }

   // define editor's stylesheet
   {
      Filesystem& fs = TSingleton< Filesystem >::getInstance();
      File* file = fs.open( FilePath( "/Editor/Theme.css" ) );
      StreamBuffer< char > fileBuf( *file );
      QString styleSheet = fileBuf.getBuffer();
      delete file;

      m_app.setStyleSheet( styleSheet );
   }

   // add the resources browser
   {
      m_resourcesBrowser = new ResourcesBrowser( this );
      addDockWidget( Qt::LeftDockWidgetArea, m_resourcesBrowser );
   }

   // add the editors tabs
   {
      m_editorsTabs = new MainEditorPanel( ui.renderWindow, this );
      connect( m_editorsTabs, SIGNAL( onTabClosed( QWidget* ) ), this, SLOT( onEditorTabClosed( QWidget* ) ) );
      ui.renderWindow->layout()->addWidget( m_editorsTabs );
   }

}

///////////////////////////////////////////////////////////////////////////////

TamyEditor::~TamyEditor()
{
   Filesystem& fs = TSingleton< Filesystem >::getInstance();
   fs.detach( *this );

   // remove all editors
   std::vector< ResourceEditor* > editors;
   m_editorsTabs->collectEditors( editors );
   m_editorsTabs->clear();

   uint editorsCount = editors.size();
   for ( unsigned int i = 0; i < editorsCount; ++i )
   {
      ResourceEditor* editor = editors[i];
      if ( editor )
      {
         editor->deinitialize( false );
      }
      delete editor;
   }

   m_timeController = NULL;

   // delete the subsystems
   delete m_mainTime; m_mainTime = NULL;
   delete m_mainTimeSlot; m_mainTimeSlot = NULL;
   delete m_editorSettings; m_editorSettings = NULL;
}

///////////////////////////////////////////////////////////////////////////////

QMenu& TamyEditor::getFileMenu()
{
   return *(ui.menuFile);
}

///////////////////////////////////////////////////////////////////////////////

QMenu& TamyEditor::getViewMenu()
{
   return *(ui.menuView);
}

///////////////////////////////////////////////////////////////////////////////

QToolBar& TamyEditor::getToolBar()
{
   return *(ui.mainToolBar);
}

///////////////////////////////////////////////////////////////////////////////

void TamyEditor::updateMain()
{
   // enable filesystem changes tracking only when the editor has started ( calling this method twice in a row won't
   // cause problems, so we can safely do it here )
   Filesystem& filesystem = TSingleton< Filesystem >::getInstance();
   filesystem.startTrackingChanges();

   // tick the main timer
   m_mainTime->tick();
   float timeElapsed = m_mainTime->getTimeElapsed();

   // accumulate the time step until a certain step length is reached - we want to step it at 60 MHz
   m_timeAccumulator += timeElapsed;
   if ( m_timeAccumulator < TIME_STEP )
   {
      return;
   }

   // if more than one frame worth of time has passed in the meantime - ignore it
   while ( m_timeAccumulator > TIME_STEP )
   {
      m_timeAccumulator -= TIME_STEP;
   }

   // dispatch events
   TSingleton< EventsDispatcher >::getInstance().dispatchEvents();
   // update the tools window
   if ( m_toolsWindow )
   {
      m_toolsWindow->update( TIME_STEP );
   }

   // update engine's main loop
   {
      m_timeController->update( TIME_STEP );

      static AISystem& animSys = TSingleton< AISystem >::getInstance();
      static GL2DSystem& gameLevel2DSys = TSingleton< GL2DSystem >::getInstance();
      static PhysicsSystem& physicsSys = TSingleton< PhysicsSystem >::getInstance();
      animSys.tick( TIME_STEP );
      physicsSys.tick( TIME_STEP );
      gameLevel2DSys.tick( TIME_STEP );

      static TransformsManagementSystem& transformsMgr = TSingleton< TransformsManagementSystem >::getInstance();
      transformsMgr.tick();

      static RenderSystem& renderSystem = TSingleton< RenderSystem >::getInstance();
      renderSystem.render();

      // track the changes in the filesystem
      static Filesystem& filesystem = TSingleton< Filesystem >::getInstance();
      filesystem.trackChanges();
   }
}

///////////////////////////////////////////////////////////////////////////////

void TamyEditor::closeEvent( QCloseEvent *event )
{
   // accept the event
   event->accept();
}

///////////////////////////////////////////////////////////////////////////////

bool TamyEditor::activateResourceEditor( Resource* resource )
{
   // first - look for a tab with the same path - if there is one, it means that we're
   // already editing this resource
   TabLocation location;
   bool found = m_editorsTabs->findTabByName( resource->getFilePath().c_str(), location );
   if ( found )
   {
      // yep - we're already editing it. Focus on it and that's it
      m_editorsTabs->setActiveTab( location );
   }

   return found;
}

///////////////////////////////////////////////////////////////////////////////

ResourceEditor* TamyEditor::createResourceEditor( Resource* resource, const QIcon& icon )
{
   // no editor can be created without an active project
   if ( m_activeProject == NULL )
   {
      return NULL;
   }

   // and even if there's an active project, the edited resource must be a part of it
   FilePath resourcePath = resource->getFilePath();
   if ( m_activeProject->isMember( resourcePath ) == false )
   {
      return NULL;
   }

   // if we got this far, it means that we need a new editor to edit this resource
   ResourceEditor* editor = GenericFactory< Resource, ResourceEditor >::create( *resource );
   if ( editor )
   {
      editor->initialize( resourcePath.c_str(), icon );
   }
   return editor;
}

///////////////////////////////////////////////////////////////////////////////

void TamyEditor::addResourceEditor( ResourceEditor* editor, Qt::DockWidgetArea dockArea )
{
   if ( editor != NULL )
   {
      m_editorsTabs->addEditor( editor, editor->getIcon(), editor->getLabel(), dockArea );
   }
}

///////////////////////////////////////////////////////////////////////////////

void TamyEditor::onEditorTabClosed( QWidget* editorWidget )
{
   ResourceEditor* editor = dynamic_cast< ResourceEditor* >( editorWidget );
   if ( editor )
   {
      editor->deinitialize( false );
      delete editor;
   }
}

///////////////////////////////////////////////////////////////////////////////

void TamyEditor::onDirAdded( const FilePath& dir )
{
   // nothing to do here
}

///////////////////////////////////////////////////////////////////////////////

void TamyEditor::onDirRemoved( const FilePath& dir )
{
   if ( !m_activeProject )
   {
      return;
   }

   // if the removed directory was where the active project was located, we need to close the project
   if ( m_activeProjectPath.isLocatedInDir( dir ) )
   {
      // yes it was - close the project
      setActiveProject( NULL );
   }
   else
   {
      closeEditors( dir, PathTest::FROM_METHOD( TamyEditor, isResourceFromDeletedDir, this ) );
   }
}

///////////////////////////////////////////////////////////////////////////////

void TamyEditor::onFileAdded( const FilePath& path )
{
   // nothing to do here
}

///////////////////////////////////////////////////////////////////////////////

void TamyEditor::onFileEdited( const FilePath& path )
{
   // nothing to do here
}

///////////////////////////////////////////////////////////////////////////////

void TamyEditor::onFileRemoved( const FilePath& path )
{
   if ( !m_activeProject )
   {
      return;
   }

   // if the removed file was the active project resource, close the project
   if ( m_activeProjectPath == path )
   {
      // yes it was - close the project
      setActiveProject( NULL );
   }
   else
   {
      // and close any editors tied to this particular resource ( Project may also have an editor assigned to it and it may be open at the moment )
      closeEditors( path, PathTest::FROM_METHOD( TamyEditor, isSamePath, this ) );
   }
}

///////////////////////////////////////////////////////////////////////////////

void TamyEditor::onFileRenamed( const FilePath& oldPath, const FilePath& newPath )
{
   if ( !m_activeProject )
   {
      // this path doesn't concern the currently active project
      return;
   }

   if ( m_activeProjectPath == oldPath )
   {
      // it's the project path that has changed - update it
      m_activeProjectPath = newPath;
   }

   // rename the editors
   std::vector< ResourceEditor* > editors;
   std::vector< TabLocation > tabsLocations;
   m_editorsTabs->collectEditors( editors, &tabsLocations );

   const int editorsCount = editors.size();
   for ( int i = 0; i < editorsCount; ++i )
   {
      ResourceEditor* editor = editors[i];
      FilePath resourcePath( editor->getLabel().toStdString() );

      if ( editor && oldPath == resourcePath )
      {
         editor->setLabel( newPath.c_str() );
         m_editorsTabs->setTabName( tabsLocations[i], newPath.c_str() );

         // found it
         return;
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void TamyEditor::onDirRenamed( const FilePath& oldPath, const FilePath& newPath )
{
   if ( !m_activeProject || !m_activeProjectPath.isLocatedInDir( oldPath ) )
   {
      // this path doesn't concern the currently active project
      return;
   }

   // rename the editors
   std::vector< ResourceEditor* > editors;
   std::vector< TabLocation > tabsLocations;
   m_editorsTabs->collectEditors( editors, &tabsLocations );

   const int editorsCount = editors.size();
   for ( int i = 0; i < editorsCount; ++i )
   {
      ResourceEditor* editor = editors[i];
      FilePath resourcePath( editor->getLabel().toStdString() );

      if ( editor )
      {
         ASSERT( resourcePath.isLocatedInDir( oldPath ) );
         resourcePath.swapPart( oldPath, newPath, resourcePath );
         editor->setLabel( resourcePath.c_str() );
         m_editorsTabs->setTabName( tabsLocations[i], resourcePath.c_str() );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void TamyEditor::onFilesystemDeleted( Filesystem& filesystem )
{
   // nothing to do here
}

///////////////////////////////////////////////////////////////////////////////

void TamyEditor::closeEditors( const FilePath& dir, const PathTest& test )
{
   // if any of the resource editors are editing a resource from the deleted directory, close them
   // immediately
   std::vector< ResourceEditor* > editors;
   std::vector< TabLocation > tabsLocations;
   m_editorsTabs->collectEditors( editors, &tabsLocations );

   const int editorsCount = editors.size();
   for ( int i = editorsCount - 1; i >= 0; --i )
   {
      ResourceEditor* editor = editors[i];
      FilePath resourcePath( editor->getLabel().toStdString() );

      if ( editor && test( resourcePath, dir ) )
      {
         editor->deinitialize( false );
         delete editor;
         m_editorsTabs->removeTab( tabsLocations[i] );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

bool TamyEditor::isSamePath( const FilePath& resourcePath, const FilePath& checkedPath )
{
   return resourcePath == checkedPath;
}

///////////////////////////////////////////////////////////////////////////////

bool TamyEditor::isParentDir( const FilePath& resourcePath, const FilePath& dir )
{
   return resourcePath.isLocatedInDir( dir );
}

///////////////////////////////////////////////////////////////////////////////

bool TamyEditor::isResourceFromDeletedDir( const FilePath& resourcePath, const FilePath& deletedDir )
{
   const Filesystem& fs = TSingleton< ResourcesManager >::getInstance().getFilesystem();
   return resourcePath.isLocatedInDir( deletedDir ) && !fs.doesExist( resourcePath );
}

///////////////////////////////////////////////////////////////////////////////

void TamyEditor::setActiveProject( Project* project )
{
   if ( m_activeProject == project )
   {
      // nothing's changed
      return;
   }

   // close all active editors
   closeEditors( FilePath( "/" ), PathTest::FROM_METHOD( TamyEditor, allTrue, this ) );

   // set the new project as the active one
   m_activeProject = project;

   // memorize project's path
   // set the project path
   if ( m_activeProject )
   {
      m_activeProjectPath = m_activeProject->getFilePath();
   }
   else
   {
      m_activeProjectPath = FilePath();
   }

   // notify the resources browser about it
   m_resourcesBrowser->setActiveProject( m_activeProject );
}

///////////////////////////////////////////////////////////////////////////////

void TamyEditor::startProfiler()
{
   ToolsWindow* toolsWindow = getToolsWindow();
   toolsWindow->showProfilers();
}

///////////////////////////////////////////////////////////////////////////////

QSettings& TamyEditor::getSettings() 
{ 
   ASSERT( m_editorSettings );
   return *m_editorSettings; 
}

///////////////////////////////////////////////////////////////////////////////

ToolsWindow* TamyEditor::getToolsWindow()
{
   if ( !m_toolsWindow )
   {
      m_toolsWindow = new ToolsWindow( this );
      connect( m_toolsWindow, SIGNAL( onClosed( ClosableDockWidget* ) ), this, SLOT( onStatusWindowClosed( ClosableDockWidget* ) ) );
      addDockWidget( Qt::BottomDockWidgetArea, m_toolsWindow );
   }

   return m_toolsWindow;
}

///////////////////////////////////////////////////////////////////////////////

void TamyEditor::onStatusWindowClosed( ClosableDockWidget* )
{
   m_toolsWindow = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void TamyEditor::toggleStatusWindow()
{
   if ( !m_toolsWindow )
   {
      getToolsWindow();
   }
   else
   {
      m_toolsWindow->close();
      m_toolsWindow = NULL;
   }
}

///////////////////////////////////////////////////////////////////////////////
