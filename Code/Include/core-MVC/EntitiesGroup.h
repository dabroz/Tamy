/// @file   core-MVC\EntitiesGroup.h
/// @brief  a group of entities
#pragma once

#include "core\MemoryRouter.h"
#include "core\List.h"


///////////////////////////////////////////////////////////////////////////////

class EntitiesGroupView;
class Entity;

///////////////////////////////////////////////////////////////////////////////

/**
 * Models are resources that contain a set of entities that work together and
 * create a scene.
 * An entity BELONGS to a model - if we add it to another model, it will automatically
 * be removed from the previous one.
 *
 * But what if we want to select a bunch of entities - in order to make a selection etc.
 * For this purpose, an EntityGroup was created.
 * It stores entities that can be attached to another model,
 * but doesn't take ownership - they won't be pulled of their parent model.
 *
 * An entities group IS NOT AWARE of entity hierarchies - it considers every entity
 * added as an individual being.
 */
class EntitiesGroup
{
   DECLARE_ALLOCATOR( EntitiesGroup, AM_DEFAULT );

private:
   List< EntitiesGroupView* >                m_views;
   List< Entity* >                           m_entities;

public:
   /**
    * Constructor.
    */
   EntitiesGroup();
   ~EntitiesGroup();

   /**
    * Adds an entity to the group.
    *
    * @param entity
    */
   void addEntity( Entity* entity );

   /**
    * Removes an entity to the group.
    *
    * @param entity
    */
   void removeEntity( Entity* entity );

   /**
    * Clears the group.
    */
   void clear();

   // -------------------------------------------------------------------------
   // Views management
   // -------------------------------------------------------------------------
   /**
    * Attaches a new view to the group.
    *
    * @param view                new group observer
    */
   void attachListener( EntitiesGroupView* view );

   /**
    * Detaches a view from the group.
    *
    * @param view    observer we want to detach
    */
   void detachListener( EntitiesGroupView* view );

   /**
    * Pulls the scene structure.
    *
    * @param view
    */
   void pullStructure( EntitiesGroupView* view );
};

///////////////////////////////////////////////////////////////////////////////
