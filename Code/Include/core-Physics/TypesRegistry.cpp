#include "core/TypeRegistrationMacros.h"
#include "core-Physics.h"


///////////////////////////////////////////////////////////////////////////////

REGISTER_ABSTRACT_TYPE( PhysicsObject );
REGISTER_ABSTRACT_TYPE( PhysicsShape );
REGISTER_TYPE( PhysicsRigidBody );
REGISTER_TYPE( PhysicsMaterial );
REGISTER_TYPE( PhysicsCharacterController );

///////////////////////////////////////////////////////////////////////////////

REGISTER_TYPE( PSBox );
REGISTER_TYPE( PSCapsule );
REGISTER_TYPE( PSPlane );
REGISTER_TYPE( PSSphere );
REGISTER_TYPE( PSTriangleMesh );

///////////////////////////////////////////////////////////////////////////////
