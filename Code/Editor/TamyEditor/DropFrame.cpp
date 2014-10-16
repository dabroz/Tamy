#include "DropFrame.h"
#include "MimeDataEncoder.h"
#include <QDragEnterEvent>
#include <QUrl>


///////////////////////////////////////////////////////////////////////////////

DropFrame::DropFrame( QWidget *parent, MimeDataEncoder* dataEncoder )
   : QFrame( parent )
   , m_dataEncoder( dataEncoder )
{
   setAcceptDrops( true );
}

///////////////////////////////////////////////////////////////////////////////

DropFrame::~DropFrame()
{
   delete m_dataEncoder;
   m_dataEncoder = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void DropFrame::dragEnterEvent( QDragEnterEvent *event )
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

void DropFrame::dragMoveEvent( QDragMoveEvent *event )
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

void DropFrame::dropEvent( QDropEvent *event )
{
   const QMimeData* mimeData = event->mimeData();

   m_dataEncoder->load( *mimeData );

   setBackgroundRole( QPalette::Light );
   event->acceptProposedAction();

   emit changed();
}

///////////////////////////////////////////////////////////////////////////////
