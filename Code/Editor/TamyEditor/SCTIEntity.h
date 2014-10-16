/// @file   TamyEditor/SCTIEntity.h
/// @brief  scene item that represents a single entity.
#pragma once

#include "SceneTreeItem.h"
#include "core\List.h"
#include <QtGui\QPixmap>


///////////////////////////////////////////////////////////////////////////////

class Model;
class Entity;

///////////////////////////////////////////////////////////////////////////////

/**
 * Scene item that represents the root of a scene.
 */
class SCTIEntity : public SceneTreeItem
{
private:
   Entity&        m_entity;
   QPixmap        m_originalIcon;
 
public:
   /**
    * Constructor.
    *
    * @param entity
    */
   SCTIEntity( Entity& entity );
   ~SCTIEntity();

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
