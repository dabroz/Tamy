#include "FilesystemTree.h"
#include "core.h"

// widgets
#include <QtWidgets\QVBoxLayout>
#include <QtWidgets\QToolbar>
#include <QtWidgets\QInputDialog>
#include <QtGui\QKeyEvent>

// tree widget
#include "FSNodeMimeData.h"
#include "ResourceMimeData.h"
#include <QtWidgets\QTreeWidgetItem>
#include "MenuUtils.h"

// nodes
#include "FSTreeNode.h"
#include "FSRootNode.h"
#include "FSDirNode.h"
#include "FSLeafNode.h"
#include "FSTreeItemsFactory.h"

// tools
#include "ResourceManagementUtil.h"
#include <QtCore\QSettings>
#include "Project.h"
#include "AdditionalTreeItemFactories.h"


///////////////////////////////////////////////////////////////////////////////

FilesystemTree::FilesystemTree( QWidget* parentWidget, const char* objectName, const char* rootDirectory )
   : QFrame( parentWidget )
   , m_fsTree( NULL )
   , m_rootDir( NULL )
   , m_itemsFactory( NULL )
   , m_section( NULL )
{
   // give the frame a meaningful name so that it's taken into consideration during UI serialization
   setObjectName( objectName );

   // setup the frame layout
   QVBoxLayout* layout = new QVBoxLayout( this );
   layout->setSpacing(0);
   layout->setMargin(0);
   setLayout( layout );

   // setup the items factory
   m_itemsFactory = new FSTreeItemsFactory();

   // setup the widgets
   initialize( objectName, rootDirectory );

   // pull the structure of the filesystem
   Filesystem& fs = TSingleton< Filesystem >::getInstance();
   fs.attach( *this );
   fs.pullStructure( *this, m_rootDir->getRelativePath() );
}

///////////////////////////////////////////////////////////////////////////////

FilesystemTree::~FilesystemTree()
{
   Filesystem& fs = TSingleton< Filesystem >::getInstance();
   fs.detach( *this );

   delete m_itemsFactory;
   m_itemsFactory = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void FilesystemTree::initialize( const QString& topObjectName, const char* rootDirectory )
{
   // toolbar
   {
      QToolBar* toolbar = new QToolBar( this );
      toolbar->setObjectName( topObjectName + "/toolbar" );
      layout()->addWidget( toolbar );
   }

   // filesystem tree
   {
      m_fsTree = new FSTreeWidget( this, topObjectName + "/m_fsTree" );
      layout()->addWidget( m_fsTree );

      connect( m_fsTree, SIGNAL( itemDoubleClicked( QTreeWidgetItem*, int ) ), this, SLOT( onItemDoubleClicked( QTreeWidgetItem*, int ) ) );
      connect( m_fsTree, SIGNAL( removeNode( QTreeWidgetItem*, QTreeWidgetItem* ) ), this, SLOT( onRemoveNode( QTreeWidgetItem*, QTreeWidgetItem* ) ) );
      connect( m_fsTree, SIGNAL( clearNode( QTreeWidgetItem* ) ), this, SLOT( onClearNode( QTreeWidgetItem* ) ) );
      connect( m_fsTree, SIGNAL( renameNode( QTreeWidgetItem*, const QString& ) ), this, SLOT( onNodeRenamed( QTreeWidgetItem*, const QString& ) ) );
      connect( m_fsTree, SIGNAL( popupMenuShown( QTreeWidgetItem*, QMenu& ) ), this, SLOT( onPopupMenuShown( QTreeWidgetItem*, QMenu& ) ) );

      ResourcesManager& rm = TSingleton< ResourcesManager >::getInstance();

      m_rootDir = new FSRootNode( m_fsTree, rootDirectory );
      m_fsTree->addTopLevelItem( m_rootDir );

      m_rootDir->setExpanded( true );
   }
}

///////////////////////////////////////////////////////////////////////////////

void FilesystemTree::setSource( FilesystemSection* section )
{
   if ( m_section == section )
   {
      // it's the same section, nothing changes
      return;
   }

   // detach self as a listener from the previous section
   Filesystem& fs = TSingleton< Filesystem >::getInstance();
   if ( m_section )
   {
      m_section->detach( *this );
   }
   else
   {
      fs.detach( *this );
   }

   // clear tree contents
   m_rootDir->clear();

   // set the new section
   m_section = section;

   // attach self as a listener from the previous section
   if ( m_section )
   {
      m_section->attach( *this );
      m_section->pullStructure( *this, m_rootDir->getRelativePath() );
   }
   else
   {
      fs.attach( *this );
      fs.pullStructure( *this, m_rootDir->getRelativePath() );
   }
}

///////////////////////////////////////////////////////////////////////////////

void FilesystemTree::onDirAdded( const FilePath& dir )
{
   addDirectory( dir );
}

///////////////////////////////////////////////////////////////////////////////

void FilesystemTree::onDirRemoved( const FilePath& dir )
{
   removeDirectory( dir );
}

///////////////////////////////////////////////////////////////////////////////

void FilesystemTree::onFileAdded( const FilePath& path )
{
   FilePath parentDir;
   path.extractDir( parentDir );
   FSTreeNode* parentDirNode = find( parentDir );
   if ( parentDirNode )
   {
      FSLeafNode* newFileNode = new FSLeafNode( parentDirNode, path.extractNodeName(), *m_itemsFactory );
      parentDirNode->addChild( newFileNode );
   }
}

///////////////////////////////////////////////////////////////////////////////

void FilesystemTree::onFileEdited( const FilePath& path )
{
   // do nothing
}

///////////////////////////////////////////////////////////////////////////////

void FilesystemTree::onFileRemoved( const FilePath& path )
{
   FSTreeNode* fileNode = find( path );
   if ( fileNode )
   {
      FSTreeNode* parentNode = static_cast< FSTreeNode* >( fileNode->parent() );
      parentNode->removeChild( fileNode );
   }
}

///////////////////////////////////////////////////////////////////////////////

void FilesystemTree::onFileRenamed( const FilePath& oldPath, const FilePath& newPath )
{
   onFileRemoved( oldPath );
   onFileAdded( newPath );
}

///////////////////////////////////////////////////////////////////////////////

void FilesystemTree::onDirRenamed( const FilePath& oldPath, const FilePath& newPath )
{
   removeDirectory( oldPath );
   addDirectory( newPath );
}

///////////////////////////////////////////////////////////////////////////////

void FilesystemTree::onFilesystemDeleted( Filesystem& filesystem )
{
   // nothing to do here
}

///////////////////////////////////////////////////////////////////////////////

bool FilesystemTree::getRootRelativePath( const FilePath& inPath, FilePath& outPath ) const
{
   // remove the part that corresponds to the name of the root
   std::string inPathStr = inPath.getRelativePath();
   const int rootPartEndOffset = inPathStr.find( m_rootDir->getRelativePath() );
   if ( rootPartEndOffset == std::string::npos )
   {
      // this file is located outside of our root directory
      return false;
   }

   ASSERT( rootPartEndOffset == 0 );
   std::string rootRelativePath = inPathStr.substr( m_rootDir->getRelativePath().getRelativePath().length() );

   outPath.set( rootRelativePath );
   return true;
}

///////////////////////////////////////////////////////////////////////////////

void FilesystemTree::addDirectory( const FilePath& dir )
{
   FilePath rootRelativePath;
   if ( !getRootRelativePath( dir, rootRelativePath ) )
   {
      return;
   }

   // add the path to it - piece by piece
   Array< std::string > pathElements;
   StringUtils::tokenize( rootRelativePath.c_str(), "/\\", pathElements );
   uint count = pathElements.size();

   FSTreeNode* currNode = m_rootDir;
   for ( uint i = 0; i < count; ++i )
   {
      FSTreeNode* childNode = currNode->find( pathElements[i] );
      if ( childNode )
      {
         // the node exists - follow it deeper
         currNode = childNode;
      }
      else
      {
         // the node doesn't exist - create a new one
         currNode = new FSDirNode( currNode, pathElements[i] + "/" );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void FilesystemTree::removeDirectory( const FilePath& dir )
{
   // find a node corresponding to the specified path
   FSTreeNode* node = find( dir );
   if ( !node )
   {
      return;
   }

   QTreeWidgetItem* parentItem = node->parent();
   if ( parentItem )
   {
      int childIdx = parentItem->indexOfChild( node );
      parentItem->takeChild( childIdx );
   }
}

///////////////////////////////////////////////////////////////////////////////

void FilesystemTree::refreshRecursive( const std::string& rootDir ) 
{
   FSTreeNode* rootDirNode = find( rootDir );
   if ( !rootDirNode )
   {
      return;
   }

   // clear the directory
   rootDirNode->clear();

   // attach self as a listener from the previous section
   if ( m_section )
   {
      m_section->pullStructure( *this, m_rootDir->getRelativePath() );
   }
   else
   {
      Filesystem& fs = TSingleton< Filesystem >::getInstance();
      fs.pullStructure( *this, m_rootDir->getRelativePath() );
   }
}

///////////////////////////////////////////////////////////////////////////////

FSTreeNode* FilesystemTree::find( const std::string& dir )
{
   FilePath rootRelativePath;
   if ( !getRootRelativePath( FilePath( dir ), rootRelativePath ) )
   {
      return NULL;
   }

   // split the path int particular elements
   Array< std::string > pathParts;
   rootRelativePath.getElements( pathParts );

   // if there were no elements, simply return the root node
   if ( pathParts.empty() )
   {
      return m_rootDir;
   }

   // find the node
   FSTreeNode* currItem = m_rootDir;
   unsigned int count = pathParts.size();
   for ( unsigned int i = 0; i < count; ++i )
   {
      std::string currPathPart = pathParts[i];
      FSTreeNode* nextItem = currItem->find( currPathPart );
      if ( !nextItem )
      {
         // that's the last leaf - there's nothing else to search in - bail
         break;
      }

      if ( i + 1 == count )
      {
         // we found it
         return nextItem;
      }

      // keep on looking starting from the node we've just analyzed
      currItem = nextItem;
   }

   // the node hasn't been mapped yet
   return NULL;
}

///////////////////////////////////////////////////////////////////////////////

FSTreeNode* FilesystemTree::open( const FilePath& path )
{
   ResourcesManager& resMgr = TSingleton< ResourcesManager >::getInstance();

   FilePath rootRelativePath;
   if ( !getRootRelativePath( path, rootRelativePath ) )
   {
      return NULL;
   }
   Array< std::string > pathParts;
   rootRelativePath.getElements( pathParts );

   // if there were no elements, simply return the root node
   if ( pathParts.empty() )
   {
      return m_rootDir;
   }

   // start mapping the nodes
   std::string currRelativePath = "/";

   FSTreeNode* currItem = m_rootDir;
   unsigned int count = pathParts.size();

   FSTreeNode* foundItem = NULL;
   for ( unsigned int i = 0; i < count; ++i )
   {
      std::string currPathPart = pathParts[i];
      FSTreeNode* nextItem = currItem->find( currPathPart );
      if ( !nextItem )
      {
         // and try finding the corresponding node again
         nextItem = currItem->find( currPathPart );
      }

      if ( !nextItem )
      {
         // still nothing - this means that the path we're looking for doesn't exist - so bail
         break;
      }

      if ( i + 1 == count )
      {
         // we found it
         foundItem = nextItem;
         break;
      }

      // keep on looking starting from the node we've just analyzed
      currRelativePath += currPathPart;
      currItem = nextItem;
   }

   if ( foundItem )
   {
      if ( dynamic_cast< FSDirNode* >( foundItem ) )
      {
         foundItem->setExpanded( true );
      }
   }

   // the path doesn't exist
   return foundItem;
}

///////////////////////////////////////////////////////////////////////////////

void FilesystemTree::focusOn( FSTreeNode* node )
{
   m_fsTree->setCurrentItem( node );
}

///////////////////////////////////////////////////////////////////////////////

void FilesystemTree::addNode( unsigned int idx, const std::string& parentDir )
{
   switch( idx )
   {
      case FS_ITEM_DIR:
      {
         // learn the new file's name
         bool ok = false;
         QString newDirName = QInputDialog::getText( this, "New directory", "Dir name:", QLineEdit::Normal, "", &ok );
         if ( ok )
         {
            const Filesystem& fs = TSingleton< ResourcesManager >::getInstance().getFilesystem();
            fs.mkdir( parentDir + newDirName.toStdString() );
         }

         break;
      }
   
      case FS_ITEM_PROJECT:
      {
         // learn the new file's name
         bool ok = false;
         QString newProjectName = QInputDialog::getText( this, "New project", "Project name:", QLineEdit::Normal, "", &ok );
         if ( ok )
         {
            // create a project directory
            ResourcesManager& resMgr = TSingleton< ResourcesManager >::getInstance();
            const Filesystem& fs = resMgr.getFilesystem();

            FilePath projectDir( parentDir + newProjectName.toStdString() );
            fs.mkdir( projectDir );

            // and place a new project resource there
            std::string projectPath = projectDir.c_str();
            projectPath += "/";
            projectPath += newProjectName.toStdString() + "." + Project::getExtension();
            resMgr.create< Project >( FilePath( projectPath ) );
         }

         break;
      }

      default:
      {
         // create the specified resource
         const SerializableReflectionType* newResourceType = m_itemsFactory->getClass( idx );
         Resource* newResource = newResourceType->instantiate<Resource>();

         ResourcesManager& resMgr = TSingleton< ResourcesManager >::getInstance();

         FilePath resourcePath;
         ResourceUtils::findAvailableDefaultName( resMgr.getFilesystem(), parentDir, newResource, resourcePath );
         newResource->setFilePath( resourcePath );
         
         resMgr.addResource( newResource );

         newResource->saveResource();

         break;
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void FilesystemTree::onRemoveNode( QTreeWidgetItem* parent, QTreeWidgetItem* child )
{
   FSTreeNode* parentItem = dynamic_cast< FSTreeNode* >( parent );
   FSTreeNode* childItem = dynamic_cast< FSTreeNode* >( child );
   parentItem->removeNode( childItem );
}

///////////////////////////////////////////////////////////////////////////////

void FilesystemTree::onClearNode( QTreeWidgetItem* item )
{
   FSTreeNode* node = dynamic_cast< FSTreeNode* >( item );
   node->clearNodes();
}

///////////////////////////////////////////////////////////////////////////////

void FilesystemTree::onItemDoubleClicked( QTreeWidgetItem* item, int column )
{
   FSTreeNode* entry = static_cast< FSTreeNode* >( item );
   entry->openItem( *this );
}

///////////////////////////////////////////////////////////////////////////////

void FilesystemTree::onNodeRenamed( QTreeWidgetItem* item, const QString& newName )
{
   FSTreeNode* node = dynamic_cast< FSTreeNode* >( item );
   node->setName( newName );
}

///////////////////////////////////////////////////////////////////////////////

void FilesystemTree::onPopupMenuShown( QTreeWidgetItem* node, QMenu& menu )
{
   class AddResourceActionFactory : public MenuActionsFactory
   {
   private:
      FilesystemTree*               m_parentTree;
      FSTreeNode*                   m_parentNode;

   public:
      AddResourceActionFactory( FilesystemTree* parentTree, FSTreeNode* parentNode )
         : m_parentTree( parentTree )
         , m_parentNode( parentNode )
      {}

      QAction* create( const QIcon& icon, const QString& desc, int itemTypeIdx, QWidget* parentWidget ) const
      {
         QAction* action = new AddFilesystemResourceAction( icon, desc, itemTypeIdx, m_parentTree, m_parentNode );
         return action;
      }
   };

   // attach the entities addition sub-menu
   FSTreeNode* parentNode = static_cast< FSTreeNode* >( node );
   TreeWidgetDescFactory* itemsFactory = parentNode->getDescFactory( *this );

   AddResourceActionFactory actionsFactory( this, parentNode );
   MenuUtils::itemsListMenu( itemsFactory, actionsFactory, &menu );

   emit popupMenuRequest( node, menu );
}

///////////////////////////////////////////////////////////////////////////////

void FilesystemTree::saveLayout( QSettings& settings )
{
   // begin serialization
   settings.beginGroup( objectName() + "/layoutSettings" );

   // save all open directories
   {
      QString openDirectories;

      std::list< FSTreeNode* > nodesToExplore;
      nodesToExplore.push_back( m_rootDir );
      while ( !nodesToExplore.empty() )
      {
         FSTreeNode* currNode = nodesToExplore.front(); nodesToExplore.pop_front();
         if ( currNode->isExpanded() )
         {
            // memorize only expanded nodes
            openDirectories += ";" + QString( currNode->getRelativePath().c_str() );
         }

         uint childrenCount = currNode->childCount();
         for ( uint i = 0; i < childrenCount; ++i )
         {
            FSDirNode* childDirNode = dynamic_cast< FSDirNode* >( currNode->child( i ) );
            if ( childDirNode )
            {
               nodesToExplore.push_back( childDirNode );
            }
         }
      }

      settings.setValue( "openDirectories", openDirectories );
   }

   // finish serialization
   settings.endGroup();
}

///////////////////////////////////////////////////////////////////////////////

void FilesystemTree::loadLayout( QSettings& settings )
{
   // begin serialization
   settings.beginGroup( objectName() + "/layoutSettings" );

   // deserialize all open directories
   {
      std::string openDirectories = settings.value( "openDirectories" ).toString().toStdString();

      Array< std::string > arrOpenDirectories;
      StringUtils::tokenize( openDirectories, ";", arrOpenDirectories );
      uint count = arrOpenDirectories.size();
      for ( uint i = 0; i < count; ++i )
      {
         open( arrOpenDirectories[i] );
      }
   }

   // finish serialization
   settings.endGroup();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FSTreeWidget::FSTreeWidget( QWidget* parent, const QString& objName )
   : TreeWidget( parent, objName )
{
   QStringList columnLabels;
   columnLabels.push_back( "Name" );
   columnLabels.push_back( "Size" );

   setColumnCount( columnLabels.size() );
   setHeaderLabels( columnLabels );

   setDragEnabled( true );
   setDropIndicatorShown( true );
   setDefaultDropAction( Qt::IgnoreAction );
   setSelectionMode( QAbstractItemView::SingleSelection );
   setDragDropMode( QAbstractItemView::DragDrop );
}

///////////////////////////////////////////////////////////////////////////////

QMimeData* FSTreeWidget::mimeData( const QList<QTreeWidgetItem *> items ) const
{
   QMimeData* data = __super::mimeData( items );

   // save filesystem paths
   {
      Array< FilePath > paths;
      unsigned int count = items.size();
      for ( unsigned int i = 0; i < count; ++i )
      {
         FSTreeNode* node = static_cast< FSTreeNode* >( items[ i ] );
         if ( node )
         {
            paths.push_back( node->getRelativePath() );
         }
      }

      FSNodeMimeData::save( paths, *data );
   }

   // save resource pointers
   {
      ResourcesManager& resMgr = TSingleton< ResourcesManager >::getInstance();

      unsigned int count = items.size();
      for ( unsigned int i = 0; i < count; ++i )
      {
         FSTreeNode* node = static_cast< FSTreeNode* >( items[ i ] );
         if ( node )
         {
            Resource* resource = resMgr.create( node->getRelativePath() );
            ResourceMimeData::save( resource, *data );
         }
      }
   }

   return data;
}

///////////////////////////////////////////////////////////////////////////////

void FSTreeWidget::dragMoveEvent( QDragMoveEvent* event )
{
   const QMimeData* data = event->mimeData();
 
   if ( FSNodeMimeData::containsRelevantData( *data ) || ResourceMimeData::containsRelevantData< Resource >( *data ) )
   {
      __super::dragMoveEvent( event );
   }
   else
   {
      event->ignore();
   }
}

///////////////////////////////////////////////////////////////////////////////

void FSTreeWidget::dropEvent( QDropEvent* event )
{
   const QMimeData* data = event->mimeData();
   Filesystem& fs = TSingleton< Filesystem >::getInstance();

   Array< FilePath > paths;
   if ( FSNodeMimeData::containsRelevantData( *data ) )
   {
      FSNodeMimeData::load( paths, *data );
   }

   if ( ResourceMimeData::containsRelevantData< Resource >( *data ) )
   {
      Resource* resource = NULL;
      ResourceMimeData::load( resource, *data );

      if ( resource )
      {
         paths.push_back( resource->getFilePath() );
      }
   }

   if ( paths.empty() )
   {
      event->ignore();
      return;
   }

   QPoint pos = event->pos();
   FSTreeNode* dropPlaceItem = static_cast< FSTreeNode* >( itemAt( pos.rx(), pos.ry() ) );
   if ( !dropPlaceItem )
   {
      dropPlaceItem = static_cast< FSTreeNode* >( topLevelItem( 0 ) );
   }

   // scan through the paths and remove the one that corresponds to the drop place item, if it's there,
   // 'cause we don't allow to drop items onto themselves
   const uint count = paths.size();
   for ( uint i = 0; i < count; ++i )
   {
      if ( paths[i] == dropPlaceItem->getRelativePath() )
      {
         paths.remove( i );
         break;
      }
   }

   // move the paths
   // find the target place and the insertion index
   FSTreeNode* targetItem = NULL;
   int insertionIndex = 0;
   {
      QAbstractItemView::DropIndicatorPosition dropPosition = dropIndicatorPosition();
      switch ( dropPosition )
      {
      case QAbstractItemView::OnItem:
      {
         if ( !dropPlaceItem->isContainerNode() )
         {
            // we're dropping it onto a leaf node - so acquire its parent
            targetItem = static_cast< FSTreeNode* >( dropPlaceItem->parent() );

            // and insert it behind it
            insertionIndex = targetItem->indexOfChild( dropPlaceItem ) + 1;
         }
         else
         {
            targetItem = dropPlaceItem;
            insertionIndex = targetItem->childCount();
         }

         break;
      }

      case QAbstractItemView::BelowItem:
      {
         targetItem = static_cast< FSTreeNode* >( dropPlaceItem->parent() );
         if ( targetItem == NULL )
         {
            targetItem = dropPlaceItem;
            insertionIndex = 0;
         }
         else
         {
            insertionIndex = targetItem->indexOfChild( dropPlaceItem ) + 1;
         }
         break;
      }

      case QAbstractItemView::AboveItem:
      {
         targetItem = static_cast< FSTreeNode* >( dropPlaceItem->parent() );
         if ( targetItem == NULL )
         {
            targetItem = dropPlaceItem;
            insertionIndex = targetItem->childCount();
         }
         else
         {
            insertionIndex = targetItem->indexOfChild( dropPlaceItem );
         }
         break;
      }

      case QAbstractItemView::OnViewport:
      {
         targetItem = static_cast< FSTreeNode* >( topLevelItem( 0 ) );
         insertionIndex = topLevelItemCount();
         break;
      }
      }
   }

   // insert the nodes into the specified node
   targetItem->insertNodes( insertionIndex, paths );

   // accept the event - the tree's gonna be rebuild either way when the nodes start moving around
   event->accept();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

AddFilesystemResourceAction::AddFilesystemResourceAction( const QIcon& icon, const QString& desc, unsigned int typeIdx, FilesystemTree* parentTree, FSTreeNode* parentNode )
   : QAction( icon, desc, parentTree )
   , m_parentTree( parentTree )
   , m_typeIdx( typeIdx )
   , m_parentNode( parentNode )
{
   connect( this, SIGNAL( triggered() ), this, SLOT( onTriggered() ) );
}

///////////////////////////////////////////////////////////////////////////////

void AddFilesystemResourceAction::onTriggered()
{
   m_parentNode->addNode( m_typeIdx, *m_parentTree );
}

///////////////////////////////////////////////////////////////////////////////
