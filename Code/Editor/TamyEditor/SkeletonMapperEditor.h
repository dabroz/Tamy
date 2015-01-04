/// @file   TamyEditor/SkeletonMapperEditor.h
/// @brief  Editor allowing to edit material resources
#pragma once

#include "ResourceEditor.h"
#include "core\Array.h"
#include "core\FilePath.h"


///////////////////////////////////////////////////////////////////////////////

class SkeletonMapper;
class Skeleton;
class QTreeWidget;
class QTreeWidgetItem;
class QLayout;
class QBoxLayout;
class QSplitter;
class QAction;
class ResourceDropArea;

///////////////////////////////////////////////////////////////////////////////

class SkeletonMapperEditor : public ResourceEditor
{
   Q_OBJECT

private:
   SkeletonMapper&         m_skeletonMapper;

   ResourceDropArea*       m_sourceSkeletonPath;
   ResourceDropArea*       m_targetSkeletonPath;

   QTreeWidget*            m_sourceTree;
   QTreeWidget*            m_targetTree;

   QTreeWidget*            m_mappingTable;

   QTreeWidget*            m_sourceChainsTree;
   QTreeWidget*            m_targetChainsTree;

   QAction*                m_actionSave;
   QAction*                m_buildMapping;

public:
   /**
    * Constructor.
    *
    * @param skeletonMapper
    */
   SkeletonMapperEditor( SkeletonMapper& mapper );
   ~SkeletonMapperEditor();

   // -------------------------------------------------------------------------
   // ResourceEditor implementation
   // -------------------------------------------------------------------------
   void onInitialize();
   void onDeinitialize( bool saveProgress );

public slots:
   void saveResource();
   void buildMapping();
   void sourceSkeletonChanged( const FilePath& path );
   void targetSkeletonChanged( const FilePath& path );

private:
   void createToolbar( QLayout* layout );
   void createMappedSkeletonsFrame( QSplitter* splitter );
   void createMappingTable( QSplitter* splitter );
   void createSkeletonChainsFrame( QSplitter* splitter );
   void createSkeletonResourceFrame( QLayout* parentLayout, QTreeWidget*& outTreeWidget, ResourceDropArea*& outSkelketonPathWidget );

   void initSkeletonTree( QTreeWidget* treeWidget, const Skeleton* skeleton ) const;
   QTreeWidgetItem* findBoneItem( QTreeWidget* treeWidget, const QString& boneName ) const;

   void updateSkeletonViews();
   void updateMappingTable();
   void updateChainsView();
};

///////////////////////////////////////////////////////////////////////////////
