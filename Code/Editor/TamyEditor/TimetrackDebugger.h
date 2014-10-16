/// @file   TamyEditor/TimetrackDebugger.h
/// @brief  a debugger that shows what has happened during simulation in form of a gantt chart
#pragma once

#include <QtWidgets\QFrame>

#include "core-AI\BlendTreePlayerListener.h"
#include "core\TimeDependent.h"
#include "core\List.h"
#include "core\GenericFactory.h"
#include <map>


///////////////////////////////////////////////////////////////////////////////

class QScrollBar;
class QGraphicsScene;
class BlendTreeDebugBlock;
class TimetrackDebuggerView;

///////////////////////////////////////////////////////////////////////////////

class TimetrackDebugger : public QFrame, public TimeDependent, public BlendTreePlayerListener
{
   Q_OBJECT

private:
   typedef GenericFactory< const BlendTreeNode, BlendTreeDebugBlock > BlocksFactory;

   BlocksFactory*                         m_blocksFactory;
   BlendTreePlayer*                       m_player;

   QFrame*                                m_ganttChartsFrame;
   QScrollBar*                            m_horizontalScrollbar;
   QScrollBar*                            m_verticalScrollbar;

   QGraphicsScene*                        m_nodesScene;
   QGraphicsScene*                        m_nodeLabelsScene;
   QGraphicsScene*                        m_eventsScene;
   TimetrackDebuggerView*                 m_nodesView;
   TimetrackDebuggerView*                 m_nodeLabelsView;
   TimetrackDebuggerView*                 m_eventsView;

   bool                                   m_uiInitialized;

   float                                  m_trackTime;
   bool                                   m_isPlaying;
   List< BlendTreeDebugBlock* >           m_activeItems;

   float                                  m_nextFreeOffset;
   std::map< const void*, float >         m_trackOffsetsMap;

public:
   /**
    * Constructor.
    *
    * @param parent
    */
   TimetrackDebugger( QWidget* parent );
   ~TimetrackDebugger();

   /**
    * Initializes debugger's runtime context.
    *
    * @param player
    */
   void initialize( BlendTreePlayer* player );

   /**
    * Deinitializes the debugger's runtime context
    */
   void deinitialize();

   /**
    * Sets the debugger in 'playing' mode.
    */
   void setPlayingMode( bool isPlaying );

   // -------------------------------------------------------------------------
   // TimeDependent implementation
   // -------------------------------------------------------------------------
   void update( float timeElapsed );

   // -------------------------------------------------------------------------
   // BlendTreePlayerListener implementation
   // -------------------------------------------------------------------------
   void onSimulationStarted( BlendTreePlayer* player );
   void onSimulationFinished( BlendTreePlayer* player );
   void onNodeStateChanged( BlendTreePlayer* player, const BlendTreeNode* node );
   void onEventTriggered( BlendTreePlayer* player, const BlendTreeEvent* btEvent );
   void onSyncPointReached( BlendTreePlayer* player, const BlendTreeNode* node, const BlendTreeEvent* syncPoint );

   // -------------------------------------------------------------------------
   // QFrame implementation
   // -------------------------------------------------------------------------
   void resizeEvent( QResizeEvent* event );

private:
   void initializeUI();
   void initializeGanttCharts( QFrame* parent );
   void adjustSceneRectangles();
   void synchronizeScrollbars();

   /**
    * Allocates a new position on the track.
    *
    * @param node
    */
   float allocateTrack( const BlendTreeNode* node );

   /**
    * Returns offset of a track that's dedicated to the specified node.
    *
    * @param node.
    */
   float getTrackOffset( const BlendTreeNode* node ) const;
};

///////////////////////////////////////////////////////////////////////////////
