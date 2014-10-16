#include "DropArea.h"
#include "MimeDataEncoder.h"
#include <QDragEnterEvent>
#include <QUrl>


///////////////////////////////////////////////////////////////////////////////

DropArea::DropArea( QWidget *parent, MimeDataEncoder* dataEncoder )
: QLabel( parent )
, m_dataEncoder( dataEncoder )
{
   setMinimumSize( 20, 20 );
   setFrameStyle( QFrame::Sunken | QFrame::StyledPanel );
   setAlignment( Qt::AlignLeft );
   setAcceptDrops( true );
   setAutoFillBackground( true );
   clear();
}

///////////////////////////////////////////////////////////////////////////////

DropArea::~DropArea()
{
   delete m_dataEncoder;
   m_dataEncoder = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void DropArea::setDataEncoder( MimeDataEncoder* dataEncoder )
{
   delete m_dataEncoder;
   m_dataEncoder = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void DropArea::dragEnterEvent( QDragEnterEvent *event )
{
   const QMimeData* mimeData = event->mimeData();
   if ( m_dataEncoder->canAccept( *mimeData ) )
   {
      setBackgroundRole( QPalette::Highlight );
      event->acceptProposedAction();
   }
   else
   {
      event->ignore();
   }
}

///////////////////////////////////////////////////////////////////////////////

void DropArea::dragMoveEvent( QDragMoveEvent *event )
{
   const QMimeData* mimeData = event->mimeData();
   if ( m_dataEncoder->canAccept( *mimeData ) )
   {
      event->acceptProposedAction();
   }
   else
   {
      event->ignore();
   }
}

///////////////////////////////////////////////////////////////////////////////

void DropArea::dropEvent( QDropEvent *event )
{
   const QMimeData* mimeData = event->mimeData();

   m_dataEncoder->load( *mimeData );

   setBackgroundRole( QPalette::Light );
   event->acceptProposedAction();

   emit changed();
}

///////////////////////////////////////////////////////////////////////////////

void DropArea::dragLeaveEvent( QDragLeaveEvent *event )
{
   clear();
   event->accept();
}

///////////////////////////////////////////////////////////////////////////////

void DropArea::clear()
{
   setBackgroundRole( QPalette::Light );

   emit changed();
}

///////////////////////////////////////////////////////////////////////////////
