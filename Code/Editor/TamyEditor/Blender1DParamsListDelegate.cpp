#include "Blender1DParamsListDelegate.h"
#include "core-AI\BlendTreeBlender1D.h"
#include "Blender1DParamEditor.h"


///////////////////////////////////////////////////////////////////////////////

Blender1DParamsListDelegate::Blender1DParamsListDelegate( QObject *parent, BlendTreeBlender1D& blendNode )
   : QStyledItemDelegate( parent )
   , m_blendNode( blendNode )
{
}

///////////////////////////////////////////////////////////////////////////////

Blender1DParamsListDelegate::~Blender1DParamsListDelegate()
{
}

///////////////////////////////////////////////////////////////////////////////

QWidget* Blender1DParamsListDelegate::createEditor( QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index ) const
{
   if ( index.column() == 1 )
   {
      Blender1DParamEditor* paramEditor = new Blender1DParamEditor( parent );
      return paramEditor;
   }
   else
   {
      return NULL;
   }
}

///////////////////////////////////////////////////////////////////////////////

void Blender1DParamsListDelegate::setEditorData( QWidget* editor, const QModelIndex& index ) const
{
   if ( index.column() == 1 )
   {
      int paramIdx = index.model()->data( index, BTB1D_PARAMETER_ROLE ).value< int >();
      float paramValue = m_blendNode.getParameterValue( paramIdx );

      Blender1DParamEditor* paramEditor = static_cast< Blender1DParamEditor* >( editor );
      paramEditor->initialize( paramValue );
   }
}

///////////////////////////////////////////////////////////////////////////////

void Blender1DParamsListDelegate::setModelData( QWidget* editor, QAbstractItemModel* model, const QModelIndex& index ) const
{
   if ( index.column() == 1 )
   {
      Blender1DParamEditor* paramEditor = static_cast< Blender1DParamEditor* >( editor );

      int paramIdx = index.model()->data( index, BTB1D_PARAMETER_ROLE ).value< int >();
      float newParamValue = paramEditor->getValue();
      m_blendNode.assignParameterValue( paramIdx, newParamValue );

      model->setData( index, QString( "%0" ).arg( newParamValue ) );
   }
}

///////////////////////////////////////////////////////////////////////////////

void Blender1DParamsListDelegate::updateEditorGeometry( QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index ) const
{
   editor->setGeometry( option.rect );
}

///////////////////////////////////////////////////////////////////////////////
