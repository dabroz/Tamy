/// @file   TamyEditor/GraphView.h
/// @brief  graphics view for displaying GraphLayouts
#pragma once

#include <QtWidgets\QGraphicsView>


///////////////////////////////////////////////////////////////////////////////

class MimeDataEncoder;
class QMimeData;

///////////////////////////////////////////////////////////////////////////////

class GraphView : public QGraphicsView 
{
   Q_OBJECT

private:
   static QBrush        s_selectionRectBrush;
   static QPen          s_selectionRectPen;

   MimeDataEncoder*     m_dataEncoder;
   float                m_scale;

   bool                 m_selectionDrawn;
   QPointF              m_selectionStart;
   QPointF              m_selectionEnd;
   QPoint               m_mousePos;

public:
   /**
    * Constructor.
    *
    * @param parentWidget
    */
   GraphView( QWidget* parentWidget );
   ~GraphView();

   /**
    * Sets a new data encoder.
    *
    * @param dataEncoder
    */
   void setDataEncoder( MimeDataEncoder* dataEncoder );

signals:
   void changed( const QMimeData *mimeData = 0 );

protected:
   // -------------------------------------------------------------------------
   // QGraphicsView implementation
   // -------------------------------------------------------------------------
   void keyPressEvent( QKeyEvent* event );
   void mouseDoubleClickEvent( QMouseEvent* event );
   void mousePressEvent( QMouseEvent* mouseEvent );
   void mouseReleaseEvent( QMouseEvent* mouseEvent );
   void mouseMoveEvent( QMouseEvent* mouseEvent );
   void focusInEvent( QFocusEvent* event );
   void focusOutEvent( QFocusEvent* event );
   void wheelEvent( QWheelEvent* event );
   void drawForeground( QPainter* painter, const QRectF& rect );
   void dragEnterEvent( QDragEnterEvent *event );
   void dragMoveEvent( QDragMoveEvent *event );
   void dropEvent( QDropEvent *event );

private:
   void getSelectionRect( QRectF& outRect ) const;
};

///////////////////////////////////////////////////////////////////////////////
