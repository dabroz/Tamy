#include "TreeWidget.h"
#include <QtWidgets\QMenu>
#include <QtWidgets\QLineEdit>
#include "core\ResourcesManager.h"
#include "core\Filesystem.h"
#include "core\Assert.h"
#include "TreeWidgetDescFactory.h"


///////////////////////////////////////////////////////////////////////////////

TreeWidget::TreeWidget( QWidget* parent, const QString& objName )
   : QTreeWidget( parent )
   , m_popupMenuItem( NULL )
   , m_renamedItem( NULL )
   , m_renameEditor( NULL )
{
   setObjectName( objName );
   setContextMenuPolicy( Qt::CustomContextMenu );

   connect( this, SIGNAL( customContextMenuRequested( const QPoint& ) ), this, SLOT( showPopupMenu( const QPoint& ) ) );
}

///////////////////////////////////////////////////////////////////////////////

void TreeWidget::showPopupMenu( const QPoint& pos )
{
   m_popupMenuItem = itemAt( pos );
   if ( m_popupMenuItem )
   {
      QMenu* popupMenu = new QMenu( this );
      // add basic actions:

      // nodes collapsing and expansion - but these two work only
      // if a node has any children
      bool hasChildren = m_popupMenuItem->childCount() > 0;
      if ( hasChildren )
      {
         // expanding the entire node's hierarchy
         QAction* expandNodeAction = new QAction( "Expand All", this );
         connect( expandNodeAction, SIGNAL( triggered() ), this, SLOT( expandNode() ) );
         popupMenu->addAction( expandNodeAction );

         // ... and collapsing it ( for symmetry of operations )
         QAction* collapseNodeAction = new QAction( "Collapse All", this );
         connect( collapseNodeAction, SIGNAL( triggered() ), this, SLOT( collapseNode() ) );
         popupMenu->addAction( collapseNodeAction );

         popupMenu->addSeparator();
      }

      // renaming the node
      {
         QAction* renameNodeAction = new QAction( QIcon( ":/TamyEditor/Resources/Icons/Editor/rename.png" ), "Rename", this );
         connect( renameNodeAction, SIGNAL( triggered() ), this, SLOT( renameNode() ) );
         popupMenu->addAction( renameNodeAction );
      }


      // removing the node
      if ( m_popupMenuItem->parent() )
      {
         QAction* removeNodeAction = new QAction( QIcon( ":/TamyEditor/Resources/Icons/Editor/quit.png" ), "Remove", this );
         connect( removeNodeAction, SIGNAL( triggered() ), this, SLOT( removeNode() ) );
         popupMenu->addAction( removeNodeAction );
      }

      // clearing the node's contents
      if ( hasChildren )
      {
         QAction* clearNodeAction = new QAction( QIcon(":/TamyEditor/Resources/Icons/Editor/clear.png" ), "Clear", this );
         connect( clearNodeAction, SIGNAL( triggered() ), this, SLOT( clearNode() ) );
         popupMenu->addAction( clearNodeAction );
      }

      // inform about a menu being shown
      emit popupMenuShown( m_popupMenuItem, *popupMenu );

      // display the menu
      popupMenu->popup( mapToGlobal( pos ) );
   }
}


///////////////////////////////////////////////////////////////////////////////

void TreeWidget::expandNode()
{
   ASSERT_MSG( m_popupMenuItem, "No tree item selected" );
   deepCollapse( m_popupMenuItem, true );
}

///////////////////////////////////////////////////////////////////////////////

void TreeWidget::collapseNode()
{
   ASSERT_MSG( m_popupMenuItem, "No tree item selected" );
   deepCollapse( m_popupMenuItem, false );
}

///////////////////////////////////////////////////////////////////////////////

void TreeWidget::deepCollapse( QTreeWidgetItem* root, bool expand )
{
   std::list< QTreeWidgetItem* > itemsQueue;
   itemsQueue.push_back( root );

   while( !itemsQueue.empty() )
   {
      QTreeWidgetItem* currItem = itemsQueue.front();
      itemsQueue.pop_front();

      currItem->setExpanded( expand );

      unsigned int count = currItem->childCount();
      for ( unsigned int i = 0; i < count; ++i )
      {
         QTreeWidgetItem* child = currItem->child( i );
         if ( child )
         {
            itemsQueue.push_back( child );
         }
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void TreeWidget::removeNode()
{
   ASSERT_MSG( m_popupMenuItem, "No tree item selected" );
   ASSERT_MSG( m_popupMenuItem->parent(), "Item needsto have a parent in order to be removed from it" );

   emit removeNode( m_popupMenuItem->parent(), m_popupMenuItem );
}

///////////////////////////////////////////////////////////////////////////////

void TreeWidget::clearNode()
{
   ASSERT_MSG( m_popupMenuItem, "No tree item selected" );

   emit clearNode( m_popupMenuItem );
}

///////////////////////////////////////////////////////////////////////////////

void TreeWidget::renameNode()
{
   ASSERT_MSG( m_renameEditor == NULL && m_renamedItem == NULL, "Previous renaming operation hasn't finished yet" );
   QTreeWidgetItem* clickedItem = m_popupMenuItem;
   renameNode( clickedItem );
}

///////////////////////////////////////////////////////////////////////////////

void TreeWidget::renameNode( QTreeWidgetItem* node )
{
   // don't allow this operation on the root category
   m_renamedItem = node;

   m_renameEditor = new QLineEdit( this );
   m_renameEditor->setText( node->text( 0 ) );

   connect( m_renameEditor, SIGNAL( editingFinished() ), this, SLOT( itemRenamed() ) );
   setItemWidget( node, 0, m_renameEditor );
}

///////////////////////////////////////////////////////////////////////////////

void TreeWidget::itemRenamed()
{
   if ( m_renameEditor == NULL || m_renamedItem == NULL )
   {
      return;
   }

   QString newName = m_renameEditor->text();
   setItemWidget( m_renamedItem, 0, NULL );
   m_renameEditor = NULL;
   
   emit renameNode( m_renamedItem, newName );

   m_renamedItem = NULL;
}

///////////////////////////////////////////////////////////////////////////////
