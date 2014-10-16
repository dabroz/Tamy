#include "TimetrackDebugger.h"
#include "core-AI\BlendTreePlayer.h"
#include "core-AI\BlendTreeEvent.h"
#include "core\Algorithms.h"
#include "core\ListUtils.h"
#include "core\TimeController.h"

#include "core-AI\BlendTreeNode.h"
#include "core-AI\BlendTreeAnimation.h"

#include "TimetrackDebuggerView.h"
#include "BlendTreeDebugBlock.h"
#include "BlendTreeDebugLabel.h"
#include "BlendTreeDebugEvent.h"
#include <QtWidgets\QVBoxLayout>
#include <QtWidgets\QHBoxLayout>
#include <QtWidgets\QFormLayout>
#include <QtWidgets\QScrollbar>
#include <QtGui\QResizeEvent>
#include <QtWidgets\QSplitter>
#include <QtWidgets\QLabel>


///////////////////////////////////////////////////////////////////////////////

#define TIMETRACK_DEBUGGER_TRACK_HEIGHT       26.0f
#define TIMETRACK_DEBUGGER_BLOCK_HEIGHT       20.0f

///////////////////////////////////////////////////////////////////////////////

TimetrackDebugger::TimetrackDebugger( QWidget* parent )
   : QFrame( parent )
   , m_player( NULL )
   , m_isPlaying( false )
   , m_trackTime( 0.0f )
   , m_nextFreeOffset( 3.0f )
   , m_uiInitialized( false )
   , m_nodesScene( NULL )
   , m_nodeLabelsScene( NULL )
   , m_eventsScene( NULL )
   , m_blocksFactory( NULL )
{
   m_blocksFactory = new BlocksFactory();
   m_blocksFactory->associate< const BlendTreeAnimation, BTDBAnimationNode >();
   m_blocksFactory->associateAbstract< const BlendTreeNode, BlendTreeDebugBlock >();
}

///////////////////////////////////////////////////////////////////////////////

TimetrackDebugger::~TimetrackDebugger()
{
   ASSERT( m_player == NULL );

   TimeController& timeController = TSingleton< TimeController >::getInstance();
   timeController.remove( *this );

   delete m_nodesScene;
   m_nodesScene = NULL;

   delete m_nodeLabelsScene;
   m_nodeLabelsScene = NULL;

   delete m_eventsScene;
   m_eventsScene = NULL;

   delete m_blocksFactory;
   m_blocksFactory = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void TimetrackDebugger::initializeUI()
{
   QVBoxLayout* layout = new QVBoxLayout( this );
   layout->setSpacing( 0 );
   layout->setMargin( 0 );
   setLayout( layout );

   m_nodesScene = new QGraphicsScene( this );
   m_nodeLabelsScene = new QGraphicsScene( this );
   m_eventsScene = new QGraphicsScene( this );

   m_ganttChartsFrame = new QFrame( this );
   {
      layout->addWidget( m_ganttChartsFrame );

      QVBoxLayout* frameWithHorizontalScrollbarLayout = new QVBoxLayout( m_ganttChartsFrame );
      frameWithHorizontalScrollbarLayout->setSpacing( 0 );
      frameWithHorizontalScrollbarLayout->setMargin( 0 );
      m_ganttChartsFrame->setLayout( frameWithHorizontalScrollbarLayout );

      QFrame* frameWithVerticalScrollbar = new QFrame( m_ganttChartsFrame );
      {
         frameWithHorizontalScrollbarLayout->addWidget( frameWithVerticalScrollbar );

         QHBoxLayout* frameWithVerticalScrollbarLayout = new QHBoxLayout( frameWithVerticalScrollbar );
         frameWithVerticalScrollbarLayout->setSpacing( 0 );
         frameWithVerticalScrollbarLayout->setMargin( 0 );
         frameWithVerticalScrollbar->setLayout( frameWithVerticalScrollbarLayout );


         QFrame* frameWithGanttCharts = new QFrame( frameWithVerticalScrollbar );
         {
            frameWithVerticalScrollbarLayout->addWidget( frameWithGanttCharts );

            QVBoxLayout* frameWithGanttChartsLayout = new QVBoxLayout( frameWithGanttCharts );
            frameWithGanttChartsLayout->setSpacing( 0 );
            frameWithGanttChartsLayout->setMargin( 0 );
            frameWithGanttCharts->setLayout( frameWithGanttChartsLayout );

            initializeGanttCharts( frameWithGanttCharts );
         }

         m_verticalScrollbar = new QScrollBar( Qt::Vertical, frameWithVerticalScrollbar );
         {
            frameWithVerticalScrollbarLayout->addWidget( m_verticalScrollbar );
         }
      }

      m_horizontalScrollbar = new QScrollBar( Qt::Horizontal, m_ganttChartsFrame );
      {
         frameWithHorizontalScrollbarLayout->addWidget( m_horizontalScrollbar );
      }
   }

   // synchronize toolbars of all gantt charts
   QScrollBar* nodesHorizScrollBar = m_nodesView->horizontalScrollBar();
   QScrollBar* eventsHorizScrollBar = m_eventsView->horizontalScrollBar();
   connect( m_horizontalScrollbar, SIGNAL( valueChanged( int ) ), eventsHorizScrollBar, SLOT( setValue( int ) ) );
   connect( m_horizontalScrollbar, SIGNAL( valueChanged( int ) ), nodesHorizScrollBar, SLOT( setValue( int ) ) );

   QScrollBar* nodeLabelsVertScrollBar = m_nodeLabelsView->verticalScrollBar();
   connect( m_verticalScrollbar, SIGNAL( valueChanged( int ) ), nodeLabelsVertScrollBar, SLOT( setValue( int ) ) );

   TimeController& timeController = TSingleton< TimeController >::getInstance();
   timeController.add( *this );

   adjustSceneRectangles();
}

///////////////////////////////////////////////////////////////////////////////

void TimetrackDebugger::initialize( BlendTreePlayer* player )
{
   m_player = player;
   m_player->attachListener( this );
}

///////////////////////////////////////////////////////////////////////////////

void TimetrackDebugger::deinitialize()
{
   if ( m_player )
   {
      m_player->detachListener( this );
      m_player = NULL;
   }
}

///////////////////////////////////////////////////////////////////////////////

void TimetrackDebugger::initializeGanttCharts( QFrame* parent )
{
   QSplitter* splitter = new QSplitter( Qt::Horizontal, parent );
   parent->layout()->addWidget( splitter );

   QFrame* leftFrame = new QFrame( splitter );
   {
      QVBoxLayout* layout = new QVBoxLayout( leftFrame );
      layout->setSpacing( 0 );
      layout->setMargin( 0 );
      leftFrame->setLayout( layout );

      splitter->addWidget( leftFrame );
   }

   QFrame* rightFrame = new QFrame( splitter );
   {
      QVBoxLayout* layout = new QVBoxLayout( rightFrame );
      layout->setSpacing( 0 );
      layout->setMargin( 0 );
      rightFrame->setLayout( layout );

      splitter->addWidget( rightFrame );
   }

   // layout frame
   {
      QLabel* eventsLabel = new QLabel( "Events", leftFrame );
      {
         eventsLabel->setMinimumHeight( TIMETRACK_DEBUGGER_TRACK_HEIGHT );
         eventsLabel->setMaximumHeight( TIMETRACK_DEBUGGER_TRACK_HEIGHT );

         leftFrame->layout()->addWidget( eventsLabel );
      }

      m_nodeLabelsView = new TimetrackDebuggerView( rightFrame, TIMETRACK_DEBUGGER_TRACK_HEIGHT );
      {
         m_nodeLabelsView->setScene( m_nodeLabelsScene );

         leftFrame->layout()->addWidget( m_nodeLabelsView );
      }
   }
   
   // right frame
   {
      m_eventsView = new TimetrackDebuggerView( rightFrame, TIMETRACK_DEBUGGER_TRACK_HEIGHT );
      {
         m_eventsView->setMinimumHeight( TIMETRACK_DEBUGGER_TRACK_HEIGHT );
         m_eventsView->setMaximumHeight( TIMETRACK_DEBUGGER_TRACK_HEIGHT );
         m_eventsView->drawTimeTicks( true );

         m_eventsView->setScene( m_eventsScene );

         rightFrame->layout()->addWidget( m_eventsView );
      }

      m_nodesView = new TimetrackDebuggerView( rightFrame, TIMETRACK_DEBUGGER_TRACK_HEIGHT );
      {
         m_nodesView->setScene( m_nodesScene );

         rightFrame->layout()->addWidget( m_nodesView );
      }
   }

   // set the initial position of gantt charts splitter
   {
      QList< int > splitterWindowSizes;
      splitterWindowSizes.push_back( width() * 0.1 );
      splitterWindowSizes.push_back( width() * 0.9 );
      splitter->setSizes( splitterWindowSizes );
   }

}

///////////////////////////////////////////////////////////////////////////////

void TimetrackDebugger::resizeEvent( QResizeEvent* event )
{
   if ( !m_uiInitialized )
   {
      initializeUI();
      m_uiInitialized = true;
   }

   synchronizeScrollbars();
}

///////////////////////////////////////////////////////////////////////////////

void TimetrackDebugger::synchronizeScrollbars()
{
   QScrollBar* horizSB = m_nodesView->horizontalScrollBar();
   const int hsbMin = horizSB->minimum();
   const int hsbMax = horizSB->maximum();
   m_horizontalScrollbar->setRange( hsbMin, hsbMax );
   m_horizontalScrollbar->setPageStep( horizSB->pageStep() );
   m_horizontalScrollbar->setSingleStep( horizSB->singleStep() );

   QScrollBar* vertSB = m_nodesView->verticalScrollBar();
   const int vsbMin = vertSB->minimum();
   const int vsbMax = vertSB->maximum();
   m_verticalScrollbar->setRange( vsbMin, vsbMax );
   m_verticalScrollbar->setPageStep( vertSB->pageStep() );
   m_verticalScrollbar->setSingleStep( vertSB->singleStep() );
}

///////////////////////////////////////////////////////////////////////////////

void TimetrackDebugger::onSimulationStarted( BlendTreePlayer* player )
{
   m_nodesScene->clear();
   m_nodeLabelsScene->clear();
   m_eventsScene->clear();

   m_trackTime = 0.0f;
      
   adjustSceneRectangles();
   synchronizeScrollbars();
}

///////////////////////////////////////////////////////////////////////////////

void TimetrackDebugger::onSimulationFinished( BlendTreePlayer* player )
{
   m_activeItems.clear();
   m_nextFreeOffset = 3.0f;
   m_trackOffsetsMap.clear();
}

///////////////////////////////////////////////////////////////////////////////

void TimetrackDebugger::onNodeStateChanged( BlendTreePlayer* player, const BlendTreeNode* node )
{
   BlendTreeNode::State state = node->getState( player );
   if ( state == BlendTreeNode::Inactive )
   {
      // node deactivated - found a corresponding item and stop updating it
      List< BlendTreeDebugBlock* >::iterator it = ListUtils::find< BlendTreeDebugBlock*, const BlendTreeNode*, BlendTreeDebugBlock >( m_activeItems, node );
      if ( !it.isEnd() )
      {
         it.markForRemoval();
      }
   }
   else
   {
      // node activated - start drawing an item for it
      List< BlendTreeDebugBlock* >::iterator it = ListUtils::find< BlendTreeDebugBlock*, const BlendTreeNode*, BlendTreeDebugBlock >( m_activeItems, node );
      if ( it.isEnd() )
      {
         // get a track dedicated to this node
         float trackOffset = allocateTrack( node );
         BlendTreeDebugBlock* item = m_blocksFactory->create( *node );
         item->initialize( TIMETRACK_DEBUGGER_BLOCK_HEIGHT, m_trackTime, trackOffset );
         m_activeItems.pushBack( item );
         m_nodesScene->addItem( item );

         // give the node a label
         m_nodeLabelsScene->addItem( new BlendTreeDebugLabel( node->getName().c_str(), TIMETRACK_DEBUGGER_BLOCK_HEIGHT, trackOffset ) );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void TimetrackDebugger::onEventTriggered( BlendTreePlayer* player, const BlendTreeEvent* btEvent )
{
   BlendTreeDebugEvent* eventsSceneItem = new BlendTreeDebugEvent( btEvent->getName().c_str(), TIMETRACK_DEBUGGER_BLOCK_HEIGHT, m_trackTime, 0 );
   m_eventsScene->addItem( eventsSceneItem );
}

///////////////////////////////////////////////////////////////////////////////

void TimetrackDebugger::onSyncPointReached( BlendTreePlayer* player, const BlendTreeNode* node, const BlendTreeEvent* syncPoint )
{
   BlendTreeDebugSyncPoint* eventsSceneItem = new BlendTreeDebugSyncPoint( syncPoint->getName().c_str(), TIMETRACK_DEBUGGER_BLOCK_HEIGHT, m_trackTime, 0, true );
   m_eventsScene->addItem( eventsSceneItem );

   float trackOffset = getTrackOffset( node );
   if ( trackOffset >= 0.0f )
   {
      BlendTreeDebugSyncPoint* nodesSceneItem = new BlendTreeDebugSyncPoint( syncPoint->getName().c_str(), TIMETRACK_DEBUGGER_BLOCK_HEIGHT, m_trackTime, trackOffset, false );
      m_nodesScene->addItem( nodesSceneItem );
   }
}

///////////////////////////////////////////////////////////////////////////////

float TimetrackDebugger::allocateTrack( const BlendTreeNode* node )
{
   float offset = 0.0f;

   std::map< const void*, float >::const_iterator it = m_trackOffsetsMap.find( node );
   if ( it == m_trackOffsetsMap.end() )
   {
      offset = m_nextFreeOffset;
      m_nextFreeOffset += TIMETRACK_DEBUGGER_TRACK_HEIGHT;
      m_trackOffsetsMap.insert( std::make_pair( node, offset ) );
   }
   else
   {
      offset = it->second;
   }

   return offset;
}

///////////////////////////////////////////////////////////////////////////////

float TimetrackDebugger::getTrackOffset( const BlendTreeNode* node ) const
{
   std::map< const void*, float >::const_iterator it = m_trackOffsetsMap.find( node );
   if ( it != m_trackOffsetsMap.end() )
   {
      return it->second;
   }

   return -1.0f;
}

///////////////////////////////////////////////////////////////////////////////

void TimetrackDebugger::setPlayingMode( bool isPlaying )
{
   m_isPlaying = isPlaying;
}

///////////////////////////////////////////////////////////////////////////////

void TimetrackDebugger::update( float timeElapsed )
{
   if ( m_isPlaying )
   {
      float scaledTimeElapsed = timeElapsed * 100.0f; // scale to milliseconds
      m_trackTime += scaledTimeElapsed;

      for ( List< BlendTreeDebugBlock* >::iterator it = m_activeItems.begin(); !it.isEnd(); ++it )
      {
         BlendTreeDebugBlock* item = *it;
         item->updateWidth( m_player, scaledTimeElapsed );
      }
      
      adjustSceneRectangles();
      synchronizeScrollbars();
   }
}

///////////////////////////////////////////////////////////////////////////////

void TimetrackDebugger::adjustSceneRectangles()
{
   const QRectF& viewGeometry = geometry();
   float width = max2< float >( m_trackTime, viewGeometry.width() );

   {
      QRectF sceneRect( 0.0f, 0.0f, width, TIMETRACK_DEBUGGER_TRACK_HEIGHT );
      m_eventsScene->setSceneRect( sceneRect );
   }
   {
      float height = max2< float >( m_trackOffsetsMap.size() * TIMETRACK_DEBUGGER_TRACK_HEIGHT, viewGeometry.height() );
      QRectF sceneRect( 0.0f, 0.0f, width, height );
      m_nodesScene->setSceneRect( sceneRect );
   }
   {
      float height = max2< float >( m_trackOffsetsMap.size() * TIMETRACK_DEBUGGER_TRACK_HEIGHT, viewGeometry.height() );
      QRectF sceneRect( 0.0f, 0.0f, m_nodeLabelsView->geometry().width(), height );
      m_nodeLabelsScene->setSceneRect( sceneRect );
   }
}
 
///////////////////////////////////////////////////////////////////////////////
