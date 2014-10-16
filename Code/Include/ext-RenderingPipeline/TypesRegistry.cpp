#include "core/TypeRegistrationMacros.h"
#include "ext-RenderingPipeline.h"


///////////////////////////////////////////////////////////////////////////////

#pragma region Components

REGISTER_TYPE( LocationRenderSettings );

#pragma endregion

#pragma region Utils

REGISTER_ABSTRACT_TYPE( LightsArray );
REGISTER_ABSTRACT_TYPE( GeometryArray );
REGISTER_ABSTRACT_TYPE( RPBoundingVolume );
REGISTER_ABSTRACT_TYPE( RPVolFrustum );
REGISTER_ABSTRACT_TYPE( RPVolAxisAlignedBox );

#pragma endregion

#pragma region MaterialNodes

REGISTER_TYPE( MNPhongBlinn );
REGISTER_TYPE( MNCookTorrance );
REGISTER_TYPE( MNEntityPtr );
REGISTER_TYPE( MNBoneMatrices );

#pragma endregion


#pragma region Rendering mechanism

REGISTER_ENUM_TYPE( RenderPassSelection );

#pragma endregion

///////////////////////////////////////////////////////////////////////////////
