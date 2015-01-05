/// @file   TamyEditor/MappedSkeletonFrame.h
/// @brief  a custom widget that displays mapped skeletons
#pragma once

#include <QtWidgets\QFrame>
#include <QtWidgets\QTreeWidget>
#include "core\FilePath.h"


///////////////////////////////////////////////////////////////////////////////

class Skeleton;
class ResourceDropArea;

///////////////////////////////////////////////////////////////////////////////

class MappedSkeletonTree : public QTreeWidget
{
   Q_OBJECT

public:
   const Skeleton*            m_skeleton;

private:
   bool                       m_isSource;

public:
   MappedSkeletonTree( QWidget* parent, bool isSource );
   ~MappedSkeletonTree();

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

   // -------------------------------------------------------------------------
   // QAbstractItemView implementation
   // -------------------------------------------------------------------------
   QMimeData* mimeData( const QList<QTreeWidgetItem *> items ) const;
   void dragMoveEvent( QDragMoveEvent* event );
   void dropEvent( QDropEvent* event );
};

///////////////////////////////////////////////////////////////////////////////

class MappedSkeletonFrame : public QFrame
{
   Q_OBJECT

public:
   ResourceDropArea*       m_skeletonPath;

private:
   MappedSkeletonTree*     m_skeletonTree;

public:
   /**
    * Constructor.
    *
    * @param parent
    * @param isSource
    */
   MappedSkeletonFrame( QWidget* parent, bool isSource );

   /**
    * Sets a new skeleton to be displayed in the tree.
    *
    * @param skeleton
    */
   void setSkeleton( const Skeleton* skeleton );

   /**
    * Returns the skeleton instance displayed in this frame.
    */
   inline const Skeleton* getSkeleton() const {
      return m_skeletonTree->m_skeleton;
   }

signals:
   void skeletonChanged();

public slots:
   void skeletonPathChanged( const FilePath& path );
};

///////////////////////////////////////////////////////////////////////////////
