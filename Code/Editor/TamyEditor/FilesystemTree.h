/// @file   TamyEditor/FilesystemTree.h
/// @brief  base for different types of filesystem trees
#pragma once

#include <QtWidgets\QFrame>
#include <QtWidgets\QTreeWidgetItem>
#include "TreeWidget.h"
#include "TreeWidgetDescFactory.h"
#include "core\Filesystem.h"
#include "TypeDescFactory.h"
#include "SerializableWidget.h"


///////////////////////////////////////////////////////////////////////////////

class TreeWidget;
class FSTreeNode;
class FSDirNode;
class FSRootNode;
class QAction;
class Resource;
class QSettings;

///////////////////////////////////////////////////////////////////////////////

/**
 * Base for different types of filesystem trees.
 *
 * Key map:
 *   - Ctrl+H renames an item
 *   - double clicking an item - if it's a resource, it will open it in a dedicated editor
 */
class FilesystemTree : public QFrame, public FilesystemListener, public SerializableWidget
{
   Q_OBJECT

private:
   FilesystemSection*            m_section;

   TreeWidget*                   m_fsTree;
   FSTreeNode*                   m_rootDir;

   // tools
   TreeWidgetDescFactory*        m_itemsFactory;

public:
   /**
    * Constructor.
    *
    * @param parentWidget
    * @parent objectName
    * @param rootDirectory
    */
   FilesystemTree( QWidget* parentWidget, const char* objectName, const char* rootDirectory = "/" );
   ~FilesystemTree();

   /**
    * Sets the active project section.
    *
    * @param section
    */
   void setSource( FilesystemSection* section );

   /**
    * Finds an existing node, providing that it's already been mapped.
    *
    * @param dir
    */
   FSTreeNode* find( const std::string& dir );

   /**
    * Opens a node ( whether it's already been mapped or not ), providing that it exists.
    * Otherwise it will finish at the last valid path element.
    *
    * @param path
    */
   FSTreeNode* open( const FilePath& path );

   /**
    * Focuses the browser on the specified tree node.
    *
    * @param node
    */
   void focusOn( FSTreeNode* node );

   /**
    * Adds the specified directory ( along with its immediate children, no recursive scan involved ) to the tree.
    *
    * @param dir
    */
   void addDirectory( const FilePath& dir );

   /**
    * Removes a directory and its contents from the tree.
    *
    * @param dir     directory path
    */
   void removeDirectory( const FilePath& dir );

   // -------------------------------------------------------------------------
   // SerializableWidget implementation
   // -------------------------------------------------------------------------
   void saveLayout( QSettings& settings );
   void loadLayout( QSettings& settings );

   // -------------------------------------------------------------------------
   // FilesystemListener implementation
   // -------------------------------------------------------------------------
   void onDirAdded( const FilePath& dir ) override;
   void onDirRemoved( const FilePath& dir ) override;
   void onDirRenamed( const FilePath& oldPath, const FilePath& newPath ) override;
   void onFileAdded( const FilePath& path ) override;
   void onFileEdited( const FilePath& path ) override;
   void onFileRemoved( const FilePath& path ) override;
   void onFileRenamed( const FilePath& oldPath, const FilePath& newPath ) override;
   void onFilesystemDeleted( Filesystem& filesystem ) override;

public slots:
   void onPopupMenuShown( QTreeWidgetItem* node, QMenu& menu );
   void onRemoveNode( QTreeWidgetItem* parent, QTreeWidgetItem* child );
   void onClearNode( QTreeWidgetItem* node );
   void onNodeRenamed( QTreeWidgetItem* node, const QString& newName );
   void onItemDoubleClicked( QTreeWidgetItem* item, int column );

signals:
   /**
    * Emitted when we need to populate a pop-up menu specific to the given node.
    *
    * @param node
    * @param menu
    */
   void popupMenuRequest( QTreeWidgetItem* node, QMenu& menu );

private:
   void initialize( const QString& objectName, const char* rootDirectory );

   /**
    * Recursively refreshes the contents of the tree, starting from the specified directory.
    *
    * @param rootDir
    */
   void refreshRecursive( const std::string& rootDir = "/" );

   /**
   * Modifies the specified path to be relative to the root of the tree.
   *
   * @param inPath
   * @param outPath
   *
   * @return 'true' if the path is ok for further processing, 'false' if it's located outside the current root dir
   */
   bool getRootRelativePath( const FilePath& inPath, FilePath& outPath ) const;

   // -------------------------------------------------------------------------
   // interface for the nodes
   // -------------------------------------------------------------------------

   friend class FSDirNode;
   friend class FSRootNode;
   inline TreeWidgetDescFactory* getDescFactory() { return m_itemsFactory; }
   void addNode( unsigned int idx, const std::string& parentDir );
};

///////////////////////////////////////////////////////////////////////////////

class FSTreeWidget : public TreeWidget
{
   Q_OBJECT

public:
   FSTreeWidget( QWidget* parent, const QString& objName );

   // -------------------------------------------------------------------------
   // QAbstractItemView implementation
   // -------------------------------------------------------------------------
   QMimeData* mimeData( const QList<QTreeWidgetItem *> items ) const;
   void dragMoveEvent( QDragMoveEvent* event );
   void dropEvent( QDropEvent* event );
};

///////////////////////////////////////////////////////////////////////////////

class AddFilesystemResourceAction : public QAction
{
   Q_OBJECT

private:
   FilesystemTree*               m_parentTree;
   FSTreeNode*                   m_parentNode;
   int                           m_typeIdx;

public:
   /**
    * Constructor.
    *
    * @param icon
    * @param desc
    * @param typeIdx
    * @param parentTree
    * @param parentNode
    */
   AddFilesystemResourceAction( const QIcon& icon, const QString& desc, unsigned int typeIdx, FilesystemTree* parentTree, FSTreeNode* parentNode );

public slots:
   void onTriggered();
};

///////////////////////////////////////////////////////////////////////////////
