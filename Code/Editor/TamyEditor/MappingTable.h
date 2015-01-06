/// @file   TamyEditor/MappingTable.h
/// @brief  a table view used by the SkeletonMapperEditor to display a list of defined mappings
#pragma once

#include <QtWidgets\QTreeWidget>
#include "core\Array.h"


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
class SkeletonMapper;
class SkeletonBoneChain;

///////////////////////////////////////////////////////////////////////////////

class MappingTable : public QTreeWidget
{
   Q_OBJECT

private:
   // mapper build results
   Array< SkeletonBoneChain* >      m_sourceChains;
   Array< SkeletonBoneChain* >      m_targetChains;
   Array< int >                     m_chainMappings;
   Skeleton*                        m_sourceChainSkeleton;
   Skeleton*                        m_targetChainSkeleton;

   // error status
   bool                             m_errorFlag;

public:
   MappingTable( QWidget* parent );
   ~MappingTable();

   /**
    * Clears the table and sets the mapping from the specified mapper.
    *
    * @param mapper
    */
   void setMapping( const SkeletonMapper& mapper );

   /**
    * Verifies the specified chain.
    *
    * @param chainIdx
    * @param skeleton
    */
   void verifyChain( BoneChainIdx chainIdx, const Skeleton* baseSkeleton );

   /**
    * Builds the mapping.
    *
    * @param baseSourceSkeleton
    * @param baseTargetSkeleton
    */
   void buildMapping( const Skeleton* baseSourceSkeleton, const Skeleton* baseTargetSkeleton );

   /**
    * Clears the build results.
    */
   void clearBuildResults();

   /**
    * Resets the error flag.
    */
   inline void resetErrorFlag() {
      m_errorFlag = false;
   }

   /**
    * Returns the value of the error flag.
    * If an error was encountered, the flag will be set to 'true'.
    */
   inline bool getErrorFlag() const {
      return m_errorFlag;
   }

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

   void updateChainNames();

   void buildChains( BoneChainIdx chainIdx, const Skeleton* baseSkeleton, Array< SkeletonBoneChain* >& outChainsArr );
   void buildMappingTable();
};

///////////////////////////////////////////////////////////////////////////////
