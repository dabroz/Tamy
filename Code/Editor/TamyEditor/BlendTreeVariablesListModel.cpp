#include "BlendTreeVariablesListModel.h"
#include "ReflectionObjectPtrMimeData.h"
#include "core-AI\BlendTreeVariable.h"
#include <QtCore\QMimeData>


///////////////////////////////////////////////////////////////////////////////

BlendTreeVariablesListModel::BlendTreeVariablesListModel( QObject* parent )
   : QStandardItemModel( parent )
{
}

///////////////////////////////////////////////////////////////////////////////

QMimeData* BlendTreeVariablesListModel::mimeData( const QModelIndexList& indexes ) const
{
   if ( indexes.empty() )
   {
      return NULL;
   }

   QMimeData* data = new QMimeData();

   QStandardItem* item = itemFromIndex( indexes.first() );
   BlendTreeVariable* checkedVariable = ( BlendTreeVariable* )( item->data( 33 ).value< void* >() );

   ReflectionObjectPtrMimeData::save( checkedVariable, *data );

   return data;
}

///////////////////////////////////////////////////////////////////////////////
