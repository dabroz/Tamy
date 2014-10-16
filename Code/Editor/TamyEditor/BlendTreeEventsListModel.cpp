#include "BlendTreeEventsListModel.h"
#include "ReflectionObjectPtrMimeData.h"
#include "core-AI\BlendTreeEvent.h"
#include <QtCore\QMimeData>


///////////////////////////////////////////////////////////////////////////////

BlendTreeEventsListModel::BlendTreeEventsListModel( QObject* parent )
   : QStandardItemModel( parent )
{
}

///////////////////////////////////////////////////////////////////////////////

QMimeData* BlendTreeEventsListModel::mimeData( const QModelIndexList& indexes ) const
{
   if ( indexes.empty() )
   {
      return NULL;
   }

   QMimeData* data = new QMimeData();

   QStandardItem* item = itemFromIndex( indexes.first() );
   BlendTreeEvent* checkedEvent = ( BlendTreeEvent* )( item->data( 33 ).value< void* >() );

   ReflectionObjectPtrMimeData::save( checkedEvent, *data );

   return data;
}

///////////////////////////////////////////////////////////////////////////////
