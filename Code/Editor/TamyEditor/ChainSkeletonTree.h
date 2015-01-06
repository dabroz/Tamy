/// @file   TamyEditor/ChainSkeletonTree.h
/// @brief  a tree widget used to preview the compiled bone chain skeletons
#pragma once

#include <QtWidgets\QTreeWidget>


///////////////////////////////////////////////////////////////////////////////

class Skeleton;

///////////////////////////////////////////////////////////////////////////////

class ChainSkeletonTree : public QTreeWidget
{
   Q_OBJECT

public:
   /**
    * Constructor.
    *
    * @param parent
    */
   ChainSkeletonTree( QWidget* parent );

   /**
    * Sets a new skeleton to be displayed in the tree.
    *
    * @param skeleton
    */
   void setSkeleton( const Skeleton* skeleton );

   /**
    * Locates and item that corresponds to the specified bone.
    */
   QTreeWidgetItem* findBoneItem( const QString& boneName ) const;
};

///////////////////////////////////////////////////////////////////////////////
