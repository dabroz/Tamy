/// @file   TamyEditor/SCTIComponent.h
/// @brief  scene item that represents a single component.
#pragma once

#include "SceneTreeItem.h"
#include "core\List.h"


///////////////////////////////////////////////////////////////////////////////

class Component;

///////////////////////////////////////////////////////////////////////////////

/**
 * Scene item that represents the root of a component.
 */
class SCTIComponent : public SceneTreeItem
{
private:
   Component&        m_component;
 
public:
   /**
    * Constructor.
    *
    * @param entity
    */
   SCTIComponent( Component& component );
   ~SCTIComponent();

   // -------------------------------------------------------------------------
   // SceneTreeItem implementation
   // -------------------------------------------------------------------------
   SceneNode* getNode() const;
   bool isContainerNode() const { return false; }
   bool canAccommodate( SceneNode* node ) const;
   void addNode( SceneNode* newNode );
   void removeNode( SceneTreeItem* removedChildItem );
   void clear();
};

///////////////////////////////////////////////////////////////////////////////
