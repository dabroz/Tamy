#include "core/TypeRegistrationMacros.h"
#include <QPointF>


///////////////////////////////////////////////////////////////////////////////

#include "Project.h"
REGISTER_TYPE( Project );

///////////////////////////////////////////////////////////////////////////////

#include "NavigationState.h"
#include "GizmoNodeManipulationState.h"
#include "DisabledState.h"

REGISTER_TYPE( NavigationState );
REGISTER_TYPE( GizmoNodeManipulationState );
REGISTER_TYPE( DisabledState );

///////////////////////////////////////////////////////////////////////////////

REGISTER_EXTERNAL_TYPE( QPointF );

///////////////////////////////////////////////////////////////////////////////

#include "DebugGeometry.h"
#include "DRDirectionalLight.h"
#include "DRPointLight.h"
#include "DRSpotLight.h"
#include "DRGeometry.h"
#include "DRSkeletonEntity.h"
#include "DRStoryLevelAnnotation.h"
#include "GizmoAxis.h"
#include "Gizmo.h"
#include "DebugGrid.h"
#include "DebugEntitiesManager.h"
REGISTER_ABSTRACT_TYPE( DebugGeometry );
REGISTER_ABSTRACT_TYPE( DebugGeometryComponent );
REGISTER_ABSTRACT_TYPE( GizmoAxis );
REGISTER_ABSTRACT_TYPE( GizmoAxisGeometryComponent );
REGISTER_ABSTRACT_TYPE( Gizmo );
REGISTER_ABSTRACT_TYPE( DebugGrid );
REGISTER_ABSTRACT_TYPE( DRDirectionalLight );
REGISTER_ABSTRACT_TYPE( DRPointLight );
REGISTER_ABSTRACT_TYPE( DRSpotLight );
REGISTER_ABSTRACT_TYPE( DRGeometry );
REGISTER_ABSTRACT_TYPE( DRSkeletonEntity );
REGISTER_ABSTRACT_TYPE( DRStoryLevelAnnotation );

REGISTER_ENUM_TYPE( DebugFeature );

///////////////////////////////////////////////////////////////////////////////

#include "MaterialRoutine.h"

REGISTER_TYPE( MaterialRoutine );

///////////////////////////////////////////////////////////////////////////////
