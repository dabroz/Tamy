/// @file   TamyEditor/StoryBrowserTreeItem.h
/// @brief  a single item placed on a story tree
#pragma once

#include <QtWidgets\QTreeWidgetItem>
#include "core\types.h"
#include "core\List.h"
#include "core\ReflectionObjectChangeListener.h"


///////////////////////////////////////////////////////////////////////////////

class Story;
class StoryNode;
class Model;
class StoryNodeInstance;

///////////////////////////////////////////////////////////////////////////////

/**
 * A single item placed on a story tree.
 */
class StoryBrowserTreeItem : public QTreeWidgetItem
{
protected:
   bool                    m_previewEnabled;
   bool                    m_previewFrozen;
   StoryNodeInstance*      m_snapshotEntity;
   Model*                  m_snapshotScene;

public:
   virtual ~StoryBrowserTreeItem();

   /**
    * Initializes the item
    *
    * @param snapshotScene       scene on which we'll display level snapshots
    * @param parentItem
    */
   void initialize( Model& snapshotScene, StoryBrowserTreeItem* parentItem );

   /**
    * Deletes stored snapshot entity.
    */
   void clearSnapshot();

   /**
    * Returns a reference to the represented story node.
    */
   virtual StoryNode* getNode() = 0;

   /**
    * Adds a new node to the represented parent story node.
    *
    * @param newNode
    */
   virtual void addNode( StoryNode* newNode ) = 0;

   /**
    * Inserts nodes at the specified position
    */
   virtual void insertNode( int insertionIdx, StoryNode* nodeToInsert ) = 0;

   /**
    * Removes the represented child story node from the represented parent story node.
    *
    * @param removedChildItem
    */
   virtual void removeNode( StoryBrowserTreeItem* removedChildItem ) = 0;

   /**
    * Clears the contents of the represented BT node.
    */
   virtual void clear() = 0;

   /**
    * Is the node capable of storing embedded nodes.
    */
   virtual bool isContainerNode() const { return true; }

   /**
    * Toggles the snapshot of the chapter on the scene.
    *
    * @param first
    */
   void toggleSnapshot( bool first = true );

   /**
    * Freezes a preview for the duration of when story is played.
    */
   void freezePreview( bool freeze );

   /**
    * Updates item's description.
    */
   void updateDescription();

protected:
   /**
    * Constructor.
    */
   StoryBrowserTreeItem();

private:
   void updatePreviewEntity();
};

///////////////////////////////////////////////////////////////////////////////
