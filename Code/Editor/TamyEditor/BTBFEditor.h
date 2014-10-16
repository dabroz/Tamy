/// @file   TamyEditor/BTBFEditor.h
/// @brier  a base class for editors that can be placed in BlendTreeEditor's bottom frame
#pragma once

#include <QtWidgets\QFrame>


///////////////////////////////////////////////////////////////////////////////

class BlendTreeNode;
class BlendTreeEditor;

///////////////////////////////////////////////////////////////////////////////

class BTBFEditor : public QFrame
{
   Q_OBJECT

protected:
   BlendTreeNode&    m_editedNode;

public:
   /**
    * Constructor.
    *
    * @param editedNode
    */
   BTBFEditor( BlendTreeNode& editedNode );
   virtual ~BTBFEditor();

   /**
    * Initializes the widget.
    *
    * @param mainEditor
    */
   virtual void initialize( BlendTreeEditor& mainEditor ) {}

   /**
    * Deinitializes the widget.
    *
    * @param mainEditor
    */
   virtual void deinitialize( BlendTreeEditor& mainEditor ) {}

   /**
    * Returns a pointer to the edited BlendTreeNode.
    */
   inline BlendTreeNode* getNode() const { return &m_editedNode; }

   /**
    * Informs the widget when the scene becomes simulated or editable.
    *
    * @param playing
    */
   virtual void setPlayingMode( bool playing ) {}
};

///////////////////////////////////////////////////////////////////////////////
