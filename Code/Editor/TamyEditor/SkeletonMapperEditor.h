/// @file   TamyEditor/SkeletonMapperEditor.h
/// @brief  Editor allowing to edit material resources
#pragma once

#include "ResourceEditor.h"
#include "core\Array.h"
#include "core\FilePath.h"


///////////////////////////////////////////////////////////////////////////////

class SkeletonMapper;
class Skeleton;
class QTreeWidgetItem;
class QLayout;
class QBoxLayout;
class QSplitter;
class QAction;
class QTreeWidget;
class QToolBar;
class MappingTable;
class MappedSkeletonFrame;

///////////////////////////////////////////////////////////////////////////////

class SkeletonMapperEditor : public ResourceEditor
{
   Q_OBJECT

private:
   SkeletonMapper&                  m_skeletonMapper;

   MappedSkeletonFrame*             m_sourceTree;
   MappedSkeletonFrame*             m_targetTree;

   MappingTable*                    m_mappingTable;

   QTreeWidget*                     m_sourceChainsTree;
   QTreeWidget*                     m_targetChainsTree;

   QAction*                         m_actionSave;
   QAction*                         m_buildMapping;
   QAction*                         m_addMapping;
   QAction*                         m_removeMapping;

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
   void mappingSelectionChanged();
   void onSkeletonChanged();

private:
   void defineToolbarActions( QToolBar* toolbar );
   void createMappedSkeletonsFrame( QSplitter* splitter );
   void createMappingTable( QSplitter* splitter );
   void createSkeletonChainsFrame( QSplitter* splitter );

   void syncResourceToEditor();
   void syncEditorToResource();
};

///////////////////////////////////////////////////////////////////////////////
