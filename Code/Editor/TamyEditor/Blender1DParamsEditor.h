/// @file   TamyEditor/Blender1DParamsEditor.h
/// @brief  blend params editor for a BlendTreeBlender1D
#pragma once

#include "BTBFEditor.h"
#include "core-AI\BlendTreeListener.h"


///////////////////////////////////////////////////////////////////////////////

class QPropertiesView;
class BlendTreeBlender1D;
class QTreeView;
class QStandardItemModel;
class QAbstractItemDelegate;
class Blender1DParamsListDelegate;

///////////////////////////////////////////////////////////////////////////////

class Blender1DParamsEditor : public BTBFEditor, public BlendTreeListener
{
   Q_OBJECT

private:
   BlendTreeBlender1D&              m_blenderNode;

   QPropertiesView*                 m_propertiesView;

   QTreeView*                       m_paramsListView;
   QStandardItemModel*              m_dataModel;

   Blender1DParamsListDelegate*     m_editTimeDelegate;
   QAbstractItemDelegate*           m_playTimeItemDelegate;

public:
   /**
    * Constructor.
    *
    * @param blenderNode
    */
   Blender1DParamsEditor( BlendTreeBlender1D& blenderNode );
   ~Blender1DParamsEditor();

   // -------------------------------------------------------------------------
   // BTBFEditor implementation
   // -------------------------------------------------------------------------
   void initialize( BlendTreeEditor& mainEditor );
   void deinitialize( BlendTreeEditor& mainEditor );
   BlendTreeNode* getNode() const;
   void setPlayingMode( bool isPlaying );

   // -------------------------------------------------------------------------
   // BlendTreeListener implementation
   // -------------------------------------------------------------------------
   void onNodeAdded( BlendTreeNode* parentNode, int insertionIdx, BlendTreeNode* node );
   void onNodeRemoved( BlendTreeNode* parentNode, BlendTreeNode* node );
   void onNodeChanged( BlendTreeNode* node );
};

///////////////////////////////////////////////////////////////////////////////
