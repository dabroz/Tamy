#include "BlendTreeEventsViewer.h"
#include "core-AI\BlendTree.h"
#include "core-AI\BlendTreePlayer.h"
#include "core-AI\BlendTreeEvent.h"
#include <QtWidgets\QVBoxLayout>
#include <QtWidgets\QListView>
#include <QtGui\QStandardItemModel>
#include <QtWidgets\QMenu>
#include <QtCore\QResource>
#include "BlendTreeEventsListDelegate.h"
#include "BlendTreeEventsListModel.h"

// stylesheet reading
#include "core\Filesystem.h"
#include "core\StreamBuffer.h"


///////////////////////////////////////////////////////////////////////////////

BlendTreeEventsViewer::BlendTreeEventsViewer( QWidget* parentWidget, BlendTree& blendTree, BlendTreePlayer* blendTreePlayer )
   : QFrame( parentWidget )
   , m_blendTree( blendTree )
   , m_blendTreePlayer( blendTreePlayer )
   , m_eventsListView( NULL )
{
   // create the UI
   initUI();

   // attach self as a listener to the tree and pull down its current state
   m_blendTree.attachListener( this );
   m_blendTree.pullStructure( this );
}

///////////////////////////////////////////////////////////////////////////////

BlendTreeEventsViewer::~BlendTreeEventsViewer()
{
   m_blendTree.detachListener( this );

   delete m_editTimeDelegate;
   m_editTimeDelegate = NULL;

   delete m_playTimeDelegate;
   m_playTimeDelegate = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeEventsViewer::initUI()
{
   QVBoxLayout* layout = new QVBoxLayout( this );
   setLayout( layout );

   // create a data model
   m_dataModel = new BlendTreeEventsListModel( this );

   // create a data view
   m_eventsListView = new QListView( this );
   {
      {
         Filesystem& fs = TSingleton< Filesystem >::getInstance();
         File* file = fs.open( FilePath( "/Editor/BlendTreeEventsViewerTheme.css" ) );
         StreamBuffer< char > fileBuf( *file );
         QString styleSheet = fileBuf.getBuffer();
         delete file;

         m_eventsListView->setStyleSheet( styleSheet );
      }

      // make every item selected with a single press of a mouse button
      connect( m_eventsListView, SIGNAL( clicked( const QModelIndex& ) ), m_eventsListView, SLOT( edit( const QModelIndex& ) ) );

      layout->addWidget( m_eventsListView );
      m_eventsListView->setContextMenuPolicy( Qt::CustomContextMenu );
      m_eventsListView->setDragDropMode( QAbstractItemView::DragOnly );
      connect( m_eventsListView, SIGNAL( customContextMenuRequested( const QPoint& ) ), this, SLOT( onShowContextMenu( const QPoint& ) ) );

      // wire the model to the view
      m_eventsListView->setModel( m_dataModel );
   }

   m_playTimeDelegate = new BTLEventsDelegate_Playing( m_eventsListView, m_blendTreePlayer );
   m_editTimeDelegate = new BTLEventsDelegate_Edition( m_eventsListView, m_blendTree );

   // set the initial data visualization mode
   setPlayingMode( false );
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeEventsViewer::setPlayingMode( bool isPlaying )
{
   if ( isPlaying )
   {
      m_eventsListView->setItemDelegate( m_playTimeDelegate );
   }
   else
   {
      m_eventsListView->setItemDelegate( m_editTimeDelegate );
   }
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeEventsViewer::onShowContextMenu( const QPoint& pos )
{
   QMenu* popupMenu = new QMenu( this );

   AddBlendTreeEventAction* addEventAction = new AddBlendTreeEventAction( m_blendTree );
   popupMenu->addAction( addEventAction );

   popupMenu->popup( mapToGlobal( pos ) );
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeEventsViewer::onEventAdded( BlendTreeEvent* event )
{
   QStandardItem* item = new QStandardItem( event->getName().c_str() );
   item->setData( QVariant::fromValue< void* >( event ), BTEL_EVENT_ROLE );

   m_dataModel->appendRow( item );
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeEventsViewer::onEventRemoved( BlendTreeEvent* event )
{
   int count = m_dataModel->rowCount();
   for ( int i = 0; i < count; ++i )
   {
      QStandardItem* item = m_dataModel->item( i, 0 );
      BlendTreeEvent* checkedEvent = ( BlendTreeEvent* )( item->data( BTEL_EVENT_ROLE ).value< void* >() );

      if ( checkedEvent == event )
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

AddBlendTreeEventAction::AddBlendTreeEventAction( BlendTree& blendTree )
   : QAction( QIcon( ":/TamyEditor/Resources/Icons/Editor/plus.png" ), "Add event", NULL )
   , m_blendTree( blendTree )
{
   connect( this, SIGNAL( triggered() ), this, SLOT( onTriggered() ) );
}

///////////////////////////////////////////////////////////////////////////////

void AddBlendTreeEventAction::onTriggered()
{
   BlendTreeEvent* newEvent = new BlendTreeEvent();
   m_blendTree.addEvent( newEvent );
}

///////////////////////////////////////////////////////////////////////////////
