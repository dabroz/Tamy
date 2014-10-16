#include "GraphView.h"
#include <QWheelEvent>
#include <QtGui\QPainter>
#include <QtGui\QPainterPath>
#include "core\Algorithms.h"
#include "core\Assert.h"
#include "GraphLayout.h"
#include "MimeDataEncoder.h"


///////////////////////////////////////////////////////////////////////////////

QBrush GraphView::s_selectionRectBrush( QColor( 255, 226, 96, 32 ) );
QPen GraphView::s_selectionRectPen( QColor( 255, 226, 96, 172 ), 2 );

///////////////////////////////////////////////////////////////////////////////

GraphView::GraphView( QWidget* parentWidget )
   : QGraphicsView( parentWidget )
   , m_scale( 1.0f )
   , m_selectionDrawn( false )
   , m_dataEncoder( NULL )
{
   setAcceptDrops( true );
   setDragMode( QGraphicsView::ScrollHandDrag );
   setContextMenuPolicy( Qt::PreventContextMenu );
   setMouseTracking( true );
}

///////////////////////////////////////////////////////////////////////////////

GraphView::~GraphView()
{
   delete m_dataEncoder;
   m_dataEncoder = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void GraphView::setDataEncoder( MimeDataEncoder* dataEncoder )
{
   delete m_dataEncoder;
   m_dataEncoder = dataEncoder;
}

///////////////////////////////////////////////////////////////////////////////

void GraphView::mouseDoubleClickEvent( QMouseEvent* event )
{
   QGraphicsItem* item = itemAt( event->pos() );
   if ( item )
   {
      QTransform scaleTransform;
      setTransform( scaleTransform, false );
      centerOn( item );
   }

   QGraphicsView::mouseDoubleClickEvent( event );
}

///////////////////////////////////////////////////////////////////////////////

void GraphView::wheelEvent( QWheelEvent* event )
{
   int d = event->delta();
   float df = (float)d / 960.0f;
   float s;
   if ( df > 0 )
   {
      s = df + 1.0f;
   }
   else
   {
      s = 1.0f / (-df + 1.0f);
   }

   QTransform scaleTransform;
   scaleTransform.scale( s, s );
   setTransform( scaleTransform, true );
   event->accept();
}

///////////////////////////////////////////////////////////////////////////////

void GraphView::keyPressEvent( QKeyEvent* event )
{
   QGraphicsView::keyPressEvent( event );

   QGraphicsScene* s = scene();
   if ( s && event->matches( QKeySequence::Bold ) )
   {
      GraphLayout* layout = static_cast< GraphLayout* >( s );

      QMenu* contextMenu = new QMenu( this );
      layout->createContextMenu( contextMenu, mapToScene( m_mousePos ) );
      contextMenu->popup( mapToGlobal( m_mousePos ) );
   }
}

///////////////////////////////////////////////////////////////////////////////

void GraphView::mousePressEvent( QMouseEvent* mouseEvent )
{
   QGraphicsScene* s = scene();
   if ( s && mouseEvent->button() == Qt::RightButton && !m_selectionDrawn )
   {
      QPointF scenePos = mapToScene( mouseEvent->pos() );
      QGraphicsItem* item = s->itemAt( scenePos, QTransform() );
      if ( !item )
      {
         // start dragging only if nothing's selected
         m_selectionDrawn = true;
         m_selectionStart = scenePos;
         m_selectionEnd = scenePos;
      }
   }

   m_mousePos = mouseEvent->pos();

   QGraphicsView::mousePressEvent( mouseEvent );
}

///////////////////////////////////////////////////////////////////////////////

void GraphView::mouseReleaseEvent( QMouseEvent* mouseEvent )
{
   if ( m_selectionDrawn )
   {
      if ( m_selectionStart != m_selectionEnd )
      {
         // commit selection
         QGraphicsScene* s = scene();
         ASSERT( s != NULL );

         QRectF selectionRect;
         getSelectionRect( selectionRect );

         QPainterPath selectionPath;
         selectionPath.addRoundedRect( selectionRect, 4.0f, 4.0f );

         s->setSelectionArea( selectionPath );
      }

      m_selectionDrawn = false;
      update();
   }

   m_mousePos = mouseEvent->pos();

   QGraphicsView::mouseReleaseEvent( mouseEvent );
}

///////////////////////////////////////////////////////////////////////////////

void GraphView::mouseMoveEvent( QMouseEvent* mouseEvent )
{
   if ( m_selectionDrawn )
   {
      m_selectionEnd = mapToScene( mouseEvent->pos() );
      update();
   }

   m_mousePos = mouseEvent->pos();

   QGraphicsView::mouseMoveEvent( mouseEvent );
}

///////////////////////////////////////////////////////////////////////////////

void GraphView::focusInEvent( QFocusEvent* event )
{
   QGraphicsView::focusInEvent( event );

   // cancel selection drawing
   m_selectionDrawn = false;
}

///////////////////////////////////////////////////////////////////////////////

void GraphView::focusOutEvent( QFocusEvent* event )
{
   QGraphicsView::focusOutEvent( event );

   // cancel selection drawing
   m_selectionDrawn = false;
}

///////////////////////////////////////////////////////////////////////////////

void GraphView::drawForeground( QPainter* painter, const QRectF& rect )
{
   QGraphicsView::drawForeground( painter, rect );

   if ( m_selectionDrawn )
   {
      painter->save();
      painter->setRenderHint( QPainter::Antialiasing, true );

      QRectF selectionRect;
      getSelectionRect( selectionRect );
      painter->setBrush( GraphView::s_selectionRectBrush );
      painter->setPen( GraphView::s_selectionRectPen );
      painter->drawRoundedRect( selectionRect, 4.0f, 4.0f );

      painter->restore();
   }
}

///////////////////////////////////////////////////////////////////////////////

void GraphView::getSelectionRect( QRectF& outRect ) const
{
   float left, right, top, bottom;
   if ( m_selectionStart.x() < m_selectionEnd.x() )
   {
      left = m_selectionStart.x();
      right = m_selectionEnd.x();
   }
   else
   {
      right = m_selectionStart.x();
      left = m_selectionEnd.x();
   }

   if ( m_selectionStart.y() < m_selectionEnd.y() )
   {
      top = m_selectionStart.y();
      bottom = m_selectionEnd.y();
   }
   else
   {
      bottom = m_selectionStart.y();
      top = m_selectionEnd.y();
   }

   outRect.setCoords( left, top, right, bottom );
}

///////////////////////////////////////////////////////////////////////////////

void GraphView::dragEnterEvent( QDragEnterEvent *event )
{
   const QMimeData* mimeData = event->mimeData();
   if ( m_dataEncoder && m_dataEncoder->canAccept( *mimeData ) )
   {
      setBackgroundRole( QPalette::Highlight );
      event->acceptProposedAction();
   }
   else
   {
      QGraphicsView::dragEnterEvent( event );
   }
}

///////////////////////////////////////////////////////////////////////////////

void GraphView::dragMoveEvent( QDragMoveEvent *event )
{
   const QMimeData* mimeData = event->mimeData();
   if ( m_dataEncoder && m_dataEncoder->canAccept( *mimeData ) )
   {
      event->acceptProposedAction();
   }
   else
   {
      // maybe the scene will use it
      QGraphicsView::dragMoveEvent( event );
   }
}

///////////////////////////////////////////////////////////////////////////////

void GraphView::dropEvent( QDropEvent *event )
{
   const QMimeData* mimeData = event->mimeData();
   if ( m_dataEncoder && m_dataEncoder->canAccept( *mimeData ) )
   {
      m_dataEncoder->load( *mimeData );

      setBackgroundRole( QPalette::Light );
      event->acceptProposedAction();

      emit changed();
   }
   else
   {
      // maybe the scene will use it
      QGraphicsView::dropEvent( event );
   }
}

///////////////////////////////////////////////////////////////////////////////
