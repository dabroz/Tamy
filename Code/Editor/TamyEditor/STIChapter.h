/// @file   TamyEditor/STIChapter.h
/// @brief  story tree item that represents a StoryChapter
#pragma once

#include "StoryBrowserTreeItem.h"
#include "core\List.h"


///////////////////////////////////////////////////////////////////////////////

class StoryChapter;
class Entity;

///////////////////////////////////////////////////////////////////////////////

/**
 * Story tree item that represents a StoryChapter.
 */
class STIChapter : public StoryBrowserTreeItem
{
private:
   StoryChapter&        m_chapter;
  

public:
   /**
    * Constructor.
    *
    * @param action
    */
   STIChapter( StoryChapter& chapter );
   ~STIChapter();

   // -------------------------------------------------------------------------
   // StoryBrowserTreeItem implementation
   // -------------------------------------------------------------------------
   StoryNode* getNode();
   void addNode( StoryNode* newNode );
   void insertNode( int insertionIdx, StoryNode* nodeToInsert );
   void removeNode( StoryBrowserTreeItem* removedChildItem );
   void clear();
};

///////////////////////////////////////////////////////////////////////////////
