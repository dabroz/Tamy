/// @file   TamyEditor/BlendTreeEventsListModel.h
/// @brief  a data model implementation used by the BlendTreeEventsViewer
#pragma once

#include <QtGui\QStandardItemModel>


///////////////////////////////////////////////////////////////////////////////

class BlendTreeEventsListModel : public QStandardItemModel
{
   Q_OBJECT

public:
   /**
    * Constructor.
    *
    * @param parent
    */
   BlendTreeEventsListModel( QObject* parent );

   // -------------------------------------------------------------------------
   // QAbnstractItemModel implementation
   // -------------------------------------------------------------------------
   QMimeData*	mimeData( const QModelIndexList& indexes ) const;
};

///////////////////////////////////////////////////////////////////////////////
