/// @file   core-MVC\EntityListener.h
/// @brief  an Entity listener
#pragma once


///////////////////////////////////////////////////////////////////////////////

class Entity;
class SceneNode;

///////////////////////////////////////////////////////////////////////////////

class EntityListener
{
public:
   virtual ~EntityListener() {}

   /**
    * Called when a child is attached to the specified entity
    *
    * @param parentEntity
    * @param child
    */
   virtual void onChildAttached( Entity* parentEntity, SceneNode* child ) {}

   /**
    * Called when a child is detached from the specified entity.
    *
    * @param parentEntity
    * @param child
    */
   virtual void onChildDetached( Entity* parentEntity, SceneNode* child ) {}
};

///////////////////////////////////////////////////////////////////////////////
