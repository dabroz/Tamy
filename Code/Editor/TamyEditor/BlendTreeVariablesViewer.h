/// @file   TamyEditor/BlendTreeVariablesViewer.h
/// @brief  a list and an editor of the blend tree variables
#pragma once

#include <QtWidgets\QFrame>
#include <QtWidgets\QAction>
#include "core-AI\BlendTreeListener.h"


///////////////////////////////////////////////////////////////////////////////

class BlendTree;
class BlendTreePlayer;
class QTreeView;
class QStandardItemModel;
class QLineEdit;
class TreeWidgetDescFactory;
class BlendTreeVariablesListDelegate;

///////////////////////////////////////////////////////////////////////////////

/**
 * A list and an editor of the blend tree variables.
 */
class BlendTreeVariablesViewer : public QFrame, public BlendTreeListener
{
   Q_OBJECT

private:
   BlendTree&                             m_blendTree;
   BlendTreePlayer*                       m_blendTreePlayer;

   QTreeView*                             m_variablesListView;
   QStandardItemModel*                    m_dataModel;

   TreeWidgetDescFactory*                 m_variablesFactory;

   BlendTreeVariablesListDelegate*        m_editTimeDelegate;
   BlendTreeVariablesListDelegate*        m_playTimeDelegate;

public:
   /**
    * Constructor.
    *
    * @param parentWidget
    * @param blendTree
    * @param blendTreeWidget
    */
   BlendTreeVariablesViewer( QWidget* parentWidget, BlendTree& blendTree, BlendTreePlayer* blendTreePlayer );
   ~BlendTreeVariablesViewer();


   /**
    * Switches the viewer to the 'playing' mode, in which instead of editing, one
    * can trigger events.
    *
    * @param isPlaying
    */
   void setPlayingMode( bool isPlaying );

   // -------------------------------------------------------------------------
   // BlendTreeListener implementation
   // -------------------------------------------------------------------------
   void onVariableAdded( BlendTreeVariable* variable );
   void onVariableRemoved( BlendTreeVariable* variable );

public slots:
   void onShowContextMenu( const QPoint& pos );

private:
   void initUI();
};

///////////////////////////////////////////////////////////////////////////////

class AddBlendTreeVariableAction : public QAction
{
   Q_OBJECT

private:
   BlendTree&                          m_blendTree;
   uint                                m_typeIdx;
   TreeWidgetDescFactory*              m_variablesFactory;

public:
   /**
    * Constructor.
    *
    * @param blendTree
    * @param type
    */
   AddBlendTreeVariableAction( const QIcon& icon, const QString& desc, unsigned int typeIdx, BlendTree& blendTree, TreeWidgetDescFactory* variablesFactory );

public slots:
   void onTriggered();
};

///////////////////////////////////////////////////////////////////////////////
