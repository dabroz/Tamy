/// @file   TamyEditor/BehaviorTreeVariablesListModel.h
/// @brief  a data model implementation used by the BehaviorTreeVariablesViewer
#pragma once

#include <QtGui\QStandardItemModel>


///////////////////////////////////////////////////////////////////////////////

class BehaviorTreeVariablesListModel : public QStandardItemModel
{
   Q_OBJECT

public:
   /**
    * Constructor.
    *
    * @param parent
    */
   BehaviorTreeVariablesListModel( QObject* parent );

   // -------------------------------------------------------------------------
   // QAbnstractItemModel implementation
   // -------------------------------------------------------------------------
   QMimeData*	mimeData( const QModelIndexList& indexes ) const;
};

///////////////////////////////////////////////////////////////////////////////
