/// @file   TamyEditor/BehaviorTreeEditor.h
/// @brief  a behavior tree resource editor
#pragma once

#include "ResourceEditor.h"


///////////////////////////////////////////////////////////////////////////////

class BehaviorTree;
class BehaviorTreeWidget;
class BehaviorTreeVariablesViewer;
class QPropertiesView;
class QTreeWidgetItem;
class QSplitter;

///////////////////////////////////////////////////////////////////////////////

class BehaviorTreeEditor : public ResourceEditor
{
   Q_OBJECT

private:
   BehaviorTree&                    m_behTree;

   BehaviorTreeWidget*              m_treeWidget;
   QPropertiesView*                 m_nodeProperties;
   BehaviorTreeVariablesViewer*     m_variablesViewer;
 
public:
   /**
    * Constructor.
    *
    * @param behTree
    */
   BehaviorTreeEditor( BehaviorTree& behTree );
   ~BehaviorTreeEditor();

public slots:
   void onSave();
   void editItem( QTreeWidgetItem* item, int column );

protected:
   // -------------------------------------------------------------------------
   // ResourceEditor implementation
   // -------------------------------------------------------------------------
   void onInitialize();
   void onDeinitialize( bool saveProgress );

private:
   void createRightHalfWindowSplitter( QSplitter* mainSplitter );
};

///////////////////////////////////////////////////////////////////////////////
