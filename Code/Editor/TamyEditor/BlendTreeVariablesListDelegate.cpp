#include "BlendTreeVariablesListDelegate.h"
#include "BlendTreeVariableEditor.h"
#include "core-AI\BlendTreeVariable.h"
#include <QtWidgets\QLineEdit>


///////////////////////////////////////////////////////////////////////////////

BlendTreeVariablesListDelegate::BlendTreeVariablesListDelegate( QObject *parent, BlendTree& blendTree, BlendTreePlayer* activePlayer )
   : QStyledItemDelegate( parent )
   , m_blendTree( blendTree )
   , m_activePlayer( activePlayer )
{
}

///////////////////////////////////////////////////////////////////////////////

BlendTreeVariablesListDelegate::~BlendTreeVariablesListDelegate()
{
}

///////////////////////////////////////////////////////////////////////////////

QWidget* BlendTreeVariablesListDelegate::createEditor( QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index ) const
{
   if ( index.column() == 1 )
   {
      BlendTreeVariableEditor* variableEditor = new BlendTreeVariableEditor( parent, m_blendTree, m_activePlayer );
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

void BlendTreeVariablesListDelegate::setEditorData( QWidget* editor, const QModelIndex& index ) const
{
   if ( index.column() == 1 )
   {
      BlendTreeVariable* btVariable = (BlendTreeVariable*)( index.model()->data( index, BTEL_VARIABLE_ROLE ).value< void* >() );

      BlendTreeVariableEditor* variableEditor = static_cast< BlendTreeVariableEditor* >( editor );
      variableEditor->initialize( btVariable );
   }
   else
   {
      QLineEdit* variableEditor = static_cast< QLineEdit* >( editor );
      variableEditor->setText( index.model()->data( index ).toString() );
   }
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeVariablesListDelegate::setModelData( QWidget* editor, QAbstractItemModel* model, const QModelIndex& index ) const
{
   if ( index.column() == 1 )
   {
      BlendTreeVariable* btVariable = (BlendTreeVariable*)( index.model()->data( index, BTEL_VARIABLE_ROLE ).value< void* >() );

      char valueStr[32];
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
      
      BlendTreeVariable* btVariable = (BlendTreeVariable*)( index.model()->data( index, BTEL_VARIABLE_ROLE ).value< void* >() );
      QString variableNewName = variableEditor->text();
      btVariable->setName( variableNewName.toStdString().c_str() );

      model->setData( index, variableNewName );
   }
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeVariablesListDelegate::updateEditorGeometry( QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index ) const
{
   editor->setGeometry( option.rect );
}

///////////////////////////////////////////////////////////////////////////////
