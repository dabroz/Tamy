/// @file   TamyEditor/ProfilerFrame.h
/// @brief  a frame that shows the engine profiler results
#pragma once

#include "ResourceEditor.h"
#include "core/Profiler.h"
#include <vector>


///////////////////////////////////////////////////////////////////////////////

class QTreeWidget;
class QTreeWidgetItem;
class QFormLayout;
class ProfilerChart;
class QLabel;
class QCloseEvent;

///////////////////////////////////////////////////////////////////////////////

/**
 * A frame that shows the engine profiler results.
 */
class ProfilerFrame : public QFrame
{
   Q_OBJECT

private:
   enum CallstackTreeColumns
   {
      CALLSTACK_COL_TIMER_ID,
      CALLSTACK_COL_NAME,
   };

   // we need the first two values in both of these enums have the same value, 'cause we're using the same timer searching 
   // algo over both of these widgets, and we need to be able to query for the timer ids in the same way in both of them
   enum TimersListColumns
   {
      TIMERSLIST_COL_TIMER_ID = CALLSTACK_COL_TIMER_ID,
      TIMERSLIST_COL_NAME = CALLSTACK_COL_NAME,
      TIMERSLIST_COL_MIN,
      TIMERSLIST_COL_CURR,
      TIMERSLIST_COL_MAX,
      TIMERSLIST_COL_TOTAL,
   };

   struct ChartDesc
   {
      ProfilerChart*    m_chart;
      QLabel*           m_topValueLabel;

      ChartDesc( ProfilerChart* chart, QLabel* topValueLabel ) : m_chart( chart ), m_topValueLabel( topValueLabel ) {}
   };

private:
   QTreeWidget*                     m_callstackTree;
   QTreeWidget*                     m_timersList;
   QTreeWidget*                     m_valueProfilersList;
   QFormLayout*                     m_chartsLayout;

   std::vector< ChartDesc >         m_timersCharts;
   std::vector< ChartDesc >         m_valueProfilersCharts;

public:
   /**
    * Constructor.
    */
   ProfilerFrame();

   /**
    * Call this method to update profiling information.
    */
   void update( float timeElapsed );

   // -------------------------------------------------------------------------
   // ResourceEditor implementation
   // -------------------------------------------------------------------------
   void onInitialize();
   void onDeinitialize( bool saveProgress );

protected:
   void closeEvent( QCloseEvent *event );

signals:
   /**
    * Signal emitted when the frame closes down.
    */
   void onClosed();

public slots:
   void showSelectedTimerProfile( QTreeWidgetItem* timerItem, int clickedColumnIdx );
   void showSelectedValueProfile( QTreeWidgetItem* timerItem, int clickedColumnIdx );
   void resetTimersListStatistics();
   void resetValueProfilersListStatistics();

private:
   // methods related to the callstack tree
   void findCallstackItem( const Profiler::Trace* prevTrace, const Profiler::Trace& trace, QTreeWidgetItem*& parentItem, QTreeWidgetItem*& item ) const;
   QTreeWidgetItem* addCallstackItem( QTreeWidgetItem* parentItem, const Profiler& profiler, const Profiler::Trace& trace );

   // methods related to lists of profilers
   QTreeWidgetItem* getTimersListItem( uint timerId );
   QTreeWidgetItem* getValueProfilerListItem( uint profilerId );

   /**
    * A helper method that extracts a profiler id from the specified tree item.
    */
   uint getProfilerId( QTreeWidgetItem* item ) const;
};

///////////////////////////////////////////////////////////////////////////////
