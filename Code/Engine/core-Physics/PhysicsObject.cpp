#include "core-Physics\PhysicsObject.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_ABSTRACT_OBJECT( PhysicsObject );
   PARENT( Component );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

PhysicsObject::PhysicsObject( const char* name )
   : Component( name )
{
}

///////////////////////////////////////////////////////////////////////////////

PhysicsObject::PhysicsObject( const PhysicsObject& rhs )
   : Component( rhs )
{
}

///////////////////////////////////////////////////////////////////////////////

PhysicsObject::~PhysicsObject()
{
}

///////////////////////////////////////////////////////////////////////////////
