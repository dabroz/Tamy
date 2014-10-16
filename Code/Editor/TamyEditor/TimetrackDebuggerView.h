/// @file   TamyEditor/TimetrackDebuggerView.h
/// @brief  a graphics view dedicated to the TimetrackDebugger
#pragma once

#include <QtWidgets\QGraphicsView>


///////////////////////////////////////////////////////////////////////////////

class TimetrackDebuggerView : public QGraphicsView
{
   Q_OBJECT

private:
   static QBrush                          s_oddTrackBrush;
   static QPen                            s_oddTrackPen;
   static QPen                            s_timeLinesPen;
   static QPen                            s_timeValuesPen;
   static QFont                           s_font;

   float                                  m_trackHeight;
   bool                                   m_drawTimeTicks;
   
public:
   /**
    * Constructor.
    *
    * @param parent
    * @param trackHeight
    */
   TimetrackDebuggerView( QWidget* parent, float trackHeight );
   ~TimetrackDebuggerView();

   /**
    * Toggles time ticks drawing.
    *
    * @param enable
    */
   inline void drawTimeTicks( bool enable ) { m_drawTimeTicks = enable; }

   // -------------------------------------------------------------------------
   // QGraphicsView implementation
   // -------------------------------------------------------------------------
   void drawBackground( QPainter* painter, const QRectF& rect );
   void drawForeground( QPainter* painter, const QRectF& rect );
};

///////////////////////////////////////////////////////////////////////////////
