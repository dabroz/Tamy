/// @file   TamyEditor/BlendTreeDebugEvent.h
/// @brief  graphics items that represent triggered events, displayed by a TimetrackDebugger
#pragma once

#include <QtWidgets\QGraphicsItem>


///////////////////////////////////////////////////////////////////////////////

class BlendTreeDebugEvent : public QGraphicsItem
{
private:
   QString           m_name;
   QRectF            m_globalBounds;

public:
   /**
    * Constructor.
    *
    * @param name
    * @param blockHeight
    * @param startTime
    * @param trackOffset
    */
   BlendTreeDebugEvent( const char* name, float blockHeight, float startTime, float trackOffset );

   // -------------------------------------------------------------------------
   // QGraphicsItem implementation
   // -------------------------------------------------------------------------
   inline QRectF boundingRect() const { return m_globalBounds; }
   void paint( QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget );
   void hoverEnterEvent( QGraphicsSceneHoverEvent* event );
};

///////////////////////////////////////////////////////////////////////////////

class BlendTreeDebugSyncPoint : public QGraphicsItem
{
private:
   QString           m_name;
   float             m_opaqueness;
   QRectF            m_globalBounds;

public:
   /**
    * Constructor.
    *
    * @param name
    * @param blockHeight
    * @param startTime
    * @param trackOffset
    * @param isOnEventsTrack
    */
   BlendTreeDebugSyncPoint( const char* name, float blockHeight, float startTime, float trackOffset, bool isOnEventsTrack );

   // -------------------------------------------------------------------------
   // QGraphicsItem implementation
   // -------------------------------------------------------------------------
   inline QRectF boundingRect() const { return m_globalBounds; }
   void paint( QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget );
   void hoverEnterEvent( QGraphicsSceneHoverEvent* event );
};

///////////////////////////////////////////////////////////////////////////////
