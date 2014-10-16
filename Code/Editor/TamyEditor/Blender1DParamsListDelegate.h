/// @file   TamyEditor/Blender1DParamsListDelegate.h
/// @brief  a custom editor  for the list of parameters of a 1D Blender
#pragma once

#include <QtWidgets\QStyledItemDelegate>

///////////////////////////////////////////////////////////////////////////////

class BlendTreeBlender1D;

///////////////////////////////////////////////////////////////////////////////

#define BTB1D_PARAMETER_ROLE    33

///////////////////////////////////////////////////////////////////////////////

class Blender1DParamsListDelegate : public QStyledItemDelegate
{
   Q_OBJECT

private:
   BlendTreeBlender1D&        m_blendNode;

public:
   /**
    * Constructor.
    *
    * @param parent
    * @param blendNode
    */
   Blender1DParamsListDelegate( QObject *parent, BlendTreeBlender1D& blendNode );
   ~Blender1DParamsListDelegate();

   // -------------------------------------------------------------------------
   // QStyledItemDelegate implementation
   // -------------------------------------------------------------------------
   QWidget* createEditor( QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index ) const;
   void setEditorData( QWidget* editor, const QModelIndex& index ) const;
   void setModelData( QWidget* editor, QAbstractItemModel* model, const QModelIndex& index ) const;
   void updateEditorGeometry( QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index ) const;
};

///////////////////////////////////////////////////////////////////////////////
