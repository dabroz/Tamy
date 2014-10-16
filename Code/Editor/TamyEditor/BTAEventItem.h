/// @file   TamyEditor/BTAEventItem.h
/// @brief  a graphics item that represents an animation event
#pragma once

#include <QtWidgets\QGraphicsItem>
#include <QtWidgets\QAction>
#include "core\ReflectionObjectChangeListener.h"


///////////////////////////////////////////////////////////////////////////////

class BlendTreeAnimationEvent;
class BlendTreeSyncPoint;
class AnimationEventsTimeline;
class BlendTreeAnimation;
class QPropertiesView;

///////////////////////////////////////////////////////////////////////////////

class BTAEventItem : public QGraphicsItem
{

public:
   virtual ~BTAEventItem() {}

   /**
    * Returns an event this item represents ( can be an animation event, a sync point - can't be certain for sure at this level ).
    */
   virtual void* getEventPtr() const = 0;

   /**
    * Creates an action that will remove the represented animation event from the animation node that owns it.
    *
    * @param animationNode       parent animation node
    */
   virtual QAction* createRemovalAction( BlendTreeAnimation& animationNode ) = 0;

   /**
    * Displays the properties of the specified animation event.
    *
    * @param propertiesView
    */
   virtual void viewProperties( QPropertiesView* propertiesView ) = 0;

   /**
    * Recalculates items bounds.
    */
   virtual void calculateBounds() = 0;
};

///////////////////////////////////////////////////////////////////////////////

/**
 * This item represents regular animation events.
 */
class BTAEAnimationEventItem : public BTAEventItem, public ReflectionObjectChangeListener
{
private:
   BlendTreeAnimationEvent*      m_btaEvent;
   AnimationEventsTimeline*      m_view;

   QRectF                        m_localBounds;
   QRectF                        m_globalBounds;

public:
   /**
    * Constructor.
    *
    * @param btaEvent
    * @param view
    */
   BTAEAnimationEventItem( BlendTreeAnimationEvent* btaEvent, AnimationEventsTimeline* view );
   ~BTAEAnimationEventItem();

   // -------------------------------------------------------------------------
   // BTAEventItem implementation
   // -------------------------------------------------------------------------
   void calculateBounds();
   inline void* getEventPtr() const { return m_btaEvent; }
   QAction* createRemovalAction( BlendTreeAnimation& animationNode );
   void viewProperties( QPropertiesView* propertiesView );

   // -------------------------------------------------------------------------
   // QGraphicsItem implementation
   // -------------------------------------------------------------------------
   inline QRectF boundingRect() const { return m_globalBounds; }
   void paint( QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget );

   // -------------------------------------------------------------------------
   // QGraphicsItem implementation
   // -------------------------------------------------------------------------
   void mouseMoveEvent( QGraphicsSceneMouseEvent* event );
   void hoverEnterEvent( QGraphicsSceneHoverEvent * event );

   // -------------------------------------------------------------------------
   // ReflectionObjectChangeListener implementation
   // -------------------------------------------------------------------------
   void onObservedPropertyChanged( ReflectionProperty& property );
};

///////////////////////////////////////////////////////////////////////////////

/**
 * This item represents animation synchronization points.
 */
class BTASyncPointItem : public BTAEventItem, public ReflectionObjectChangeListener
{
private:
   BlendTreeSyncPoint*           m_btaSyncPoint;
   AnimationEventsTimeline*      m_view;

   QRectF                        m_localBounds;
   QRectF                        m_globalBounds;

public:
   /**
    * Constructor.
    *
    * @param btaSyncPoint
    * @param view
    */
   BTASyncPointItem( BlendTreeSyncPoint* btaSyncPoint, AnimationEventsTimeline* view );
   ~BTASyncPointItem();

   // -------------------------------------------------------------------------
   // BTAEventItem implementation
   // -------------------------------------------------------------------------
   void calculateBounds();
   inline void* getEventPtr() const { return m_btaSyncPoint; }
   QAction* createRemovalAction( BlendTreeAnimation& animationNode );
   void viewProperties( QPropertiesView* propertiesView );

   // -------------------------------------------------------------------------
   // QGraphicsItem implementation
   // -------------------------------------------------------------------------
   inline QRectF boundingRect() const { return m_globalBounds; }
   void paint( QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget );

   // -------------------------------------------------------------------------
   // QGraphicsItem implementation
   // -------------------------------------------------------------------------
   void mouseMoveEvent( QGraphicsSceneMouseEvent* event );
   void hoverEnterEvent( QGraphicsSceneHoverEvent * event );

   // -------------------------------------------------------------------------
   // ReflectionObjectChangeListener implementation
   // -------------------------------------------------------------------------
   void onObservedPropertyChanged( ReflectionProperty& property );
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

class RemoveAnimationEventAction : public QAction
{
   Q_OBJECT

private:
   BlendTreeAnimation&        m_animationNode;
   BlendTreeAnimationEvent*   m_btaAnimEvent;

public:
   /**
    * Constructor.
    *
    * @param animationNode
    * @param btaAnimEvent
    */
   RemoveAnimationEventAction( BlendTreeAnimation& animationNode, BlendTreeAnimationEvent* btaAnimEvent );

public slots:
   void onTriggered();
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

class RemoveAnimationSyncPoint : public QAction
{
   Q_OBJECT

private:
   BlendTreeAnimation&        m_animationNode;
   BlendTreeSyncPoint*        m_btaSyncPoint;

public:
   /**
    * Constructor.
    *
    * @param animationNode
    * @param btaSyncPoint
    */
   RemoveAnimationSyncPoint( BlendTreeAnimation& animationNode, BlendTreeSyncPoint* btaSyncPoint );

public slots:
   void onTriggered();
};

///////////////////////////////////////////////////////////////////////////////
