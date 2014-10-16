/// @file   TamyEditor/BehaviorTreeVariablesListDelegate.h
/// @brief  a delegate that renders behavior tree variables list contents
#pragma once

#include <QtWidgets\QStyledItemDelegate>


///////////////////////////////////////////////////////////////////////////////

class BehaviorTree;
class BehaviorTreeRunner;

///////////////////////////////////////////////////////////////////////////////

#define BTEL_VARIABLE_ROLE    33

///////////////////////////////////////////////////////////////////////////////

class BehaviorTreeVariablesListDelegate : public QStyledItemDelegate
{
   Q_OBJECT

private:
   BehaviorTree&              m_behaviorTree;
   BehaviorTreeRunner*        m_activePlayer;

public:
   /**
    * Constructor.
    *
    * @param parent
    * @param blendTree
    * @param activePlayer
    */
   BehaviorTreeVariablesListDelegate( QObject *parent, BehaviorTree& behaviorTree, BehaviorTreeRunner* activePlayer );
   ~BehaviorTreeVariablesListDelegate();

   // -------------------------------------------------------------------------
   // QStyledItemDelegate implementation
   // -------------------------------------------------------------------------
   QWidget* createEditor( QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index ) const;
   void setEditorData( QWidget* editor, const QModelIndex& index ) const;
   void setModelData( QWidget* editor, QAbstractItemModel* model, const QModelIndex& index ) const;
   void updateEditorGeometry( QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index ) const;
};

///////////////////////////////////////////////////////////////////////////////
