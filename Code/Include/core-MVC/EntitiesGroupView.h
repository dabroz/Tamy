/// @file   core-MVC\EntitiesGroupView.h
/// @brief  EntitiesGroup view
#pragma once


#include "core\List.h"


///////////////////////////////////////////////////////////////////////////////

class EntitiesGroup;
class Entity;

///////////////////////////////////////////////////////////////////////////////

/**
 * This class is an entities group observer.
 */
class EntitiesGroupView 
{
private:
   List< EntitiesGroup* >          m_groups;

public:
   virtual ~EntitiesGroupView();

   /**
    * This method is called when a new entity is added to the observed model.
    *
    * @param entity
    */
   virtual void onEntityAdded( Entity* entity ) {}

   /**
    * This method is called just before a entity is removed from the observed 
    * model.
    *
    * @param entity
    */
   virtual void onEntityRemoved( Entity* entity ) {}

   // -------------------------------------------------------------------------
   // Model-View link management
   // -------------------------------------------------------------------------
   /**
    * A EntitiesGroup instance calls this method to inform the view about the link
    * they now create.
    *
    * @param group   instance of EntitiesGroup the view is now observing
    */
   virtual void onAttachedToGroup( EntitiesGroup& group );

   /**
    * A EntitiesGroup instance calls this method to inform the view that the link
    * they created was broken and they are no longer connected.
    *
    * @param group   instance of EntitiesGroup the view used to be observing.
    */
   virtual void onDetachedFromGroup( EntitiesGroup& group );

protected:
   /**
    * Constructor.
    */
   EntitiesGroupView();
};

///////////////////////////////////////////////////////////////////////////////
