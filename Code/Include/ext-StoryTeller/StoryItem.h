/// @file   ext-StoryTeller/StoryItem.h
/// @brief  a StoryItem definition
#pragma once

#include "ext-StoryTeller\StoryNode.h"
#include "core\List.h"
#include "core\FilePath.h"
#include "core\Matrix.h"
#include "core\ReflectionObjectChangeListener.h"


///////////////////////////////////////////////////////////////////////////////

class Prefab;

///////////////////////////////////////////////////////////////////////////////

/**
 * An item used in a story.
 *
 * ASSUMPTION: for now we assume that each item has only 2 states - unused and used.
 * Moreover - when used, an item can change the state of another item, to which
 * it's bound by definition
 */
class StoryItem : public StoryNode, public ReflectionObjectChangeListener
{
   DECLARE_CLASS();
   DECLARE_ALLOCATOR( StoryItem, AM_DEFAULT );

private:
   // static data
   FilePath                         m_thumbnailPath;
   bool                             m_uniqueItem;
   Matrix                           m_transform;

   Prefab*                          m_itemPrefab;

public:
   /**
    * Constructor.
    */
   StoryItem();
   ~StoryItem();

   /**
    * Sets a new world transform for the item.
    */
   void setTransform( const Matrix& transform );

   /**
    * Returns the world transform of the item.
    */
   inline const Matrix& getTransform() const { return m_transform; }

   /**
    * Sets a prefab that should be used as item's representation.
    *
    * @param itemPrefab
    */
   void setItemPrefab( Prefab* itemPrefab );

   // ----------------------------------------------------------------------
   // Characteristics
   // ----------------------------------------------------------------------
   /**
    * Tells if this item can have only one instance.
    */
   inline bool isUniqueItem() const { return m_uniqueItem; }

   /**
    * Returns the path to item's thumbnail.
    */
   inline const FilePath& getThumbnailPath() const { return m_thumbnailPath; }

   // -------------------------------------------------------------------------
   // StoryNode implementation
   // -------------------------------------------------------------------------
   void pullStructure( StoryListener* listener );
   void onHostStorySet( Story* story );
   Prefab* getRepresentationPrefab() { return m_itemPrefab; }
   StoryNodeInstance* instantiate();
   BehaviorTree* getLogics() const { return NULL; }

   // -------------------------------------------------------------------------
   // ReflectionObjectChangeListener implementation
   // -------------------------------------------------------------------------
   void onObservedPropertyChanged( ReflectionProperty& property );
   void onObservedObjectDeleted( ReflectionObject* deletedObject );
};

///////////////////////////////////////////////////////////////////////////////
