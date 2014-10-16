/// @file   TamyEditor/BehaviorTreeVariablesViewer.h
/// @brief  a list and an editor of the behavior tree variables
#pragma once

#include <QtWidgets\QFrame>
#include <QtWidgets\QAction>
#include "core-AI\BehaviorTreeListener.h"


///////////////////////////////////////////////////////////////////////////////

class BehaviorTree;
class BehaviorTreeRunner;
class QTreeView;
class QStandardItemModel;
class QLineEdit;
class TreeWidgetDescFactory;
class BehaviorTreeVariablesListDelegate;

///////////////////////////////////////////////////////////////////////////////

/**
 * A list and an editor of the blend tree variables.
 */
class BehaviorTreeVariablesViewer : public QFrame, public BehaviorTreeListener
{
   Q_OBJECT

private:
   BehaviorTree&                          m_behaviorTree;
   BehaviorTreeRunner*                    m_behaviorTreePlayer;

   QTreeView*                             m_variablesListView;
   QStandardItemModel*                    m_dataModel;

   TreeWidgetDescFactory*                 m_variablesFactory;

   BehaviorTreeVariablesListDelegate*     m_editTimeDelegate;
   BehaviorTreeVariablesListDelegate*     m_playTimeDelegate;

public:
   /**
    * Constructor.
    *
    * @param parentWidget
    * @param behaviorTree
    * @param behaviorTreePlayer
    */
   BehaviorTreeVariablesViewer( QWidget* parentWidget, BehaviorTree& behaviorTree, BehaviorTreeRunner* behaviorTreePlayer );
   ~BehaviorTreeVariablesViewer();

   /**
    * Switches the viewer to the 'playing' mode, in which instead of editing, one
    * can trigger events.
    *
    * @param isPlaying
    */
   void setPlayingMode( bool isPlaying );

   // -------------------------------------------------------------------------
   // BehaviorTreeListener implementation
   // -------------------------------------------------------------------------
   void onVariableAdded( BehaviorTreeVariable* variable );
   void onVariableRemoved( BehaviorTreeVariable* variable );

public slots:
   void onShowContextMenu( const QPoint& pos );

private:
   void initUI();
};

///////////////////////////////////////////////////////////////////////////////

class AddBehaviorTreeVariableAction : public QAction
{
   Q_OBJECT

private:
   BehaviorTree&                       m_behaviorTree;
   uint                                m_typeIdx;
   TreeWidgetDescFactory*              m_variablesFactory;

public:
   /**
    * Constructor.
    *
    * @param BehaviorTree
    * @param type
    */
   AddBehaviorTreeVariableAction( const QIcon& icon, const QString& desc, unsigned int typeIdx, BehaviorTree& behaviorTree, TreeWidgetDescFactory* variablesFactory );

public slots:
   void onTriggered();
};

///////////////////////////////////////////////////////////////////////////////
