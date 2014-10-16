#include "ResourcesBrowser.h"
#include <QtWidgets\QTreeWidget>
#include <QtWidgets\QTreeWidgetItem>
#include <QtWidgets\QDockWidget>
#include <QtGui\QDrag>
#include <QtCore\QMimeData>
#include <QtWidgets\QInputDialog>
#include <QtWidgets\QToolBar>
#include <QtWidgets\QMenu>
#include <QtWidgets\QTabWidget>
#include <QtWidgets\QListWidget>
#include <QtCore\QSettings>
#include "core.h"
#include "progressdialog.h"
#include "tamyeditor.h"
#include "Project.h"
#include <list>

// trees
#include "FilesystemTree.h"
#include "FSTreeNode.h"
#include "FSRootNode.h"
#include "FSDirNode.h"
#include "FSLeafNode.h"


///////////////////////////////////////////////////////////////////////////////

#define BOOKMARK_FILEPATH_DATA_INDEX      1

///////////////////////////////////////////////////////////////////////////////

ResourcesBrowser::ResourcesBrowser( QWidget* parentWidget )
   : QDockWidget( "Resources Browser", parentWidget )
   , m_tabsManager( NULL )
   , m_projectBrowserTree( NULL )
   , m_activeProject( NULL )
{
   setObjectName("ResourcesBrowser/dockWidget");

   // initialize user interface
   initUI();
}

///////////////////////////////////////////////////////////////////////////////

ResourcesBrowser::~ResourcesBrowser()
{
}

///////////////////////////////////////////////////////////////////////////////

void ResourcesBrowser::initUI()
{
   // setup dockable properties view widget
   QWidget* dockWidgetContents = new QWidget( this );
   dockWidgetContents->setObjectName("ResourcesBrowser/dockWidgetContents");
   setWidget( dockWidgetContents );

   QVBoxLayout* layout = new QVBoxLayout( dockWidgetContents );
   dockWidgetContents->setLayout( layout );
   layout->setSpacing(0);
   layout->setMargin(0);

   // create the tabs manager that will host all browser feature tabs
   {
      m_tabsManager = new QTabWidget( dockWidgetContents );
      m_tabsManager->setObjectName( "ResourcesBrowser/tabsManager" );
      layout->addWidget( m_tabsManager );

      // setup the scene tree container widgets
      setupProjectTree();
      setupFilesystemTree();
      
      // setup the bookmarks tab
      setupBookmarks();

      // setup the search tab
      setupFileFinder();
   }

   // by default activate the filesystem browser - that's where the user should start
   // by creating a project
   m_tabsManager->setCurrentIndex( TI_Filesystem );
}

///////////////////////////////////////////////////////////////////////////////

void ResourcesBrowser::setupProjectTree()
{
   ASSERT( m_tabsManager != NULL );

   m_projectBrowserTree = new FilesystemTree( m_tabsManager, "ResourcesBrowser/projectTreeFrame" );

   // add the tree to a new tab
   m_tabsManager->addTab( m_projectBrowserTree, "Project" );
   connect( m_projectBrowserTree, SIGNAL( popupMenuRequest( QTreeWidgetItem*, QMenu& ) ), this, SLOT( showProjectBrowserPopupMenu( QTreeWidgetItem*, QMenu& ) ) );
}

///////////////////////////////////////////////////////////////////////////////

void ResourcesBrowser::setupFilesystemTree()
{
   ASSERT( m_tabsManager != NULL );

   m_filesystemBrowserTree = new FilesystemTree( m_tabsManager, "ResourcesBrowser/filesystemTreeFrame" );

   m_tabsManager->addTab( m_filesystemBrowserTree, "Filesystem" );
   connect( m_filesystemBrowserTree, SIGNAL( popupMenuRequest( QTreeWidgetItem*, QMenu& ) ), this, SLOT( showFilesystemBrowserPopupMenu( QTreeWidgetItem*, QMenu& ) ) );
}

///////////////////////////////////////////////////////////////////////////////

void ResourcesBrowser::setupBookmarks()
{
   ASSERT( m_tabsManager != NULL );

   QString topObjectName( "ResourcesBrowser/bookmarksFrame" );

   // frame
   QFrame* frame = NULL;
   QVBoxLayout* layout = NULL;
   {
      frame = new QFrame( m_tabsManager );
      frame->setObjectName( topObjectName );
      layout = new QVBoxLayout( frame );
      frame->setLayout( layout );
      layout->setSpacing(0);
      layout->setMargin(0);
      m_tabsManager->addTab( frame, "Bookmarks" );
   }

   // bookmarks list
   {
      m_bookmarks = new QListWidget( m_tabsManager );
      m_bookmarks->setContextMenuPolicy( Qt::CustomContextMenu );
      m_bookmarks->setObjectName( topObjectName + "/m_bookmarks" );
      layout->addWidget( m_bookmarks );

      connect( m_bookmarks, SIGNAL( itemDoubleClicked( QListWidgetItem* ) ), this, SLOT( onFocusOnFile( QListWidgetItem* ) ) );
      connect( m_bookmarks, SIGNAL( customContextMenuRequested( const QPoint& ) ), this, SLOT( showBookmarksPopupMenu( const QPoint& ) ) );
   }
}

///////////////////////////////////////////////////////////////////////////////

void ResourcesBrowser::setupFileFinder()
{
   QString topObjectName( "ResourcesBrowser/fileFinderFrame" );

   // frame
   QFrame* frame = NULL;
   QVBoxLayout* layout = NULL;
   {
      frame = new QFrame( m_tabsManager );
      frame->setObjectName( topObjectName );
      layout = new QVBoxLayout( frame );
      frame->setLayout( layout );
      layout->setSpacing(0);
      layout->setMargin(0);
      m_tabsManager->addTab( frame, "Search" );
   }

   // toolbar
   {
      QToolBar* toolbar = new QToolBar( m_tabsManager );
      toolbar->setObjectName( topObjectName + "/toolbar" );
      layout->addWidget( toolbar );

      // find file
      {
         m_findFile = new QAction( tr( "Find file" ), toolbar );
         m_findFile->setIcon( QIcon( ":/TamyEditor/Resources/Icons/Editor/search.png" ) );
         toolbar->addAction( m_findFile );
         connect( m_findFile, SIGNAL( triggered() ), this, SLOT( onFindFile() ) );

         m_searchedFileName = new QLineEdit( toolbar );
         toolbar->addWidget( m_searchedFileName );
      }
   }

   // results list
   {
      m_searchResults = new QListWidget( m_tabsManager );
      m_searchResults->setObjectName( topObjectName + "/m_searchResults" );
      layout->addWidget( m_searchResults );

      connect( m_searchResults, SIGNAL( itemDoubleClicked( QListWidgetItem* ) ), this, SLOT( onFocusOnFile( QListWidgetItem* ) ) );

   }
}

///////////////////////////////////////////////////////////////////////////////

void ResourcesBrowser::showProjectBrowserPopupMenu( QTreeWidgetItem* node, QMenu& menu )
{
   ResourcesManager& resMgr = TSingleton< ResourcesManager >::getInstance();

   // create additional save actions, providing the item we clicked
   // is a loaded resource
   FSLeafNode* nodeItem = dynamic_cast< FSLeafNode* >( node );
   if ( nodeItem )
   {
      Resource* resource = resMgr.findResource( nodeItem->getRelativePath() );
      if ( resource != NULL )
      {
         menu.addSeparator();

         QAction* saveNodeAction = new SaveResourceAction( QIcon( ":/TamyEditor/Resources/Icons/Editor/saveFile.png" ), "Save", this, *resource, false );
         menu.addAction( saveNodeAction );

         QAction* saveHierarchyAction = new SaveResourceAction( QIcon( ":/TamyEditor/Resources/Icons/Editor/saveFile.png" ), "Save All", this, *resource, true );
         menu.addAction( saveHierarchyAction );
      }
   }

   // create additional save actions providing the item we clicked
   // is a directory 
   FSDirNode* dirNode = dynamic_cast< FSDirNode* >( node );
   if ( dirNode )
   {
      menu.addSeparator();

      QAction* addBookmarkAction = new AddBookmarkAction( QIcon( ":/TamyEditor/Resources/Icons/Editor/addBookmark.png" ), "Add bookmark", this, FilePath( dirNode->getRelativePath() ), *this );
      menu.addAction( addBookmarkAction );

      if ( m_activeProject != NULL )
      {
         menu.addSeparator();

         // we might also want ot remove directories from a project
         FilePath dirPath( dirNode->getRelativePath() );
         QAction* removeProjectDir = new ManageProjectDirAction( QIcon( ":/TamyEditor/Resources/Icons/Editor/removeProjectDir.png" ), "Remove from project", this, m_activeProject, dirPath, false );
         menu.addAction( removeProjectDir );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void ResourcesBrowser::showFilesystemBrowserPopupMenu( QTreeWidgetItem* node, QMenu& menu )
{
   // create additional save actions providing the item we clicked is a directory 
   FSDirNode* dirNode = dynamic_cast< FSDirNode* >( node );
   if ( dirNode )
   {
      menu.addSeparator();

      QAction* addBookmarkAction = new AddBookmarkAction( QIcon( ":/TamyEditor/Resources/Icons/Editor/addBookmark.png" ), "Add bookmark", this, FilePath( dirNode->getRelativePath() ), *this );
      menu.addAction( addBookmarkAction );

      if ( m_activeProject != NULL )
      {
         menu.addSeparator();

         // if there's an active project, we might want to add or remove directories to/from it
         FilePath dirPath( dirNode->getRelativePath() );
         bool isMember = m_activeProject->isMember( dirPath );
         if ( !isMember )
         {
            QAction* addProjectDir = new ManageProjectDirAction( QIcon( ":/TamyEditor/Resources/Icons/Editor/addProjectDir.png" ), "Add to project", this, m_activeProject, dirPath, true );
            menu.addAction( addProjectDir );
         }
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void ResourcesBrowser::setActiveProject( Project* project )
{
   if ( m_activeProject == project )
   {
      // nothing happens
      return;
   }

   m_activeProject = project;

   // switch the data source to the one provided by the new project
   FilesystemSection* projectFilesystemSection = NULL;
   if ( m_activeProject )
   {
      projectFilesystemSection = &m_activeProject->getFilesystemSection();
   }
   m_projectBrowserTree->setSource( projectFilesystemSection );

   // and finally switch to a proper tab
   m_tabsManager->setCurrentIndex( m_activeProject ? TI_Project : TI_Filesystem );
}

///////////////////////////////////////////////////////////////////////////////

void ResourcesBrowser::addBookmark( const FilePath& relativePath )
{
   QListWidgetItem* item = new QListWidgetItem();

   std::string nodeName = relativePath.extractNodeName();
   item->setText( nodeName.c_str() );
   item->setData( BOOKMARK_FILEPATH_DATA_INDEX, relativePath.c_str() );

   m_bookmarks->addItem( item );

   // activate the bookmarks tab
   m_tabsManager->setCurrentIndex( TI_Bookmarks );
}

///////////////////////////////////////////////////////////////////////////////

void ResourcesBrowser::showBookmarksPopupMenu( const QPoint& pos )
{
   QListWidgetItem* clickedItem = m_bookmarks->itemAt( pos );
   if ( !clickedItem )
   {
      // no item was clicked - ignore
      return;
   }

   // create the menu
   QMenu* popupMenu = new QMenu( this );
   {
      QAction* removeBookmarkAction = new RemoveBookmarkAction( QIcon( ":/TamyEditor/Resources/Icons/Editor/removeBookmark.png" ), "Remove", this, m_bookmarks, clickedItem );
      connect( removeBookmarkAction, SIGNAL( triggered() ), this, SLOT( removeBookmark() ) );
      popupMenu->addAction( removeBookmarkAction );
   }

   // display the menu
   popupMenu->popup( mapToGlobal( pos ) );
}

///////////////////////////////////////////////////////////////////////////////

void ResourcesBrowser::onFocusOnFile( QListWidgetItem* item )
{
   std::string pathStr = item->data( BOOKMARK_FILEPATH_DATA_INDEX ).toString().toStdString();
   FilePath path( pathStr );

   // try opening the file in the filesystem browser
   {
      FSTreeNode* node = m_filesystemBrowserTree->open( path );
      if ( node )
      {
         m_filesystemBrowserTree->focusOn( node );
         m_tabsManager->setCurrentIndex( TI_Filesystem );
      }
   }

   // and now try opening it in the project browser. If we find it, we'll focus on the file there,
   // thus overriding the previous focus - which is more desirable, since we always consider project
   // items more important than filesystem items in terms of our work flow with the ResourcesBrowser
   {
      FSTreeNode* node = m_projectBrowserTree->open( path );
      if ( node )
      {
         m_projectBrowserTree->focusOn( node );
         m_tabsManager->setCurrentIndex( TI_Project );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void ResourcesBrowser::onFindFile()
{
   struct SearchListened : public FilesystemScanner
   {
      std::vector< FilePath >       m_paths;
      std::string                   m_filter;

      SearchListened( const std::string& filter )
         : m_filter( filter )
      {
         StringUtils::toUpper( m_filter);
      }

      void onDirectory( const FilePath& name ) override
      {
         std::string relPath = name.getRelativePath();
         StringUtils::toUpper( relPath );
         if ( relPath.find( m_filter ) != std::string::npos )
         {
            m_paths.push_back( name );
         }
      }

      void onFile( const FilePath& name ) override
      {
         std::string relPath = name.getRelativePath();
         StringUtils::toUpper( relPath );
         if ( relPath.find( m_filter ) != std::string::npos )
         {
            m_paths.push_back( name );
         }
      }
   };

   // clear previous search results
   m_searchResults->clear();

   // start the search
   std::string filter = m_searchedFileName->text().toStdString();
   SearchListened scanner( filter );

   ResourcesManager& resMgr = TSingleton< ResourcesManager >::getInstance();
   Filesystem& fs = resMgr.getFilesystem();
   FilePath searchRootDir( fs.getCurrRoot() );
   resMgr.getFilesystem().scan( searchRootDir, scanner, true );

   // present the results
   uint count = scanner.m_paths.size();
   for ( uint i = 0; i < count; ++i )
   {
      QListWidgetItem* item = new QListWidgetItem();
      item->setText( scanner.m_paths[i].c_str() );
      item->setData( BOOKMARK_FILEPATH_DATA_INDEX, scanner.m_paths[i].c_str() );

      m_searchResults->addItem( item );
   }
}

///////////////////////////////////////////////////////////////////////////////

void ResourcesBrowser::saveLayout( QSettings& settings )
{
   // begin serialization
   settings.beginGroup( "ResourcesBrowser/layoutSettings" );

   // serialize the list of bookmarks
   {
      QString bookmarks;

      uint bookmarksCount = m_bookmarks->count();
      for ( uint i = 0; i < bookmarksCount; ++i )
      {
          bookmarks += ";" + m_bookmarks->item( i )->data( BOOKMARK_FILEPATH_DATA_INDEX ).toString();
      }

      settings.setValue( "bookmarks", bookmarks );
   }


   // finish serialization
   settings.endGroup();
}

///////////////////////////////////////////////////////////////////////////////

void ResourcesBrowser::loadLayout( QSettings& settings )
{
   // begin serialization
   settings.beginGroup( "ResourcesBrowser/layoutSettings" );

   // deserialize the list of bookmarks
   {
      std::string bookmarks = settings.value( "bookmarks" ).toString().toStdString();
      Array< std::string > arrBookmarks;
      StringUtils::tokenize( bookmarks, ";", arrBookmarks );

      uint count = arrBookmarks.size();
      for ( uint i = 0; i < count; ++i )
      {
         addBookmark( FilePath( arrBookmarks[i] ) );
      }
   }

   // finish serialization
   settings.endGroup();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

SaveResourceAction::SaveResourceAction( const QIcon& icon, const char* name, QObject* parent, Resource& resource, bool recursive )
   : QAction( icon, name, parent )
   , m_resource( resource )
   , m_recursive( recursive )
{
   connect( this, SIGNAL( triggered() ), this, SLOT( onTriggered() ) );
}

///////////////////////////////////////////////////////////////////////////////

void SaveResourceAction::onTriggered()
{
   m_resource.saveResource();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

AddBookmarkAction::AddBookmarkAction( const QIcon& icon, const char* name, QObject* parent, const FilePath& relativePath, ResourcesBrowser& browser )
   : QAction( icon, name, parent )
   , m_relativePath( relativePath )
   , m_browser( browser )
{
   connect( this, SIGNAL( triggered() ), this, SLOT( onTriggered() ) );
}

///////////////////////////////////////////////////////////////////////////////

void AddBookmarkAction::onTriggered()
{
   m_browser.addBookmark( m_relativePath );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

RemoveBookmarkAction::RemoveBookmarkAction( const QIcon& icon, const char* name, QObject* parent, QListWidget* bookmarksList, QListWidgetItem* removedItem )
   : QAction( icon, name, parent )
   , m_bookmarksList( bookmarksList )
   , m_removedItem( removedItem )
{
   connect( this, SIGNAL( triggered() ), this, SLOT( onTriggered() ) );
}

///////////////////////////////////////////////////////////////////////////////

void RemoveBookmarkAction::onTriggered()
{
   int itemIdx = m_bookmarksList->row( m_removedItem );
   QListWidgetItem* item = m_bookmarksList->takeItem( itemIdx );
   delete item;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

ManageProjectDirAction::ManageProjectDirAction( const QIcon& icon, const char* name, QObject* parent, Project* project, const FilePath& relativePath, bool add )
   : QAction( icon, name, parent )
   , m_project( project )
   , m_relativePath( relativePath )
   , m_add( add )
{
   connect( this, SIGNAL( triggered() ), this, SLOT( onTriggered() ) );

}

///////////////////////////////////////////////////////////////////////////////

void ManageProjectDirAction::onTriggered()
{  
   if ( m_add )
   {
      m_project->addDirectory( m_relativePath );
   }
   else
   {
      m_project->removeDirectory( m_relativePath );
   }

   // any change to the project needs to be saved
   m_project->saveResource();
}

///////////////////////////////////////////////////////////////////////////////

