#include "BehaviorTreeVariablesViewer.h"
#include "core-AI\BehaviorTree.h"
#include "core-AI\BehaviorTreeRunner.h"
#include "core-AI\BehTreeVariable.h"
#include <QtWidgets\QVBoxLayout>
#include <QtWidgets\QTreeView>
#include <QtGui\QStandardItemModel>
#include <QtWidgets\QMenu>
#include <QtCore\QResource>
#include "BehaviorTreeVariablesListDelegate.h"
#include "BehaviorTreeVariablesListModel.h"

// stylesheet reading
#include "core\Filesystem.h"
#include "core\StreamBuffer.h"

// ui utils
#include "MenuUtils.h"
#include "TypeDescFactory.h"


///////////////////////////////////////////////////////////////////////////////

BehaviorTreeVariablesViewer::BehaviorTreeVariablesViewer( QWidget* parentWidget, BehaviorTree& blendTree, BehaviorTreeRunner* behaviorTreeRunner )
   : QFrame( parentWidget )
   , m_behaviorTree( blendTree )
   , m_behaviorTreePlayer( behaviorTreeRunner )
{
   m_variablesFactory = new TypeDescFactory< BehaviorTreeVariable >( "unknownBehaviorTreeIcon.png" );

   // create the UI
   initUI();

   // attach self as a listener to the tree and pull down its current state
   m_behaviorTree.attachListener( this );
   m_behaviorTree.pullStructure( this );
}

///////////////////////////////////////////////////////////////////////////////

BehaviorTreeVariablesViewer::~BehaviorTreeVariablesViewer()
{
   m_behaviorTree.detachListener( this );

   delete m_editTimeDelegate;
   m_editTimeDelegate = NULL;

   delete m_playTimeDelegate;
   m_playTimeDelegate = NULL;

   delete m_variablesFactory;
   m_variablesFactory = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void BehaviorTreeVariablesViewer::initUI()
{
   QVBoxLayout* layout = new QVBoxLayout( this );
   setLayout( layout );

   // create a data model
   m_dataModel = new BehaviorTreeVariablesListModel( this );
   {
      m_dataModel->setColumnCount( 2 );

      QStringList headerLabels;
      headerLabels.push_back( "Variable name" );
      headerLabels.push_back( "Value" );
      m_dataModel->setHorizontalHeaderLabels( headerLabels );
   }

   // create a data view
   m_variablesListView = new QTreeView( this );
   {
      // make every item selected with a single press of a mouse button
      connect( m_variablesListView, SIGNAL( clicked( const QModelIndex& ) ), m_variablesListView, SLOT( edit( const QModelIndex& ) ) );

      layout->addWidget( m_variablesListView );
      m_variablesListView->setContextMenuPolicy( Qt::CustomContextMenu );
      m_variablesListView->setDragDropMode( QAbstractItemView::DragOnly );
      connect( m_variablesListView, SIGNAL( customContextMenuRequested( const QPoint& ) ), this, SLOT( onShowContextMenu( const QPoint& ) ) );

      // wire the model to the view
      m_variablesListView->setModel( m_dataModel );
   }

   m_editTimeDelegate = new BehaviorTreeVariablesListDelegate( m_variablesListView, m_behaviorTree, m_behaviorTreePlayer );
   m_playTimeDelegate = new BehaviorTreeVariablesListDelegate( m_variablesListView, m_behaviorTree, NULL );

   // set the initial data visualization mode
   setPlayingMode( false );
}

///////////////////////////////////////////////////////////////////////////////

void BehaviorTreeVariablesViewer::setPlayingMode( bool isPlaying )
{
   if ( isPlaying )
   {
      m_variablesListView->setItemDelegate( m_editTimeDelegate );
   }
   else
   {
      m_variablesListView->setItemDelegate( m_playTimeDelegate );
   }

   // refresh item values
   int count = m_dataModel->rowCount();
   char valueStr[32];

   for ( int i = 0; i < count; ++i )
   {
      QStandardItem* item = m_dataModel->item( i, 1 );

      BehaviorTreeVariable* checkedVariable = ( BehaviorTreeVariable* )( item->data( BTEL_VARIABLE_ROLE ).value< void* >() );
      if ( isPlaying )
      {
         checkedVariable->runtimeValueToString( m_behaviorTreePlayer, valueStr );
      }
      else
      {
         checkedVariable->valueToString( valueStr );
      }

      item->setText( valueStr) ;
   }
}

///////////////////////////////////////////////////////////////////////////////

void BehaviorTreeVariablesViewer::onShowContextMenu( const QPoint& pos )
{
   QMenu* popupMenu = new QMenu( this );


   class AddVariableActionFactory : public MenuActionsFactory
   {
   private:
      BehaviorTree&                 m_behaviorTree;
      TreeWidgetDescFactory*        m_variablesFactory;

   public:
      AddVariableActionFactory( BehaviorTree& behaviorTree, TreeWidgetDescFactory* variablesFactory )
         : m_behaviorTree( behaviorTree )
         , m_variablesFactory( variablesFactory )
      {}

      QAction* create( const QIcon& icon, const QString& desc, int itemTypeIdx, QWidget* parentWidget ) const
      {
         QAction* action = new AddBehaviorTreeVariableAction( icon, desc, itemTypeIdx, m_behaviorTree, m_variablesFactory );
         return action;
      }
   };

   AddVariableActionFactory actionsFactory( m_behaviorTree, m_variablesFactory );
   MenuUtils::itemsListMenu( m_variablesFactory, actionsFactory, popupMenu );

   popupMenu->popup( mapToGlobal( pos ) );
}

///////////////////////////////////////////////////////////////////////////////

void BehaviorTreeVariablesViewer::onVariableAdded( BehaviorTreeVariable* variable )
{
   QStandardItem* varNameItem = new QStandardItem( variable->getName().c_str() );
   varNameItem->setData( QVariant::fromValue< void* >( variable ), BTEL_VARIABLE_ROLE );

   char valueStr[64];
   variable->valueToString( valueStr );
   QStandardItem* varValueItem = new QStandardItem( valueStr );
   varValueItem->setData( QVariant::fromValue< void* >( variable ), BTEL_VARIABLE_ROLE );

   QList< QStandardItem* > columns;
   columns.push_back( varNameItem );
   columns.push_back( varValueItem );
   m_dataModel->appendRow( columns );
}

///////////////////////////////////////////////////////////////////////////////

void BehaviorTreeVariablesViewer::onVariableRemoved( BehaviorTreeVariable* variable )
{
   int count = m_dataModel->rowCount();
   for ( int i = 0; i < count; ++i )
   {
      QStandardItem* item = m_dataModel->item( i, 0 );
      BehaviorTreeVariable* checkedVariable = ( BehaviorTreeVariable* )( item->data( BTEL_VARIABLE_ROLE ).value< void* >() );

      if ( checkedVariable == variable )
      {
         // that's the item
         m_dataModel->removeRow( i );
         break;
      }
   }
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

AddBehaviorTreeVariableAction::AddBehaviorTreeVariableAction( const QIcon& icon, const QString& desc, uint typeIdx, BehaviorTree& behaviorTree, TreeWidgetDescFactory* variablesFactory )
   : QAction( icon, desc, NULL )
   , m_behaviorTree( behaviorTree )
   , m_typeIdx( typeIdx )
   , m_variablesFactory( variablesFactory )
{
   connect( this, SIGNAL( triggered() ), this, SLOT( onTriggered() ) );
}

///////////////////////////////////////////////////////////////////////////////

void AddBehaviorTreeVariableAction::onTriggered()
{
   const SerializableReflectionType* variableType = m_variablesFactory->getClass( m_typeIdx );
   ASSERT_MSG( variableType, "Requested a variable of an undefined type" );

   BehaviorTreeVariable* newVariable = variableType->instantiate< BehaviorTreeVariable >();
   m_behaviorTree.addVariable( newVariable );
}

///////////////////////////////////////////////////////////////////////////////
