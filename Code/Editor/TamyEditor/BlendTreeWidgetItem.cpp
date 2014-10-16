#include "BlendTreeWidgetItem.h"
#include "core-AI\BlendTreeNode.h"
#include "core-AI\BlendTreePlayer.h"
#include "MenuUtils.h"
#include <QtGui\QIcon>


///////////////////////////////////////////////////////////////////////////////

BlendTreeWidgetItem::~BlendTreeWidgetItem()
{
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeWidgetItem::updateDescription()
{
   BlendTreeNode* node = getNode();
   setText( 0, node->getName().c_str() );
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeWidgetItem::updateRuntimeState( BlendTreePlayer* player )
{
   BlendTreeNode* node = getNode();   
   BlendTreeNode::State nodeState = node->getState( player );

   const SerializableReflectionType& nodeType = node->getVirtualRTTI();
   QString iconName = MenuUtils::getIconForType( nodeType, nodeState == BlendTreeNode::Inactive ? Icon_Regular : Icon_Active );
   setIcon( 0, QIcon( iconName ) );
}

///////////////////////////////////////////////////////////////////////////////
