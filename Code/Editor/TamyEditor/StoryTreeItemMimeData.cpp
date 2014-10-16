#include "StoryTreeItemMimeData.h"
#include <QtCore\QMimeData>
#include <QtCore\QDataStream>


/////////////////////////////////////////////////////////////////////////////////

StoryTreeItemMimeData::StoryTreeItemMimeData( StoryBrowserTreeItem*& itemPtr )
   : m_itemPtr( itemPtr )
{
}

/////////////////////////////////////////////////////////////////////////////////

void StoryTreeItemMimeData::load( const QMimeData& data )
{
   m_itemPtr = StoryTreeItemMimeDataUtil::decode( &data );
}

/////////////////////////////////////////////////////////////////////////////////

bool StoryTreeItemMimeData::canAccept( const QMimeData& data ) const
{
   return StoryTreeItemMimeDataUtil::checkDataPresence( &data );
}

/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////

void StoryTreeItemMimeDataUtil::encode( const StoryBrowserTreeItem* itemPtr, QMimeData* mimeData )
{
   QByteArray encodedData;
   QDataStream stream( &encodedData, QIODevice::WriteOnly );

   uint movedItemPtr = (uint)itemPtr;
   stream << movedItemPtr;

   mimeData->setData( "TamyEditor/StoryBrowserTreeWidget", encodedData );
}

/////////////////////////////////////////////////////////////////////////////////

StoryBrowserTreeItem* StoryTreeItemMimeDataUtil::decode( const QMimeData* mimeData )
{
   // decode the stream, removing nodes from their previous location and batching them for addition
   QByteArray encodedData = mimeData->data( "TamyEditor/StoryBrowserTreeWidget" );
   QDataStream stream( &encodedData, QIODevice::ReadOnly );

   if ( stream.atEnd() ) 
   {      
      return NULL;
   }

   uint movedItemPtr;
   stream >> movedItemPtr;

   StoryBrowserTreeItem* decodedItemPtr = ( StoryBrowserTreeItem* )movedItemPtr;
   return decodedItemPtr;
}

/////////////////////////////////////////////////////////////////////////////////

bool StoryTreeItemMimeDataUtil::checkDataPresence( const QMimeData* mimeData )
{
   QByteArray encodedData = mimeData->data( "TamyEditor/StoryBrowserTreeWidget" );
   QDataStream stream( &encodedData, QIODevice::ReadOnly );

   return !stream.atEnd();
}

/////////////////////////////////////////////////////////////////////////////////
