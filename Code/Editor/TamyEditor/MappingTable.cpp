#include "MappingTable.h"

// animation
#include "core-AI\Skeleton.h"
#include "core-AI\SkeletonMapper.h"
#include "core-AI\SkeletonMapperUtils.h"

// qt widgets
#include <QtCore\QMimeData>
#include <QtGui\qevent.h>


///////////////////////////////////////////////////////////////////////////////

MappingTable::MappingTable( QWidget* parent )
   : QTreeWidget( parent )
   , m_errorFlag( false )
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

   m_sourceChainSkeleton = new Skeleton();
   m_targetChainSkeleton = new Skeleton();
}

///////////////////////////////////////////////////////////////////////////////

MappingTable::~MappingTable()
{
   clearBuildResults();

   m_sourceChainSkeleton->removeReference();
   m_sourceChainSkeleton = NULL;

   m_targetChainSkeleton->removeReference();
   m_targetChainSkeleton = NULL;
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

      m_errorFlag = true;
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

   // if we dropped a bone onto one of the fields, and the other one doesn't contain an entry yet,
   // populate both at once
   if ( columnIdx == BoneMappingCol_SourceChainStart && dropPlaceItem->text( BoneMappingCol_SourceChainEnd ).isEmpty() )
   {
      dropPlaceItem->setText( BoneMappingCol_SourceChainEnd, boneName );
   }
   else if ( columnIdx == BoneMappingCol_SourceChainEnd && dropPlaceItem->text( BoneMappingCol_SourceChainStart ).isEmpty() )
   {
      dropPlaceItem->setText( BoneMappingCol_SourceChainStart, boneName );
   }
   else if ( columnIdx == BoneMappingCol_TargetChainStart && dropPlaceItem->text( BoneMappingCol_TargetChainEnd ).isEmpty() )
   {
      dropPlaceItem->setText( BoneMappingCol_TargetChainEnd, boneName );
   }
   else if ( columnIdx == BoneMappingCol_TargetChainEnd && dropPlaceItem->text( BoneMappingCol_TargetChainStart ).isEmpty() )
   {
      dropPlaceItem->setText( BoneMappingCol_TargetChainStart, boneName );
   }

   event->accept();
}

///////////////////////////////////////////////////////////////////////////////

void MappingTable::addMapping()
{
   // Finding a unique name for the chain is a matter of appending the number of entries
   // to the core of the name.
   // @see the comment in updateChainNames() for details
   char tmpSourceName[128];
   char tmpTargetName[128];
   sprintf( tmpSourceName, "source_%d", topLevelItemCount() );
   sprintf( tmpTargetName, "target_%d", topLevelItemCount() );

   QStringList values;
   values.push_back( tmpSourceName );
   values.push_back( "" );
   values.push_back( "" );
   values.push_back( tmpTargetName );
   values.push_back( "" );
   values.push_back( "" );

   addTopLevelItem( new QTreeWidgetItem( values ) );
}

///////////////////////////////////////////////////////////////////////////////

void MappingTable::updateChainNames()
{
   // Go through chains and rename them, assigning each chain a name that consists
   // of the core that signifies its mapping place ( source/target ) and an integral
   // number corresponding to the row in the table.
   // This way we'll maintain the order and allow for an easy addition of new chains with unique names
   // into the table.

   char tmpSourceName[128];
   char tmpTargetName[128];

   const uint mappingsCount = topLevelItemCount();
   for ( uint i = 0; i < mappingsCount; ++i )
   {
      sprintf( tmpSourceName, "source_%d", i );
      sprintf( tmpTargetName, "target_%d", i );

      QTreeWidgetItem* item = topLevelItem( i );
      item->setText( BoneMappingCol_SourceChainName, tmpSourceName );
      item->setText( BoneMappingCol_TargetChainName, tmpTargetName );
   }
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

   updateChainNames();
}

///////////////////////////////////////////////////////////////////////////////

void MappingTable::setMapping( const SkeletonMapper& mapper )
{
   clear();
   /*
   const Skeleton* sourceChainSkeleton = mapper.getSourceChainSkeleton();
   const Skeleton* targetChainSkeleton = mapper.getTargetChainSkeleton();

   const uint targetChainsCount = mapper.getTargetChainsCount();
   for ( uint targetChainIdx = 0; targetChainIdx < targetChainsCount; ++targetChainIdx )
   {
      const int sourceChainIdx = mapper.getMappingForChain( targetChainIdx );
      if ( sourceChainIdx < 0 )
      {
         continue;
      }

      const SkeletonBoneChain* targetChain = mapper.getTargetChain( targetChainIdx );
      const SkeletonBoneChain* sourceChain = mapper.getSourceChain( sourceChainIdx );

      ASSERT( targetChain && sourceChain && sourceChain != targetChain );

      const char* sourceChainStart = mapper.m_sourceSkeleton->m_boneNames[sourceChain->m_firstBoneIdx].c_str();
      const char* sourceChainEnd = mapper.m_sourceSkeleton->m_boneNames[sourceChain->m_lastBoneIdx].c_str();
      const char* targetChainStart = mapper.m_targetSkeleton->m_boneNames[targetChain->m_firstBoneIdx].c_str();
      const char* targetChainEnd = mapper.m_targetSkeleton->m_boneNames[targetChain->m_lastBoneIdx].c_str();

      QStringList boneLabels;
      boneLabels.push_back( sourceChainSkeleton->m_boneNames[sourceChainIdx].c_str() );
      boneLabels.push_back( sourceChainStart );
      boneLabels.push_back( sourceChainEnd );
      boneLabels.push_back( targetChainSkeleton->m_boneNames[targetChainIdx].c_str() );
      boneLabels.push_back( targetChainStart );
      boneLabels.push_back( targetChainEnd );
      addTopLevelItem( new QTreeWidgetItem() );
   }*/

   updateChainNames();
}

///////////////////////////////////////////////////////////////////////////////

void MappingTable::verifyChain( BoneChainIdx chainIdx, const Skeleton* skeleton )
{
   const int startCols[2] = { BoneMappingCol_SourceChainStart, BoneMappingCol_TargetChainStart };
   const int endCols[2] = { BoneMappingCol_SourceChainEnd, BoneMappingCol_TargetChainEnd };

   const int startColIdx = startCols[chainIdx];
   const int endColIdx = endCols[chainIdx];

   const uint mappingsCount = topLevelItemCount();

   // reset the statuses
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
}

///////////////////////////////////////////////////////////////////////////////

void MappingTable::clearBuildResults()
{
   m_sourceChainSkeleton->clear();
   m_targetChainSkeleton->clear();

   m_chainMappings.clear();

   uint count = m_sourceChains.size();
   for ( uint i = 0; i < count; ++i )
   {
      m_sourceChains[i]->removeReference();
   }
   m_sourceChains.clear();

   count = m_targetChains.size();
   for ( uint i = 0; i < count; ++i )
   {
      m_targetChains[i]->removeReference();
   }
   m_targetChains.clear();
}

///////////////////////////////////////////////////////////////////////////////

void MappingTable::buildMapping( const Skeleton* baseSourceSkeleton, const Skeleton* baseTargetSkeleton )
{
   clearBuildResults();

   buildChains( SourceBoneChain, baseSourceSkeleton, m_sourceChains );
   buildChains( TargetBoneChain, baseTargetSkeleton, m_targetChains );

   SkeletonMapperUtils::buildChainSkeleton( baseSourceSkeleton, m_sourceChains, m_sourceChainSkeleton );
   SkeletonMapperUtils::buildChainSkeleton( baseTargetSkeleton, m_targetChains, m_targetChainSkeleton );

   buildMappingTable();
}

///////////////////////////////////////////////////////////////////////////////

void MappingTable::buildChains( BoneChainIdx chainIdx, const Skeleton* baseSkeleton, Array< SkeletonBoneChain* >& outChainsArr )
{
   const int nameCols[2] = { BoneMappingCol_SourceChainName, BoneMappingCol_TargetChainName };
   const int startCols[2] = { BoneMappingCol_SourceChainStart, BoneMappingCol_TargetChainStart };
   const int endCols[2] = { BoneMappingCol_SourceChainEnd, BoneMappingCol_TargetChainEnd };

   const int nameColIdx = nameCols[chainIdx];
   const int startColIdx = startCols[chainIdx];
   const int endColIdx = endCols[chainIdx];

   const uint mappingsCount = topLevelItemCount();
   for ( uint i = 0; i < mappingsCount; ++i )
   {
      QTreeWidgetItem* item = topLevelItem( i );

      const std::string chainName = item->text( nameColIdx ).toStdString();
      const int firstBoneIdx = baseSkeleton->getBoneIndex( item->text( startColIdx ).toStdString().c_str() );
      const int lastBoneIdx = baseSkeleton->getBoneIndex( item->text( endColIdx ).toStdString().c_str() );

      SkeletonBoneChain* chain = new SkeletonBoneChain( baseSkeleton, chainName, firstBoneIdx, lastBoneIdx );
      outChainsArr.push_back( chain );
   }
}

///////////////////////////////////////////////////////////////////////////////

void MappingTable::buildMappingTable()
{
   const uint mappingsCount = topLevelItemCount();
   m_chainMappings.resize( mappingsCount, -1 );

   for ( uint i = 0; i < mappingsCount; ++i )
   {
      QTreeWidgetItem* item = topLevelItem( i );

      const std::string sourceChainName = item->text( BoneMappingCol_SourceChainName ).toStdString();
      const std::string targetChainName = item->text( BoneMappingCol_TargetChainName ).toStdString();

      const int sourceChainIdx = m_sourceChainSkeleton->getBoneIndex( sourceChainName.c_str() );
      const int targetChainIdx = m_targetChainSkeleton->getBoneIndex( targetChainName.c_str() );

      if ( sourceChainIdx < 0 || sourceChainIdx >= m_sourceChainSkeleton->getBoneCount() )
      {
         setChainStatus( i, BoneMappingCol_SourceChainName, false, "Chain not found in the chain skeleton" );
         continue;
      }

      if ( targetChainIdx < 0 || targetChainIdx >= m_targetChainSkeleton->getBoneCount() )
      {
         setChainStatus( i, BoneMappingCol_TargetChainName, false, "Chain not found in the chain skeleton" );
         continue;
      }

      if ( m_chainMappings[targetChainIdx] >= 0 )
      {
         const int mappedSourceChainIdx = m_chainMappings[targetChainIdx];

         char tmpStr[512];
         sprintf_s( tmpStr, "Target chain '%s' is already mapped to a source chain '%s'", targetChainName.c_str(), m_sourceChainSkeleton->m_boneNames[mappedSourceChainIdx].c_str() );
         setChainStatus( i, BoneMappingCol_SourceChainName, false, tmpStr );
         setChainStatus( i, BoneMappingCol_TargetChainName, false, tmpStr );
         continue;
      }

      // all is good - proceed with adding the new mapping
      m_chainMappings[targetChainIdx] = sourceChainIdx;
   }
}

///////////////////////////////////////////////////////////////////////////////
