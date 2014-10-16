/// @file   TamyEditor\ResourcesBrowser.h
/// @brief  viewer allowing to browse through resources
#pragma once

#include <QtWidgets\QDockWidget>
#include <QObject>
#include <QtWidgets\QTreeWidgetItem>
#include <QPoint>
#include <QtWidgets\QAction>
#include "SerializableWidget.h"
#include "core/FilePath.h"
#include "core/Filesystem.h"


///////////////////////////////////////////////////////////////////////////////

class QPushButton;
class QTabWidget;
class QListWidget;
class QListWidgetItem;
class QSettings;
class QLineEdit;
class FilesystemTree;
class Resource;
class Project;
class FilesystemSection;

///////////////////////////////////////////////////////////////////////////////

class ResourcesBrowser : public QDockWidget, public SerializableWidget
{
   Q_OBJECT

private:
   enum TaIndex
   {
      TI_Project     = 0,
      TI_Filesystem  = 1,
      TI_Bookmarks   = 2,
      TI_Search      = 3,
   };

private:
   QTabWidget*                   m_tabsManager;
   
   // filesystem browser
   FilesystemTree*               m_filesystemBrowserTree;

   // project browser
   Project*                      m_activeProject;
   FilesystemTree*               m_projectBrowserTree;

   // bookmarks
   QListWidget*                  m_bookmarks;

   // find file action
   QAction*                      m_findFile;
   QLineEdit*                    m_searchedFileName;
   QListWidget*                  m_searchResults;

public:
   /**
    * Constructor.
    *
    * @param parentWidget
    */
   ResourcesBrowser( QWidget* parentWidget );
   ~ResourcesBrowser();

   /**
    * Adds a new bookmark.
    *
    * @param relativePath
    */
   void addBookmark( const FilePath& relativePath );

   /**
    * Activates the specified project.
    *
    * @param project
    */
   void setActiveProject( Project* project );

   // -------------------------------------------------------------------------
   // SerializableWidget implementation
   // -------------------------------------------------------------------------
   void saveLayout( QSettings& settings );
   void loadLayout( QSettings& settings );

public slots:
   void showProjectBrowserPopupMenu( QTreeWidgetItem* node, QMenu& menu );
   void showFilesystemBrowserPopupMenu( QTreeWidgetItem* node, QMenu& menu );
   void onFocusOnFile( QListWidgetItem* item );
   void onFindFile();
   void showBookmarksPopupMenu( const QPoint& pt );

private:
   void initUI();

   // -------------------------------------------------------------------------
   // layout setup helper methods
   // -------------------------------------------------------------------------
   void setupFilesystemTree();
   void setupProjectTree();
   void setupBookmarks();
   void setupFileFinder();
};

///////////////////////////////////////////////////////////////////////////////

/**
 * An action that saves the selected resource.
 */
class SaveResourceAction : public QAction
{
   Q_OBJECT

private:
   Resource&               m_resource;
   bool                    m_recursive;

public:
   /**
    * Constructor.
    *
    * @param icon
    * @param name
    * @param parent
    * @param resource
    * @param recursive     should all dependencies be saved as well?
    */
   SaveResourceAction( const QIcon& icon, const char* name, QObject* parent, Resource& resource, bool recursive );

public slots:
   void onTriggered();
};

///////////////////////////////////////////////////////////////////////////////

class AddBookmarkAction : public QAction
{
   Q_OBJECT

private:
   FilePath                m_relativePath;
   ResourcesBrowser&       m_browser;

public:
   /**
    * Constructor.
    *
    * @param icon
    * @param name
    * @param parent
    * @param relativePath
    * @param browser
    */
   AddBookmarkAction( const QIcon& icon, const char* name, QObject* parent, const FilePath& relativePath, ResourcesBrowser& browser );

public slots:
   void onTriggered();
};

///////////////////////////////////////////////////////////////////////////////

class RemoveBookmarkAction : public QAction
{
   Q_OBJECT

private:
   QListWidget*           m_bookmarksList;
   QListWidgetItem*       m_removedItem;

public:
   /**
    * Constructor.
    *
    * @param icon
    * @param name
    * @param parent
    * @param bookmarksList
    * @param removedItem
    */
   RemoveBookmarkAction( const QIcon& icon, const char* name, QObject* parent, QListWidget* bookmarksList, QListWidgetItem* removedItem );

public slots:
   void onTriggered();
};

///////////////////////////////////////////////////////////////////////////////

class ManageProjectDirAction : public QAction
{
   Q_OBJECT

private:
   Project*                m_project;
   FilePath                m_relativePath;
   bool                    m_add;

public:
   /**
    * Constructor.
    *
    * @param icon
    * @param name
    * @param parent
    * @param project
    * @param relativePath
    * @param add              'true' == add, 'false' == remove
    */
   ManageProjectDirAction( const QIcon& icon, const char* name, QObject* parent, Project* project, const FilePath& relativePath, bool add );

public slots:
   void onTriggered();
};

///////////////////////////////////////////////////////////////////////////////
