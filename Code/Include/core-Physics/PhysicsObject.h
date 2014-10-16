/// @file   core-Physics\PhysicsObject.h
/// @brief  a base class for different types of physics objects populating a physics world
#pragma once

#include "core-MVC\Component.h"


///////////////////////////////////////////////////////////////////////////////

class PhysicsWorld;

///////////////////////////////////////////////////////////////////////////////

class PhysicsObject : public Component
{
   DECLARE_ALLOCATOR( PhysicsObject, AM_DEFAULT );
   DECLARE_CLASS();

public:
   /**
    * Constructor.
    *
    * @param name
    */
   PhysicsObject( const char* name = "PhysicsObject" );

   /**
    * Copy constructor.
    *
    * @param rhs
    */
   PhysicsObject( const PhysicsObject& rhs );
   virtual ~PhysicsObject();

   // -------------------------------------------------------------------------
   // Instantiation
   // -------------------------------------------------------------------------
   /**
    * Adds the object to the specified physics world.
    *
    * @param physicsWorld
    */
   virtual void addToWorld( PhysicsWorld& physicsWorld ) {}

   /**
    * Removes the object from the specified physics world.
    *
    * @param physicsWorld
    */
   virtual void removeFromWorld( PhysicsWorld& physicsWorld ) {}

   // -------------------------------------------------------------------------
   // Updates
   // -------------------------------------------------------------------------

   /**
    * Updates the state of the object before the simulation frame.
    *
    * @param deltaTime
    */
   virtual void tick( float deltaTime ) {}

};

///////////////////////////////////////////////////////////////////////////////
