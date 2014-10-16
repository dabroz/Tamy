/// @file   ext-StoryTeller/StoryListener.h
/// @brief  listener that gets informed about changes done to story objects
#pragma once

#include "core\types.h"


///////////////////////////////////////////////////////////////////////////////

class StoryNode;

///////////////////////////////////////////////////////////////////////////////

/**
 * Listener that gets informed about changes done to story objects
 */
class StoryListener
{
public:
   virtual ~StoryListener() {}

   /**
    * Called when a new node was added to the tree.
    *
    * @param parentNode
    * @param insertionIdx
    * @param node
    */
   virtual void onNodeAdded( StoryNode* parentNode, int insertionIdx, StoryNode* node ) = 0;

   /**
    * Called when a node is about to be removed from the tree.
    *
    * @param parentNode
    * @param node
    */
   virtual void onNodeRemoved( StoryNode* parentNode, StoryNode* node ) = 0;

   /**
    * Called when something about a node changes.
    *
    * @param node
    */
   virtual void onNodeChanged( StoryNode* node ) = 0;
};

///////////////////////////////////////////////////////////////////////////////
