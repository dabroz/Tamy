/// @file   TamyEditor/MappingTable.h
/// @brief  a table view used by the SkeletonMapperEditor to display a list of defined mappings
#pragma once

#include <QtWidgets\QTreeWidget>


///////////////////////////////////////////////////////////////////////////////

enum BoneMappingTableColums
{
   BoneMappingCol_SourceChainName,
   BoneMappingCol_SourceChainStart,
   BoneMappingCol_SourceChainEnd,
   BoneMappingCol_TargetChainName,
   BoneMappingCol_TargetChainStart,
   BoneMappingCol_TargetChainEnd,
};

///////////////////////////////////////////////////////////////////////////////

enum BoneChainIdx
{
   SourceBoneChain,
   TargetBoneChain
};

///////////////////////////////////////////////////////////////////////////////

class Skeleton;

///////////////////////////////////////////////////////////////////////////////

class MappingTable : public QTreeWidget
{
   Q_OBJECT

public:
   MappingTable( QWidget* parent );

   /**
    * Verifies the specified chain.
    *
    * @param chainIdx
    * @param skeleton
    */
   bool verifyChain( BoneChainIdx chainIdx, const Skeleton* skeleton );

   // -------------------------------------------------------------------------
   // QAbstractItemView implementation
   // -------------------------------------------------------------------------
   void dragMoveEvent( QDragMoveEvent* event );
   void dropEvent( QDropEvent* event );

public slots:
   void addMapping();
   void removeMapping();

private:
   void resetChainStatus( uint chainIdx, int columnIdx );
   void setChainStatus( uint chainIdx, int columnIdx, bool newStatus, const char* errorMsg );

};

///////////////////////////////////////////////////////////////////////////////
