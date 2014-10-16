/// @file   TamyEditor/AnimationEventsEditor.h
/// @brief  animation events editor
#pragma once

#include "BTBFEditor.h"
#include "core\ReflectionObjectChangeListener.h"
#include "core-AI\BlendTreeAnimationListener.h"
#include "core\TimeDependent.h"
#include <QtWidgets\QAction>
#include <QtGui\QBrush>
#include <QtGui\QPen>


///////////////////////////////////////////////////////////////////////////////

class BlendTreeAnimation;
class BlendTreeEvent;
class AnimationEventsTimeline;
class BlendTreeAnimationEvent;
class QGraphicsScene;
class QPropertiesView;
class QSlider;
class QPushButton;
class SkeletonAnimationPlayer;

///////////////////////////////////////////////////////////////////////////////

/**
 * Animation events editor.
 */
class AnimationEventsEditor : public BTBFEditor, public ReflectionObjectChangeListener, public BlendTreeAnimationListener, public TimeDependent
{
   Q_OBJECT

private:
   BlendTreeAnimation&        m_animationNode;

   AnimationEventsTimeline*   m_view;
   QGraphicsScene*            m_eventsScene;
   QPropertiesView*           m_eventProperties;
   QSlider*                   m_slider;
   QPushButton*               m_animationToggleButton;

   bool                       m_animationToggled;
   BlendTreeEditor*           m_mainEditor;
   SkeletonAnimationPlayer*   m_customAnimationPlayer;

public:
   /**
    * Constructor.
    *
    * @param animationNode
    */
   AnimationEventsEditor( BlendTreeAnimation& animationNode );
   ~AnimationEventsEditor();

   // -------------------------------------------------------------------------
   // BTBFEditor implementation
   // -------------------------------------------------------------------------
   void initialize( BlendTreeEditor& mainEditor );
   void deinitialize( BlendTreeEditor& mainEditor );
   BlendTreeNode* getNode() const;

   // -------------------------------------------------------------------------
   // ReflectionObjectChangeListener implementation
   // -------------------------------------------------------------------------
   void onObservedPropertyChanged( ReflectionProperty& property );

   // -------------------------------------------------------------------------
   // BlendTreeAnimationListener implementation
   // -------------------------------------------------------------------------
   void onEventAdded( BlendTreeAnimationEvent* btaEvent );
   void onEventRemoved( BlendTreeAnimationEvent* btaEvent );
   void onSyncPointAdded( BlendTreeSyncPoint* syncPoint );
   void onSyncPointRemoved( BlendTreeSyncPoint* syncPoint );

   // -------------------------------------------------------------------------
   // TimeDependent implementation
   // -------------------------------------------------------------------------
   void update( float timeElapsed );

public slots:
   void onShowContextMenu( const QPoint& pos );
   void onItemSelected();
   void timeValueChanged( int timeInMillisecs );
   void onAnimationToggled( bool );
};

///////////////////////////////////////////////////////////////////////////////

class AddAnimationEventAction : public QAction
{
   Q_OBJECT

private:
   BlendTreeAnimation&        m_animationNode;
   BlendTreeEvent*            m_btEvent;
   float                      m_eventTime;

public:
   /**
    * Constructor.
    *
    * @param animationNode
    * @param btEvent
    */
   AddAnimationEventAction( BlendTreeAnimation& animationNode, BlendTreeEvent* btEvent, float eventTime );

public slots:
   void onTriggered();
};

///////////////////////////////////////////////////////////////////////////////

class AddAnimationSyncPointAction : public QAction
{
   Q_OBJECT

private:
   BlendTreeAnimation&        m_animationNode;
   BlendTreeEvent*            m_btEvent;
   float                      m_eventTime;

public:
   /**
    * Constructor.
    *
    * @param animationNode
    * @param btEvent
    */
   AddAnimationSyncPointAction( BlendTreeAnimation& animationNode, BlendTreeEvent* btEvent, float eventTime );

public slots:
   void onTriggered();
};

///////////////////////////////////////////////////////////////////////////////
