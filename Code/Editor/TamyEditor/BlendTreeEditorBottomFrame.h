/// @file   TamyEditor\BlendTreeEditorBottomFrame.h
/// @brief  frame located below the preview frame - you can put a widget in it
#pragma once

#include <QtWidgets\QTabWidget>
#include "core-AI\BlendTreeListener.h"
#include "core\GenericFactory.h"


///////////////////////////////////////////////////////////////////////////////

class BlendTree;
class BlendTreeEditor;
class BlendTreeNode;
class BTBFEditor;

///////////////////////////////////////////////////////////////////////////////

class BlendTreeEditorBottomFrame : public QTabWidget, public BlendTreeListener
{
   Q_OBJECT

private:
   BlendTree&                                      m_blendTree;
   BlendTreeEditor&                                m_mainEditor;
   GenericFactory< BlendTreeNode, BTBFEditor >*    m_editorsFactory;

public:
   /**
    * Constructor.
    *
    * @param parent
    * @param blendTree
    * @param mainEditor
    */
   BlendTreeEditorBottomFrame( QWidget* parent, BlendTree& blendTree, BlendTreeEditor& mainEditor );
   ~BlendTreeEditorBottomFrame();

   /**
    * Creates an editor for the specified node
    *
    * @param node
    */
   void createEditor( BlendTreeNode* node );

   /**
    * Informs the widget when the scene becomes simulated or editable.
    *
    * @param playing
    */
   void setPlayingMode( bool playing );

   // -------------------------------------------------------------------------
   // BlendTreeListener implementation
   // -------------------------------------------------------------------------
   void onNodeRemoved( BlendTreeNode* parentNode, BlendTreeNode* node );

public slots:
   void onCloseTab( int tabIdx );
};

///////////////////////////////////////////////////////////////////////////////
