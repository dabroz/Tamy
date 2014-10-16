/// @file   TamyEditor/BlendTreeVariablesListModel.h
/// @brief  a data model implementation used by the BlendTreeVariablesViewer
#pragma once

#include <QtGui\QStandardItemModel>


///////////////////////////////////////////////////////////////////////////////

class BlendTreeVariablesListModel : public QStandardItemModel
{
   Q_OBJECT

public:
   /**
    * Constructor.
    *
    * @param parent
    */
   BlendTreeVariablesListModel( QObject* parent );

   // -------------------------------------------------------------------------
   // QAbnstractItemModel implementation
   // -------------------------------------------------------------------------
   QMimeData*	mimeData( const QModelIndexList& indexes ) const;
};

///////////////////////////////////////////////////////////////////////////////
