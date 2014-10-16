/// @file   TamyEditor/BlendTreeEventsListDelegate.h
/// @brief  a delegate that renders blend tree events list contents
#pragma once

#include <QtWidgets\QStyledItemDelegate>


///////////////////////////////////////////////////////////////////////////////

class BlendTree;
class BlendTreePlayer;

///////////////////////////////////////////////////////////////////////////////

#define BTEL_EVENT_ROLE    33

///////////////////////////////////////////////////////////////////////////////

class BTLEventsDelegate_Playing : public QStyledItemDelegate
{
   Q_OBJECT

private:
   BlendTreePlayer*           m_blendTreePlayer;

public:
   /**
    * Constructor.
    *
    * @param parent
    * @param blendTreePlayer
    */
   BTLEventsDelegate_Playing( QObject *parent, BlendTreePlayer* blendTreePlayer );

   // -------------------------------------------------------------------------
   // QStyledItemDelegate implementation
   // -------------------------------------------------------------------------
   QWidget* createEditor( QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index ) const;
   void setEditorData( QWidget* editor, const QModelIndex& index ) const;
   void setModelData( QWidget* editor, QAbstractItemModel* model, const QModelIndex& index ) const;
   void updateEditorGeometry( QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index ) const;
};

///////////////////////////////////////////////////////////////////////////////

class BTLEventsDelegate_Edition : public QStyledItemDelegate
{
   Q_OBJECT

private:
   BlendTree&                 m_blendTree;

public:
   /**
    * Constructor.
    *
    * @param parent
    * @param blendTree
    */
   BTLEventsDelegate_Edition( QObject *parent, BlendTree& blendTree );

   // -------------------------------------------------------------------------
   // QStyledItemDelegate implementation
   // -------------------------------------------------------------------------
   QWidget* createEditor( QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index ) const;
   void setEditorData( QWidget* editor, const QModelIndex& index ) const;
   void setModelData( QWidget* editor, QAbstractItemModel* model, const QModelIndex& index ) const;
   void updateEditorGeometry( QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index ) const;
};

///////////////////////////////////////////////////////////////////////////////
