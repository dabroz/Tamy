#include "ProfilerFrame.h"
#include <QtWidgets\QTreeWidget>
#include <QtWidgets\QHBoxLayout>
#include <QtWidgets\QVBoxLayout>
#include <QtWidgets\QFormLayout>
#include <QtWidgets\QSplitter>
#include <QtWidgets\QScrollArea>
#include <QtWidgets\QLabel>
#include <QtWidgets\QTabWidget>
#include <QtWidgets\QToolBar>
#include <QtWidgets\QAction>
#include <QtGui\QCloseEvent>
#include "ProfilerChart.h"
#include "core/ResourcesManager.h"
#include "core/Filesystem.h"


///////////////////////////////////////////////////////////////////////////////

ProfilerFrame::ProfilerFrame()
   : m_callstackTree( NULL )
{
   // create the main window splitter
   QSplitter* windowSplitter = new QSplitter( Qt::Horizontal, this );
   {
      // setup the frame layout and add the splitter widget to it
      QHBoxLayout* layout = new QHBoxLayout( this );
      layout->setMargin( 0 );
      layout->setSpacing( 0 );
      setLayout( layout );

      layout->addWidget( windowSplitter );
   }

   // create a tab manager that will contain various views of the active timers
   {
      QTabWidget* tabManager = new QTabWidget( windowSplitter );
      windowSplitter->addWidget( tabManager );

      // create a tree widget that will display a call-stack of profiled methods
      m_callstackTree = new QTreeWidget( tabManager );
      {
         QStringList headerLabels;
         headerLabels << "Timer ID" << "Name";
         m_callstackTree->setColumnCount( headerLabels.size() );
         m_callstackTree->setHeaderLabels( headerLabels );

         connect( m_callstackTree, SIGNAL( itemDoubleClicked( QTreeWidgetItem*, int ) ), this, SLOT( showSelectedTimerProfile( QTreeWidgetItem*, int ) ) );

         tabManager->addTab( m_callstackTree, QString( "Callstack" ) );
      }

      // create a list that will display all activated timers with basic info about their activity
      QFrame* timersListFrame = new QFrame( tabManager );
      {
         tabManager->addTab( timersListFrame, QString( "All timers" ) );

         // initialize frame's layout
         QVBoxLayout* layout = new QVBoxLayout( timersListFrame );
         {
            layout->setSpacing( 0 );
            layout->setMargin( 0 );
            timersListFrame->setLayout( layout );
         }

         // create a toolbar
         QToolBar* toolbar = new QToolBar( timersListFrame );
         {
            layout->addWidget( toolbar );

            QAction* resetStatisticsAction = new QAction( QIcon( ":/TamyEditor/Resources/Icons/Editor/quit.png" ), tr( "Reset statistics" ), toolbar );
            toolbar->addAction( resetStatisticsAction );
            connect( resetStatisticsAction, SIGNAL( triggered() ), this, SLOT( resetTimersListStatistics() ) );
         }

         // and the list
         m_timersList = new QTreeWidget( tabManager );
         {
            layout->addWidget( m_timersList );

            QStringList headerLabels;
            headerLabels << "Timer ID" << "Name" << "Min" << "Curr" << "Max" << "Total";
            m_timersList->setColumnCount( headerLabels.size() );
            m_timersList->setHeaderLabels( headerLabels );

            connect( m_timersList, SIGNAL( itemDoubleClicked( QTreeWidgetItem*, int ) ), this, SLOT( showSelectedTimerProfile( QTreeWidgetItem*, int ) ) );
         }
      }

      // create a list of all value profilers
      QFrame* valueProfilersListFrame = new QFrame( tabManager );
      {
         tabManager->addTab( valueProfilersListFrame, QString( "Value profilers" ) );

         // initialize frame's layout
         QVBoxLayout* layout = new QVBoxLayout( valueProfilersListFrame );
         {
            layout->setSpacing( 0 );
            layout->setMargin( 0 );
            valueProfilersListFrame->setLayout( layout );
         }

         // create a toolbar
         QToolBar* toolbar = new QToolBar( valueProfilersListFrame );
         {
            layout->addWidget( toolbar );

            QAction* resetStatisticsAction = new QAction( QIcon( ":/TamyEditor/Resources/Icons/Editor/quit.png" ), tr( "Reset statistics" ), toolbar );
            toolbar->addAction( resetStatisticsAction );
            connect( resetStatisticsAction, SIGNAL( triggered() ), this, SLOT( resetValueProfilersListStatistics() ) );
         }

         // and the list
         m_valueProfilersList = new QTreeWidget( tabManager );
         {
            layout->addWidget( m_valueProfilersList );

            QStringList headerLabels;
            headerLabels << "Timer ID" << "Name" << "Min" << "Curr" << "Max";
            m_valueProfilersList->setColumnCount( headerLabels.size() );
            m_valueProfilersList->setHeaderLabels( headerLabels );

            connect( m_valueProfilersList, SIGNAL( itemDoubleClicked( QTreeWidgetItem*, int ) ), this, SLOT( showSelectedValueProfile( QTreeWidgetItem*, int ) ) );
         }
      }
   }

   // create a frame where we'll put the charts
   {
      QScrollArea* chartsFrameScrollArea = new QScrollArea( windowSplitter );
      chartsFrameScrollArea->setWidgetResizable( true );
      windowSplitter->addWidget( chartsFrameScrollArea );

      QFrame* chartsFrame = new QFrame( chartsFrameScrollArea );
      chartsFrameScrollArea->setWidget( chartsFrame );

      m_chartsLayout = new QFormLayout( chartsFrame );
      chartsFrame->setLayout( m_chartsLayout );
   }

   // configure the splitter
   {
      windowSplitter->setOpaqueResize( false );

      QList< int > splitterWindowSizes;
      splitterWindowSizes.push_back( width() / 2 );
      splitterWindowSizes.push_back( width() / 2 );
      windowSplitter->setSizes( splitterWindowSizes );
   }
}

///////////////////////////////////////////////////////////////////////////////

void ProfilerFrame::onInitialize()
{
   // nothing to do here
}

///////////////////////////////////////////////////////////////////////////////

void ProfilerFrame::onDeinitialize( bool saveProgress )
{
   // nothing to do here
}

///////////////////////////////////////////////////////////////////////////////

uint ProfilerFrame::getProfilerId( QTreeWidgetItem* item ) const
{
   uint itemTimerId = 0;
   if ( item != NULL )
   {
      itemTimerId = item->data( CALLSTACK_COL_TIMER_ID, 0 ).toInt();
   }
   return itemTimerId;
}

///////////////////////////////////////////////////////////////////////////////

void ProfilerFrame::update( float timeElapsed )
{
   const Profiler& profiler = TSingleton< Profiler >::getInstance();

   // update the callstack view
   {
      QTreeWidgetItem* parentItem = NULL;
      QTreeWidgetItem* item = NULL;
      const Profiler::Trace* prevTrace = NULL;

      uint tracesCount = profiler.getTracesCount();
      for ( uint i = 0; i < tracesCount; ++i )
      {
         // step through subsequent traces, adding new tree leaves if required,
         // and updating the profiling information for each
         const Profiler::Trace& trace = profiler.getTrace( i );

         findCallstackItem( prevTrace, trace, parentItem, item );
         if ( item == NULL )
         {
            // an entry for this method doesn't exist yet - add it
            item = addCallstackItem( parentItem, profiler, trace );
         }

         // memorize the parent item and the parent trace id for future lookups
         prevTrace = &trace;
      }
   }

   // update the timers list
   {
      uint lastTimerId = profiler.getTimersCount();
      for ( uint timerId = 1; timerId <= lastTimerId; ++timerId )
      {
         QTreeWidgetItem* timerItem = getTimersListItem( timerId );
         double timerTimeElapsed = profiler.getTimeElapsed( timerId ) * 1000.0f; // scale up to [us]

         
         // determine new minimum value
         {
            double minVal = timerItem->data( TIMERSLIST_COL_MIN, 0 ).toDouble();
            minVal =  ( minVal < timerTimeElapsed ) ? minVal : timerTimeElapsed;
            timerItem->setData( TIMERSLIST_COL_MIN, 0, QVariant::fromValue< double >( minVal ) );
         }

         // determine new average value
         {
            timerItem->setData( TIMERSLIST_COL_CURR, 0, QVariant::fromValue< double >( timerTimeElapsed ) );
         }

         // determine new maximum value
         {
            double maxVal = timerItem->data( TIMERSLIST_COL_MAX, 0 ).toDouble();
            maxVal = ( maxVal > timerTimeElapsed ) ? maxVal : timerTimeElapsed;
            timerItem->setData( TIMERSLIST_COL_MAX, 0, QVariant::fromValue< double >( maxVal ) );
         }

         // sum the total value
         {
            double totalVal = timerItem->data( TIMERSLIST_COL_TOTAL, 0 ).toDouble();
            timerItem->setData( TIMERSLIST_COL_TOTAL, 0, QVariant::fromValue< double >( totalVal + timerTimeElapsed ) );
         }
      }
   }

   // update the value profilers list
   {
      uint valueProfilersCount = profiler.getValueProfilersCount();
      for ( uint profilerId = 0; profilerId < valueProfilersCount; ++profilerId )
      {
         QTreeWidgetItem* profilerItem = getValueProfilerListItem( profilerId );
         double value = profiler.getProfiledValue( profilerId );

         // determine new minimum value
         {
            double minVal = profilerItem->data( TIMERSLIST_COL_MIN, 0 ).toDouble();
            minVal =  ( minVal < value ) ? minVal : value;
            profilerItem->setData( TIMERSLIST_COL_MIN, 0, QVariant::fromValue< double >( minVal ) );
         }

         // determine new average value
         {
            profilerItem->setData( TIMERSLIST_COL_CURR, 0, QVariant::fromValue< double >( value ) );
         }

         // determine new maximum value
         {
            double maxVal = profilerItem->data( TIMERSLIST_COL_MAX, 0 ).toDouble();
            maxVal = ( maxVal > value ) ? maxVal : value;
            profilerItem->setData( TIMERSLIST_COL_MAX, 0, QVariant::fromValue< double >( maxVal ) );
         }

         // we're not summing the total value here
      }
   }


   // update active timers
   uint chartsCount = m_timersCharts.size();
   for ( uint i = 0; i < chartsCount; ++i )
   {
      ProfilerChart* chart = m_timersCharts[i].m_chart;
      QLabel* topValueLabel = m_timersCharts[i].m_topValueLabel;


      double timerTimeElapsed = profiler.getTimeElapsed( chart->m_profilerId );
      float topValue = chart->addTimeSample( timeElapsed, timerTimeElapsed );

      QString topValueText;
      topValueText.sprintf( "%.3f [us]", topValue * 1000.0f );
      topValueLabel->setText( topValueText );
   }

   // update value profilers
   chartsCount = m_valueProfilersCharts.size();
   for ( uint i = 0; i < chartsCount; ++i )
   {
      ProfilerChart* chart = m_valueProfilersCharts[i].m_chart;
      QLabel* topValueLabel = m_valueProfilersCharts[i].m_topValueLabel;

      double value = profiler.getProfiledValue( chart->m_profilerId );
      float topValue = chart->addValueSample( timeElapsed, value );

      QString topValueText;
      topValueText.sprintf( "%.3f", topValue );
      topValueLabel->setText( topValueText );
   }
}

///////////////////////////////////////////////////////////////////////////////

QTreeWidgetItem* ProfilerFrame::getTimersListItem( uint timerId )
{
   // first, locate the item on the list
   int itemsCount = m_timersList->topLevelItemCount();
   int insertionIdx = itemsCount;
   for ( int i = 0; i < itemsCount; ++i )
   {
      QTreeWidgetItem* item = m_timersList->topLevelItem( i );
      uint id = getProfilerId( item );

      if ( id == timerId )
      {
         // found it
         return item;
      }
      else if ( id > timerId )
      {
         insertionIdx = i - 1;
         break;
      }
   }

   // create a new entry
   const Profiler& profiler = TSingleton< Profiler >::getInstance();

   QTreeWidgetItem* item = new QTreeWidgetItem( m_timersList );
   m_timersList->insertTopLevelItem( insertionIdx, item );

   const std::string& timerName = profiler.getTimerName( timerId );
   item->setData( TIMERSLIST_COL_TIMER_ID, 0, QVariant::fromValue< int >( timerId ) );
   item->setText( TIMERSLIST_COL_NAME, timerName.c_str() );
   item->setData( TIMERSLIST_COL_MIN, 0, QVariant::fromValue< double >( 10000000.0f ) );
   item->setData( TIMERSLIST_COL_CURR, 0, QVariant::fromValue< double >( 0 ) );
   item->setData( TIMERSLIST_COL_MAX, 0, QVariant::fromValue< double >( 0 ) );
   item->setData( TIMERSLIST_COL_TOTAL, 0, QVariant::fromValue< double >( 0 ) );

   return item;
}

///////////////////////////////////////////////////////////////////////////////

QTreeWidgetItem* ProfilerFrame::getValueProfilerListItem( uint profilerId )
{
   // first, locate the item on the list
   int itemsCount = m_valueProfilersList->topLevelItemCount();
   int insertionIdx = itemsCount;
   for ( int i = 0; i < itemsCount; ++i )
   {
      QTreeWidgetItem* item = m_valueProfilersList->topLevelItem( i );
      uint id = getProfilerId( item );

      if ( id == profilerId )
      {
         // found it
         return item;
      }
      else if ( id > profilerId )
      {
         insertionIdx = i - 1;
         break;
      }
   }

   // create a new entry
   const Profiler& profiler = TSingleton< Profiler >::getInstance();

   QTreeWidgetItem* item = new QTreeWidgetItem( m_valueProfilersList );
   m_valueProfilersList->insertTopLevelItem( insertionIdx, item );

   const std::string& valueProfilerName = profiler.getValueProfilerName( profilerId );
   item->setData( TIMERSLIST_COL_TIMER_ID, 0, QVariant::fromValue< int >( profilerId ) );
   item->setText( TIMERSLIST_COL_NAME, valueProfilerName.c_str() );
   item->setData( TIMERSLIST_COL_MIN, 0, QVariant::fromValue< double >( 10000000.0f ) );
   item->setData( TIMERSLIST_COL_CURR, 0, QVariant::fromValue< double >( 0 ) );
   item->setData( TIMERSLIST_COL_MAX, 0, QVariant::fromValue< double >( 0 ) );
   item->setData( TIMERSLIST_COL_TOTAL, 0, QVariant::fromValue< double >( 0 ) );

   return item;
}

///////////////////////////////////////////////////////////////////////////////

void ProfilerFrame::findCallstackItem( const Profiler::Trace* prevTrace, const Profiler::Trace& trace, QTreeWidgetItem*& parentItem, QTreeWidgetItem*& item ) const
{
   QTreeWidgetItem* analyzedItem = NULL;

   if ( !prevTrace )
   {
      // this is the first trace - see if any of the root items matches the parent trace id
      int count = m_callstackTree->topLevelItemCount();
      for( int i = 0; i < count; ++i )
      {
         QTreeWidgetItem* childItem = m_callstackTree->topLevelItem( i );
         uint itemTraceId = getProfilerId( childItem );
         if ( itemTraceId == trace.m_parentTimerId )
         {
            // found it
            analyzedItem = childItem;
            break;
         }
      }
   }
   else
   {
      if ( prevTrace->m_timerId == trace.m_parentTimerId )
      {
         // this is a method call made from the previous method, so we're going deeper into the call-stack
         analyzedItem = item;
      }
      else if ( prevTrace->m_parentTimerId == trace.m_parentTimerId )
      {
         // this is a method call made from the same context the previous method call was made ( they share the same parent trace )
         analyzedItem = parentItem;
      }
      else if ( parentItem )
      {
         // this is a call made from higher up in the call stack, so we need to find the parent item that
         // bears the new trace's parent's id
         for ( analyzedItem = parentItem->parent(); analyzedItem != NULL; analyzedItem = analyzedItem->parent() )
         {
            uint itemTraceId = getProfilerId( analyzedItem );
            if ( itemTraceId == trace.m_parentTimerId )
            {
               // found it
               break;
            }
         }
      }
   }

   // memorize the new parent item
   parentItem = analyzedItem;
   item = NULL;

   // find the corresponding trace item
   if ( analyzedItem )
   {
      int childrenCount = analyzedItem->childCount();
      for ( int i = 0; i < childrenCount; ++i )
      {
         QTreeWidgetItem* childItem = analyzedItem->child( i );

         uint itemTraceId = getProfilerId( childItem );
         if ( itemTraceId == trace.m_timerId )
         {
            // found it
            item = childItem;
            return;
         }
      }
   }
   else
   {
      // look in the tree root
      int count = m_callstackTree->topLevelItemCount();
      for( int i = 0; i < count; ++i )
      {
         QTreeWidgetItem* childItem = m_callstackTree->topLevelItem( i );
         uint itemTraceId = getProfilerId( childItem );
         if ( itemTraceId == trace.m_timerId )
         {
            // found it
            item = childItem;
            break;
         }
      }
   }

   // if we found ourselves here, it means that it's just a trace that doesn't have a corresponding item created yet
}

///////////////////////////////////////////////////////////////////////////////

QTreeWidgetItem* ProfilerFrame::addCallstackItem( QTreeWidgetItem* parentItem, const Profiler& profiler, const Profiler::Trace& trace )
{
   QTreeWidgetItem* newItem = NULL;

   if ( !parentItem )
   {
      // this is a top level tree item
      newItem = new QTreeWidgetItem( m_callstackTree );
      m_callstackTree->addTopLevelItem( newItem );
   }
   else
   {
      newItem = new QTreeWidgetItem( parentItem );
      parentItem->addChild( newItem );
   }

   // setup the new item
   const std::string& timerName = profiler.getTimerName( trace.m_timerId );
   newItem->setText( CALLSTACK_COL_NAME, timerName.c_str() );
   newItem->setData( CALLSTACK_COL_TIMER_ID, 0, QVariant::fromValue< int >( trace.m_timerId ) );

   return newItem;
}

///////////////////////////////////////////////////////////////////////////////

void ProfilerFrame::showSelectedTimerProfile( QTreeWidgetItem* timerItem, int clickedColumnIdx )
{
   // check if we aren't already plotting a chart for this one
   uint timerId = getProfilerId( timerItem );

   uint chartsCount = m_timersCharts.size();
   for ( uint i = 0; i < chartsCount; ++i )
   {
      if ( m_timersCharts[i].m_chart->m_profilerId == timerId )
      {
         // yup - already plotting this one
         return;
      }
   }

   // we currently don't have a chart for this timer, so let's create it
   Profiler& profiler = TSingleton< Profiler >::getInstance();

   QString chartLabel;
   chartLabel.sprintf( "%d ) %s:", timerId, profiler.getTimerName( timerId ).c_str() );

   QFrame* chartFrame = new QFrame( m_chartsLayout->parentWidget() );
   {
      QHBoxLayout* chartFrameLayout = new QHBoxLayout( chartFrame );
      chartFrameLayout->setMargin( 0.0f );
      chartFrameLayout->setSpacing( 0.0f );
      chartFrame->setLayout( chartFrameLayout );

      ProfilerChart* chart = new ProfilerChart( m_chartsLayout->parentWidget(), timerId );
      chartFrameLayout->addWidget( chart );
   
      QLabel* topValueLabel = new QLabel( chartFrame );
      chartFrameLayout->addWidget( topValueLabel );

      m_timersCharts.push_back( ChartDesc( chart, topValueLabel ) );
   }

   m_chartsLayout->addRow( chartLabel, chartFrame );
}

///////////////////////////////////////////////////////////////////////////////

void ProfilerFrame::resetTimersListStatistics()
{
   int count = m_timersList->topLevelItemCount();
   for ( int i = 0; i < count; ++i )
   {
      QTreeWidgetItem* timerItem = m_timersList->topLevelItem( i );

      timerItem->setData( TIMERSLIST_COL_MIN, 0, QVariant::fromValue< double >( 10000000.0f ) );
      timerItem->setData( TIMERSLIST_COL_CURR, 0, QVariant::fromValue< double >( 0 ) );
      timerItem->setData( TIMERSLIST_COL_MAX, 0, QVariant::fromValue< double >( 0 ) );
      timerItem->setData( TIMERSLIST_COL_TOTAL, 0, QVariant::fromValue< double >( 0 ) );
   }
}

///////////////////////////////////////////////////////////////////////////////

void ProfilerFrame::showSelectedValueProfile( QTreeWidgetItem* timerItem, int clickedColumnIdx )
{
   // check if we aren't already plotting a chart for this one
   uint profilerId = getProfilerId( timerItem );

   uint chartsCount = m_valueProfilersCharts.size();
   for ( uint i = 0; i < chartsCount; ++i )
   {
      if ( m_valueProfilersCharts[i].m_chart->m_profilerId == profilerId )
      {
         // yup - already plotting this one
         return;
      }
   }

   // we currently don't have a chart for this timer, so let's create it
   Profiler& profiler = TSingleton< Profiler >::getInstance();

   QString chartLabel;
   chartLabel.sprintf( "%d ) %s:", profilerId, profiler.getValueProfilerName( profilerId ).c_str() );

   QFrame* chartFrame = new QFrame( m_chartsLayout->parentWidget() );
   {
      QHBoxLayout* chartFrameLayout = new QHBoxLayout( chartFrame );
      chartFrameLayout->setMargin( 0.0f );
      chartFrameLayout->setSpacing( 0.0f );
      chartFrame->setLayout( chartFrameLayout );

      ProfilerChart* chart = new ProfilerChart( m_chartsLayout->parentWidget(), profilerId );
      chartFrameLayout->addWidget( chart );

      QLabel* topValueLabel = new QLabel( chartFrame );
      chartFrameLayout->addWidget( topValueLabel );

      m_valueProfilersCharts.push_back( ChartDesc( chart, topValueLabel ) );
   }

   m_chartsLayout->addRow( chartLabel, chartFrame );
}

///////////////////////////////////////////////////////////////////////////////

void ProfilerFrame::resetValueProfilersListStatistics()
{
   int count = m_valueProfilersList->topLevelItemCount();
   for ( int i = 0; i < count; ++i )
   {
      QTreeWidgetItem* valueProfilerItem = m_valueProfilersList->topLevelItem( i );

      valueProfilerItem->setData( TIMERSLIST_COL_MIN, 0, QVariant::fromValue< double >( 10000000.0f ) );
      valueProfilerItem->setData( TIMERSLIST_COL_CURR, 0, QVariant::fromValue< double >( 0 ) );
      valueProfilerItem->setData( TIMERSLIST_COL_MAX, 0, QVariant::fromValue< double >( 0 ) );
   }
}

///////////////////////////////////////////////////////////////////////////////

void ProfilerFrame::closeEvent( QCloseEvent *event )
{
   emit onClosed();
   event->accept();
}

///////////////////////////////////////////////////////////////////////////////
