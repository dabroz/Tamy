/// @file   TamyEditor/AnimationEventsTimeline.h
/// @brief  a custom scene view used by the AnimationEventsEditor
#pragma once

#include <QtWidgets\QGraphicsView>
#include <QtGui\QPen>


///////////////////////////////////////////////////////////////////////////////

class QSlider;

///////////////////////////////////////////////////////////////////////////////

class AnimationEventsTimeline : public QGraphicsView
{
   Q_OBJECT

private:
   static QPen                s_markerPen;
   static QPen                s_primaryTimeLinesPen;
   static QPen                s_secondaryTimeLinesPen;
   static QPen                s_timeValuesPen;
   static QFont               s_font;


   QSlider*                   m_slider;
   float                      m_timeBarPos;

public:
   /**
    * Constructor.
    *
    * @param parentWidget
    */
   AnimationEventsTimeline( QWidget* parentWidget );
   ~AnimationEventsTimeline();

   /**
    * Sets the slider instance this widget should track.
    *
    * @param slider
    */
   void setSlider( QSlider* slider );

   /**
    * Adjusts view rectangle so that it stays focused on the same spot of the scene.
    */
   void adjustRect();

   /**
    * Maps event time to a position in the viewport.
    *
    * @param eventTime
    * @return eventTime
    */
   float timeToViewportPos( float eventTime ) const;

   /**
    * Maps a position in the viewport to an event time.
    *
    * @param viewportPos
    * @return eventTime
    */
   float viewportPosToTime( float viewportPos ) const;

public slots:
   void timeValueChanged( int timeInMillisecs );

protected:
   // -------------------------------------------------------------------------
   // QGraphicsView implementation
   // -------------------------------------------------------------------------
   void resizeEvent( QResizeEvent* event );
   void drawBackground( QPainter* painter, const QRectF& rect );

};

///////////////////////////////////////////////////////////////////////////////
