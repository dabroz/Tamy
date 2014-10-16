#include "core.h"
#include "core/TypeRegistrationMacros.h"

///////////////////////////////////////////////////////////////////////////////

REGISTER_CORE_TYPES();

///////////////////////////////////////////////////////////////////////////////

REGISTER_EXTERNAL_TYPE( Point );

///////////////////////////////////////////////////////////////////////////////

REGISTER_EXTERNAL_TYPE( FastFloat );
REGISTER_EXTERNAL_TYPE( EulerAngles );
REGISTER_EXTERNAL_TYPE( Matrix );
REGISTER_EXTERNAL_TYPE( Vector );
REGISTER_EXTERNAL_TYPE( Plane );
REGISTER_EXTERNAL_TYPE( Quaternion );
REGISTER_EXTERNAL_TYPE( Transform );
REGISTER_EXTERNAL_TYPE( Color );
REGISTER_EXTERNAL_TYPE( Triangle );
REGISTER_EXTERNAL_TYPE( Sphere );
REGISTER_EXTERNAL_TYPE( AxisAlignedBox );

///////////////////////////////////////////////////////////////////////////////

REGISTER_EXTERNAL_TYPE( FilePath );
REGISTER_ABSTRACT_TYPE( Resource );
REGISTER_TYPE( ResourceHandle );
REGISTER_EXTERNAL_TYPE( RefPtr );

///////////////////////////////////////////////////////////////////////////////
