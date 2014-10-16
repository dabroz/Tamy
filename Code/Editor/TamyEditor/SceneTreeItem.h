/// @file   TamyEditor/SceneTreeItem.h
/// @brief   a single item placed on a scene tree
#pragma once

#include <QtWidgets\QTreeWidgetItem>
#include "core\types.h"
#include "core\List.h"
#include "core\ReflectionObjectChangeListener.h"


///////////////////////////////////////////////////////////////////////////////

class SceneNode;
class Model;

///////////////////////////////////////////////////////////////////////////////

/**
 * A single item placed on a scene tree.
 */
class SceneTreeItem : public QTreeWidgetItem
{
private:
   bool m_canStoreEntities;
   bool m_canStoreComponents;
   bool m_canBeTurnedIntoPrefab;

public:
   /**
    * Constructor.
    *
    * @param canStoreEntities
    * @param canStoreComponents
    * @param canBeTurnedIntoPrefab
    */
   SceneTreeItem( bool canStoreEntities, bool canStoreComponents, bool canBeTurnedIntoPrefab );
   virtual ~SceneTreeItem();

   /**
    * Returns a reference to the represented BT node.
    */
   virtual SceneNode* getNode() const = 0;

   /**
    * Checks if this node can store other nodes.
    */
   bool isContainerNode() const { return m_canStoreEntities || m_canStoreEntities; }

   /**
    * Tells if entities can be added to this node
    */
   bool canStoreEntities() const { return m_canStoreEntities; }

   /**
    * Tells if components can be added to this node.
    */
   bool canStoreComponents() const { return m_canStoreComponents; }

   /**
    * Tells if the represented node can be turned into a prefab
    */
   bool canBeTurnedIntoPrefab() const;

   /**
    * Checks if this node can accommodate the specified node.
    *
    * @param node
    */
   virtual bool canAccommodate( SceneNode* node ) const = 0;

   /**
    * Adds a new node to the represented parent BT node.
    *
    * @param newNode
    */
   virtual void addNode( SceneNode* newNode ) = 0;

   /**
    * Removes the represented child scene node from the represented parent scene node.
    *
    * @param removedChildItem
    */
   virtual void removeNode( SceneTreeItem* removedChildItem ) = 0;

   /**
    * Clears the contents of the represented BT node.
    */
   virtual void clear() = 0;

   /**
    * Updates item's description.
    */
   virtual void updateDescription();

private:
   void updatePreviewEntity();
};

///////////////////////////////////////////////////////////////////////////////
