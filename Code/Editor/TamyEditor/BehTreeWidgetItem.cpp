#include "BehTreeWidgetItem.h"
#include "core-AI\BehTreeNode.h"


///////////////////////////////////////////////////////////////////////////////

BehTreeWidgetItem::~BehTreeWidgetItem()
{
}

///////////////////////////////////////////////////////////////////////////////

void BehTreeWidgetItem::updateDescription()
{
   BehTreeNode* node = getNode();
   setText( 0, node->getName().c_str() );
}

///////////////////////////////////////////////////////////////////////////////
