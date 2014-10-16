#include "SceneItemMimeData.h"
#include "SceneTreeItem.h"
#include <QtCore\QMimeData>
#include <QtCore\QDataStream>


/////////////////////////////////////////////////////////////////////////////////

SceneItemMimeData::SceneItemMimeData( SceneTreeItem*& itemPtr )
   : m_itemPtr( itemPtr )
{
}

/////////////////////////////////////////////////////////////////////////////////

void SceneItemMimeData::load( const QMimeData& data )
{
   m_itemPtr = SceneItemMimeDataUtil::decode( &data );
}

/////////////////////////////////////////////////////////////////////////////////

bool SceneItemMimeData::canAccept( const QMimeData& data ) const
{
   return SceneItemMimeDataUtil::checkDataPresence( &data );
}

/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////

void SceneItemMimeDataUtil::encode( const SceneTreeItem* itemPtr, QMimeData* mimeData )
{
   QByteArray encodedData;
   QDataStream stream( &encodedData, QIODevice::WriteOnly );

   uint movedItemPtr = (uint)itemPtr;
   stream << movedItemPtr;

   mimeData->setData( "TamyEditor/SceneTreeWidget", encodedData );
}

/////////////////////////////////////////////////////////////////////////////////

SceneTreeItem* SceneItemMimeDataUtil::decode( const QMimeData* mimeData )
{
   // decode the stream, removing nodes from their previous location and batching them for addition
   QByteArray encodedData = mimeData->data( "TamyEditor/SceneTreeWidget" );
   QDataStream stream( &encodedData, QIODevice::ReadOnly );

   if ( stream.atEnd() ) 
   {      
      return NULL;
   }

   uint movedItemPtr;
   stream >> movedItemPtr;

   SceneTreeItem* decodedItemPtr = ( SceneTreeItem* )movedItemPtr;
   return decodedItemPtr;
}

/////////////////////////////////////////////////////////////////////////////////

bool SceneItemMimeDataUtil::checkDataPresence( const QMimeData* mimeData )
{
   QByteArray encodedData = mimeData->data( "TamyEditor/SceneTreeWidget" );
   QDataStream stream( &encodedData, QIODevice::ReadOnly );

   return !stream.atEnd();
}

/////////////////////////////////////////////////////////////////////////////////
