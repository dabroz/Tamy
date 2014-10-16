/// @file   TamyEditor/BlendTreeEventsViewer.h
/// @brief  a list and an editor of the blend tree events
#pragma once

#include <QtWidgets\QFrame>
#include <QtWidgets\QAction>
#include "core-AI\BlendTreeListener.h"


///////////////////////////////////////////////////////////////////////////////

class BlendTree;
class BlendTreePlayer;
class QListView;
class QStandardItemModel;
class QLineEdit;
class BTLEventsDelegate_Playing;
class BTLEventsDelegate_Edition;

///////////////////////////////////////////////////////////////////////////////

/**
 * A list and an editor of the blend tree events.
 */
class BlendTreeEventsViewer : public QFrame, public BlendTreeListener
{
   Q_OBJECT

private:
   BlendTree&                       m_blendTree;
   BlendTreePlayer*                 m_blendTreePlayer;

   QListView*                       m_eventsListView;
   QStandardItemModel*              m_dataModel;

   BTLEventsDelegate_Edition*       m_editTimeDelegate;
   BTLEventsDelegate_Playing*       m_playTimeDelegate;

public:
   /**
    * Constructor.
    *
    * @param parentWidget
    * @param blendTree
    * @param blendTreeWidget
    */
   BlendTreeEventsViewer( QWidget* parentWidget, BlendTree& blendTree, BlendTreePlayer* blendTreePlayer );
   ~BlendTreeEventsViewer();

   /**
    * Switches the viewer to the 'playing' mode, in which instead of editing, one
    * can trigger events.
    *
    * @param isPlaying
    */
   void setPlayingMode( bool isPlaying );

   // -------------------------------------------------------------------------
   // BlendTreeListener implementation
   // -------------------------------------------------------------------------
   void onEventAdded( BlendTreeEvent* event );
   void onEventRemoved( BlendTreeEvent* event );

public slots:
   void onShowContextMenu( const QPoint& pos );

private:
   void initUI();
};

///////////////////////////////////////////////////////////////////////////////

class AddBlendTreeEventAction : public QAction
{
   Q_OBJECT

private:
   BlendTree&                          m_blendTree;

public:
   /**
    * Constructor.
    *
    * @param blendTree
    */
   AddBlendTreeEventAction( BlendTree& blendTree );

public slots:
   void onTriggered();
};

///////////////////////////////////////////////////////////////////////////////
