#include "MappingTable.h"

// animation
#include "core-AI\Skeleton.h"

// qt widgets
#include <QtCore\QMimeData>
#include <QtGui\qevent.h>

///////////////////////////////////////////////////////////////////////////////

MappingTable::MappingTable( QWidget* parent )
   : QTreeWidget( parent )
{
   QStringList headerLabels;
   headerLabels.push_back( tr( "Source chain name" ) );
   headerLabels.push_back( tr( "Source chain start" ) );
   headerLabels.push_back( tr( "Source chain end" ) );
   headerLabels.push_back( tr( "Target chain name" ) );
   headerLabels.push_back( tr( "Target chain start" ) );
   headerLabels.push_back( tr( "Target chain end" ) );
   setHeaderLabels( headerLabels );

   setDragEnabled( false );
   setAcceptDrops( true );
}

///////////////////////////////////////////////////////////////////////////////

bool MappingTable::verifyChain( BoneChainIdx chainIdx, const Skeleton* skeleton )
{
   const int startCols[2] = { BoneMappingCol_SourceChainStart, BoneMappingCol_TargetChainStart };
   const int endCols[2] = { BoneMappingCol_SourceChainEnd, BoneMappingCol_TargetChainEnd };

   const int startColIdx = startCols[chainIdx];
   const int endColIdx = endCols[chainIdx];

   const uint mappingsCount = topLevelItemCount();

   // reset the statuses
   bool status = true;
   for ( uint i = 0; i < mappingsCount; ++i )
   {
      resetChainStatus( i, startColIdx );
      resetChainStatus( i, endColIdx );
   }

   // make sure that the chain definitions are correct ( that the end bone is a successor of the start bone )
   for ( uint i = 0; i < mappingsCount; ++i )
   {
      QTreeWidgetItem* item = topLevelItem( i );

      std::string chainStart = item->text( startColIdx ).toStdString();
      std::string chainEnd = item->text( endColIdx ).toStdString();

      bool isBoneOrderCorrect = chainStart == chainEnd || skeleton->verifyParentChildRelation( chainStart, chainEnd );
      setChainStatus( i, startColIdx, isBoneOrderCorrect, "Bone order is incorrect" );
      setChainStatus( i, endColIdx, isBoneOrderCorrect, "Bone order is incorrect" );

      status &= isBoneOrderCorrect;
   }

   // Make sure the chains don't overlap.
   // In order to test that, take note which bones are mapped to chains.
   // If you find a bone already mapped to another chain, then we know that there's
   // an overlap
   Array< int > indexOfOwnerChain( skeleton->getBoneCount() );
   indexOfOwnerChain.resize( skeleton->getBoneCount(), -1 );
   for ( uint chainIdx = 0; chainIdx < mappingsCount; ++chainIdx )
   {
      QTreeWidgetItem* item = topLevelItem( chainIdx );
      std::string chainStart = item->text( startColIdx ).toStdString();
      std::string chainEnd = item->text( endColIdx ).toStdString();

      const int parentBoneIdx = skeleton->getBoneIndex( chainStart.c_str() );
      const int childBoneIdx = skeleton->getBoneIndex( chainEnd.c_str() );

      if ( parentBoneIdx < 0 || childBoneIdx < 0 )
      {
         continue;
      }

      for ( int boneIdx = childBoneIdx; boneIdx >= 0; boneIdx = skeleton->m_boneParentIndices[boneIdx] )
      {
         if ( indexOfOwnerChain[boneIdx] >= 0 )
         {
            char tmpStr[128];
            sprintf_s( tmpStr, "Bone '%s' is already assigned to chain %d", skeleton->m_boneNames[boneIdx].c_str(), indexOfOwnerChain[boneIdx] );
            setChainStatus( chainIdx, startColIdx, false, tmpStr );
            setChainStatus( chainIdx, endColIdx, false, tmpStr );

            status = false;
         }
         else
         {
            indexOfOwnerChain[boneIdx] = chainIdx;
         }

         if ( boneIdx == parentBoneIdx )
         {
            // we reached the end of the chain
            break;
         }
      }
   }

   return status;
}

///////////////////////////////////////////////////////////////////////////////

void MappingTable::resetChainStatus( uint chainIdx, int columnIdx )
{
   static QBrush regularBrush( QColor( 0, 0, 0 ) );

   QTreeWidgetItem* item = topLevelItem( chainIdx );
   item->setForeground( columnIdx, regularBrush );
   item->setToolTip( columnIdx, "" );
}

///////////////////////////////////////////////////////////////////////////////

void MappingTable::setChainStatus( uint chainIdx, int columnIdx, bool newStatus, const char* errorMsg )
{
   static QBrush errorBrush( QColor( 224, 100, 100 ) );

   QTreeWidgetItem* item = topLevelItem( chainIdx );
   if ( !newStatus )
   {
      item->setForeground( columnIdx, errorBrush );
      item->setToolTip( columnIdx, errorMsg );
   }
}

///////////////////////////////////////////////////////////////////////////////

void MappingTable::dragMoveEvent( QDragMoveEvent* event )
{
   const QMimeData* data = event->mimeData();

   QByteArray encodedData = data->data( "TamyEditor/MappingTreeWidget" );
   QDataStream stream( &encodedData, QIODevice::ReadOnly );

   if ( stream.atEnd() )
   {
      event->ignore();
      return;
   }

   // filter out the drops to invalid columns
   QPoint pos = event->pos();
   QTreeWidgetItem* dropPlaceItem = itemAt( pos.rx(), pos.ry() );
   if ( !dropPlaceItem )
   {
      event->ignore();
      return;
   }

   const int columnIdx = columnAt( pos.rx() );
   if ( columnIdx == BoneMappingCol_SourceChainName || columnIdx == BoneMappingCol_TargetChainName )
   {
      event->ignore();
      return;
   }

   bool isSourceBone;
   stream >> isSourceBone;

   if ( isSourceBone && ( columnIdx >= BoneMappingCol_TargetChainName ) )
   {
      event->ignore();
      return;
   }
   if ( !isSourceBone && ( columnIdx < BoneMappingCol_TargetChainName ) )
   {
      event->ignore();
      return;
   }

   QTreeWidget::dragMoveEvent( event );
}

///////////////////////////////////////////////////////////////////////////////

void MappingTable::dropEvent( QDropEvent* event )
{
   const QMimeData* data = event->mimeData();
   QByteArray encodedData = data->data( "TamyEditor/MappingTreeWidget" );
   QDataStream stream( &encodedData, QIODevice::ReadOnly );

   QPoint pos = event->pos();
   QTreeWidgetItem* dropPlaceItem = itemAt( pos.rx(), pos.ry() );
   const int columnIdx = columnAt( pos.rx() );

   bool isSourceBone;
   stream >> isSourceBone;

   QString boneName;
   stream >> boneName;
   dropPlaceItem->setText( columnIdx, boneName );

   event->accept();
}

///////////////////////////////////////////////////////////////////////////////

void MappingTable::addMapping()
{
   QStringList values;
   values.push_back( "newSourceChain" );
   values.push_back( "" );
   values.push_back( "" );
   values.push_back( "newTargetChain" );
   values.push_back( "" );
   values.push_back( "" );

   addTopLevelItem( new QTreeWidgetItem( values ) );
}

///////////////////////////////////////////////////////////////////////////////

void MappingTable::removeMapping()
{
   QList< QTreeWidgetItem*> itemsToRemove = selectedItems();
   foreach( QTreeWidgetItem* item, itemsToRemove )
   {
      const int itemIdx = indexOfTopLevelItem( item );
      takeTopLevelItem( itemIdx );
   }
}

///////////////////////////////////////////////////////////////////////////////
