#include "BehaviorTreeVariablesListDelegate.h"
#include "BehaviorTreeVariableEditor.h"
#include "core-AI\BehTreeVariable.h"
#include <QtWidgets\QLineEdit>


///////////////////////////////////////////////////////////////////////////////

BehaviorTreeVariablesListDelegate::BehaviorTreeVariablesListDelegate( QObject *parent, BehaviorTree& behaviorTree, BehaviorTreeRunner* activePlayer )
   : QStyledItemDelegate( parent )
   , m_behaviorTree( behaviorTree )
   , m_activePlayer( activePlayer )
{
}

///////////////////////////////////////////////////////////////////////////////

BehaviorTreeVariablesListDelegate::~BehaviorTreeVariablesListDelegate()
{
}

///////////////////////////////////////////////////////////////////////////////

QWidget* BehaviorTreeVariablesListDelegate::createEditor( QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index ) const
{
   if ( index.column() == 1 )
   {
      BehaviorTreeVariableEditor* variableEditor = new BehaviorTreeVariableEditor( parent, m_behaviorTree, m_activePlayer );
      return variableEditor;
   }
   else
   {
      if ( m_activePlayer )
      {
         // we don't allow the user to change variable's name at runtime
         return NULL;
      }
      else
      {
         QLineEdit* variableEditor = new QLineEdit( parent );
         return variableEditor;
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void BehaviorTreeVariablesListDelegate::setEditorData( QWidget* editor, const QModelIndex& index ) const
{
   if ( index.column() == 1 )
   {
      BehaviorTreeVariable* btVariable = (BehaviorTreeVariable*)( index.model()->data( index, BTEL_VARIABLE_ROLE ).value< void* >() );

      BehaviorTreeVariableEditor* variableEditor = static_cast< BehaviorTreeVariableEditor* >( editor );
      variableEditor->initialize( btVariable );
   }
   else
   {
      QLineEdit* variableEditor = static_cast< QLineEdit* >( editor );
      variableEditor->setText( index.model()->data( index ).toString() );
   }
}

///////////////////////////////////////////////////////////////////////////////

void BehaviorTreeVariablesListDelegate::setModelData( QWidget* editor, QAbstractItemModel* model, const QModelIndex& index ) const
{
   if ( index.column() == 1 )
   {
      BehaviorTreeVariable* btVariable = (BehaviorTreeVariable*)( index.model()->data( index, BTEL_VARIABLE_ROLE ).value< void* >() );

      char valueStr[64];
      if ( m_activePlayer )
      {
         btVariable->runtimeValueToString( m_activePlayer, valueStr );
      }
      else
      {
         btVariable->valueToString( valueStr );
      }
      model->setData( index, QString( valueStr ) );
   }
   else
   {
      QLineEdit* variableEditor = static_cast< QLineEdit* >( editor );

      BehaviorTreeVariable* btVariable = (BehaviorTreeVariable*)( index.model()->data( index, BTEL_VARIABLE_ROLE ).value< void* >() );
      QString variableNewName = variableEditor->text();
      btVariable->setName( variableNewName.toStdString().c_str() );

      model->setData( index, variableNewName );
   }
}

///////////////////////////////////////////////////////////////////////////////

void BehaviorTreeVariablesListDelegate::updateEditorGeometry( QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index ) const
{
   editor->setGeometry( option.rect );
}

///////////////////////////////////////////////////////////////////////////////
