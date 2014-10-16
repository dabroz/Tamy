/// @file   TamyEditor/SCTIRoot.h
/// @brief  scene item that represents the root of a scene.
#pragma once

#include "SceneTreeItem.h"
#include "core\List.h"


///////////////////////////////////////////////////////////////////////////////

class Model;
class Entity;

///////////////////////////////////////////////////////////////////////////////

/**
 * Scene item that represents the root of a scene.
 */
class SCTIRoot : public SceneTreeItem
{
private:
   Model&        m_scene;
 
public:
   /**
    * Constructor.
    *
    * @param scene
    */
   SCTIRoot( Model& scene );
   ~SCTIRoot();

   // -------------------------------------------------------------------------
   // SceneTreeItem implementation
   // -------------------------------------------------------------------------
   SceneNode* getNode() const;
   bool canAccommodate( SceneNode* node ) const;
   void addNode( SceneNode* newNode );
   void removeNode( SceneTreeItem* removedChildItem );
   void clear();
   void updateDescription();
};

///////////////////////////////////////////////////////////////////////////////
