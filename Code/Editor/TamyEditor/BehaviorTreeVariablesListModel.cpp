#include "BehaviorTreeVariablesListModel.h"
#include "ReflectionObjectPtrMimeData.h"
#include "core-AI\BehTreeVariable.h"
#include <QtCore\QMimeData>


///////////////////////////////////////////////////////////////////////////////

BehaviorTreeVariablesListModel::BehaviorTreeVariablesListModel( QObject* parent )
   : QStandardItemModel( parent )
{
}

///////////////////////////////////////////////////////////////////////////////

QMimeData* BehaviorTreeVariablesListModel::mimeData( const QModelIndexList& indexes ) const
{
   if ( indexes.empty() )
   {
      return NULL;
   }

   QMimeData* data = new QMimeData();

   QStandardItem* item = itemFromIndex( indexes.first() );
   BehaviorTreeVariable* checkedVariable = ( BehaviorTreeVariable* )( item->data( 33 ).value< void* >() );

   ReflectionObjectPtrMimeData::save( checkedVariable, *data );

   return data;
}

///////////////////////////////////////////////////////////////////////////////
