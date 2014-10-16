#include "BlendTreeVariablesViewer.h"
#include "core-AI\BlendTree.h"
#include "core-AI\BlendTreePlayer.h"
#include "core-AI\BlendTreeVariable.h"
#include <QtWidgets\QVBoxLayout>
#include <QtWidgets\QTreeView>
#include <QtGui\QStandardItemModel>
#include <QtWidgets\QMenu>
#include <QtCore\QResource>
#include "BlendTreeVariablesListDelegate.h"
#include "BlendTreeVariablesListModel.h"

// stylesheet reading
#include "core\Filesystem.h"
#include "core\StreamBuffer.h"

// ui utils
#include "MenuUtils.h"
#include "TypeDescFactory.h"


///////////////////////////////////////////////////////////////////////////////

BlendTreeVariablesViewer::BlendTreeVariablesViewer( QWidget* parentWidget, BlendTree& blendTree, BlendTreePlayer* blendTreePlayer )
   : QFrame( parentWidget )
   , m_blendTree( blendTree )
   , m_blendTreePlayer( blendTreePlayer )
{
   m_variablesFactory = new TypeDescFactory< BlendTreeVariable >( "unknownBlendTreeIcon.png" );

   // create the UI
   initUI();

   // attach self as a listener to the tree and pull down its current state
   m_blendTree.attachListener( this );
   m_blendTree.pullStructure( this );
}

///////////////////////////////////////////////////////////////////////////////

BlendTreeVariablesViewer::~BlendTreeVariablesViewer()
{
   m_blendTree.detachListener( this );

   delete m_editTimeDelegate;
   m_editTimeDelegate = NULL;

   delete m_playTimeDelegate;
   m_playTimeDelegate = NULL;

   delete m_variablesFactory;
   m_variablesFactory = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeVariablesViewer::initUI()
{
   QVBoxLayout* layout = new QVBoxLayout( this );
   setLayout( layout );

   // create a data model
   m_dataModel = new BlendTreeVariablesListModel( this );
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

   m_editTimeDelegate = new BlendTreeVariablesListDelegate( m_variablesListView, m_blendTree, m_blendTreePlayer );
   m_playTimeDelegate = new BlendTreeVariablesListDelegate( m_variablesListView, m_blendTree, NULL );

   // set the initial data visualization mode
   setPlayingMode( false );
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeVariablesViewer::setPlayingMode( bool isPlaying )
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
      
      BlendTreeVariable* checkedVariable = ( BlendTreeVariable* )( item->data( BTEL_VARIABLE_ROLE ).value< void* >() );
      if ( isPlaying )
      {
         checkedVariable->runtimeValueToString( m_blendTreePlayer, valueStr );
      }
      else
      {
         checkedVariable->valueToString( valueStr );
      }

      item->setText( valueStr) ;
   }
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeVariablesViewer::onShowContextMenu( const QPoint& pos )
{
   QMenu* popupMenu = new QMenu( this );


   class AddVariableActionFactory : public MenuActionsFactory
   {
   private:
      BlendTree&                    m_blendTree;
      TreeWidgetDescFactory*        m_variablesFactory;

   public:
      AddVariableActionFactory( BlendTree& blendTree, TreeWidgetDescFactory* variablesFactory )
         : m_blendTree( blendTree )
         , m_variablesFactory( variablesFactory )
      {}

      QAction* create( const QIcon& icon, const QString& desc, int itemTypeIdx, QWidget* parentWidget ) const
      {
         QAction* action = new AddBlendTreeVariableAction( icon, desc, itemTypeIdx, m_blendTree, m_variablesFactory );
         return action;
      }
   };

   AddVariableActionFactory actionsFactory( m_blendTree, m_variablesFactory );
   MenuUtils::itemsListMenu( m_variablesFactory, actionsFactory, popupMenu );

   popupMenu->popup( mapToGlobal( pos ) );
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeVariablesViewer::onVariableAdded( BlendTreeVariable* variable )
{
   QStandardItem* varNameItem = new QStandardItem( variable->getName().c_str() );
   varNameItem->setData( QVariant::fromValue< void* >( variable ), BTEL_VARIABLE_ROLE );

   char valueStr[32];
   variable->valueToString( valueStr );
   QStandardItem* varValueItem = new QStandardItem( valueStr );
   varValueItem->setData( QVariant::fromValue< void* >( variable ), BTEL_VARIABLE_ROLE );

   QList< QStandardItem* > columns;
   columns.push_back( varNameItem );
   columns.push_back( varValueItem );
   m_dataModel->appendRow( columns );
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeVariablesViewer::onVariableRemoved( BlendTreeVariable* variable )
{
   int count = m_dataModel->rowCount();
   for ( int i = 0; i < count; ++i )
   {
      QStandardItem* item = m_dataModel->item( i, 0 );
      BlendTreeVariable* checkedVariable = ( BlendTreeVariable* )( item->data( BTEL_VARIABLE_ROLE ).value< void* >() );

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

AddBlendTreeVariableAction::AddBlendTreeVariableAction( const QIcon& icon, const QString& desc, uint typeIdx, BlendTree& blendTree, TreeWidgetDescFactory* variablesFactory )
   : QAction( icon, desc, NULL )
   , m_blendTree( blendTree )
   , m_typeIdx( typeIdx )
   , m_variablesFactory( variablesFactory )
{
   connect( this, SIGNAL( triggered() ), this, SLOT( onTriggered() ) );
}

///////////////////////////////////////////////////////////////////////////////

void AddBlendTreeVariableAction::onTriggered()
{
   const SerializableReflectionType* variableType = m_variablesFactory->getClass( m_typeIdx );
   ASSERT_MSG( variableType, "Requested a variable of an undefined type" );

   BlendTreeVariable* newVariable = variableType->instantiate< BlendTreeVariable >();
   m_blendTree.addVariable( newVariable );
}

///////////////////////////////////////////////////////////////////////////////
