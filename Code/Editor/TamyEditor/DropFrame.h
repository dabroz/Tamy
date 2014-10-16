/// @file   TamyEditor\DropFrame.h
/// @brief  A frame that can accepts various types of drag&drops
#pragma once

#include <QtWidgets\QFrame>


///////////////////////////////////////////////////////////////////////////////

class QMimeData;
class MimeDataEncoder;

///////////////////////////////////////////////////////////////////////////////

/**
 * A frame that accepts drag&drops.
 */
class DropFrame : public QFrame
{
   Q_OBJECT

private:
   MimeDataEncoder*  m_dataEncoder;

public:
   /**
    * Constructor.
    */
   DropFrame( QWidget* parent, MimeDataEncoder* dataEncoder );
   ~DropFrame();

signals:
   void changed( const QMimeData *mimeData = 0 );

protected:
   void dragEnterEvent( QDragEnterEvent *event );
   void dragMoveEvent( QDragMoveEvent *event );
   void dropEvent( QDropEvent *event );
};

///////////////////////////////////////////////////////////////////////////////
