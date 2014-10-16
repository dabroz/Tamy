/// @file   TamyEditor\DropArea.h
/// @brief  A widget that can accepts various types of drag&drops
#pragma once

#include <QtWidgets\QLabel>


///////////////////////////////////////////////////////////////////////////////

class QMimeData;
class MimeDataEncoder;

///////////////////////////////////////////////////////////////////////////////

/**
 * A widget that accepts drag&drops.
 */
class DropArea : public QLabel
{
   Q_OBJECT

private:
   QLabel*           m_label;
   MimeDataEncoder*  m_dataEncoder;

public:
   /**
    * Constructor.
    */
   DropArea( QWidget* parent, MimeDataEncoder* dataEncoder );
   ~DropArea();

   /**
    * Sets a different data encoder.
    *
    * @param dataEncoder
    */
   void setDataEncoder( MimeDataEncoder* dataEncoder );

public slots:
   void clear();

signals:
   void changed( const QMimeData *mimeData = 0 );

protected:
   void dragEnterEvent( QDragEnterEvent *event );
   void dragMoveEvent( QDragMoveEvent *event );
   void dragLeaveEvent( QDragLeaveEvent *event );
   void dropEvent( QDropEvent *event );
};

///////////////////////////////////////////////////////////////////////////////
