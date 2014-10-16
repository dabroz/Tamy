/// @file   TamyEditor/BlendTreeVariablesListDelegate.h
/// @brief  a delegate that renders blend tree variables list contents
#pragma once

#include <QtWidgets\QStyledItemDelegate>


///////////////////////////////////////////////////////////////////////////////

class BlendTree;
class BlendTreePlayer;

///////////////////////////////////////////////////////////////////////////////

#define BTEL_VARIABLE_ROLE    33

///////////////////////////////////////////////////////////////////////////////

class BlendTreeVariablesListDelegate : public QStyledItemDelegate
{
   Q_OBJECT

private:
   BlendTree&                 m_blendTree;
   BlendTreePlayer*           m_activePlayer;

public:
   /**
    * Constructor.
    *
    * @param parent
    * @param blendTree
    * @param activePlayer
    */
   BlendTreeVariablesListDelegate( QObject *parent, BlendTree& blendTree, BlendTreePlayer* activePlayer );
   ~BlendTreeVariablesListDelegate();

   // -------------------------------------------------------------------------
   // QStyledItemDelegate implementation
   // -------------------------------------------------------------------------
   QWidget* createEditor( QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index ) const;
   void setEditorData( QWidget* editor, const QModelIndex& index ) const;
   void setModelData( QWidget* editor, QAbstractItemModel* model, const QModelIndex& index ) const;
   void updateEditorGeometry( QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index ) const;
};

///////////////////////////////////////////////////////////////////////////////
