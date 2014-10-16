#include "ext-StoryTeller.h"
#include "core\TypeRegistrationMacros.h"


///////////////////////////////////////////////////////////////////////////////

REGISTER_TYPE( Story );
REGISTER_ABSTRACT_TYPE( StoryNode );

///////////////////////////////////////////////////////////////////////////////

REGISTER_ABSTRACT_TYPE( StoryAction );
REGISTER_ABSTRACT_TYPE( StoryCondition );
DEFINE_TYPE_ID( GBNodeOutput<StoryAction> );
MAP_TYPE( GBNodeInput<StoryAction>, ReflectionObject );
REGISTER_TYPE( SAVoidInput );
REGISTER_TYPE( SAVoidOutput );
REGISTER_TYPE( SAAnnotationInput );
REGISTER_TYPE( SAAnnotationOutput );

///////////////////////////////////////////////////////////////////////////////

// nodes
REGISTER_TYPE( StoryChapter );
REGISTER_TYPE( StoryItem );
REGISTER_ABSTRACT_TYPE( StoryNodeInstance );
REGISTER_TYPE( StoryActor );

///////////////////////////////////////////////////////////////////////////////

// items
REGISTER_TYPE( SITrigger );

///////////////////////////////////////////////////////////////////////////////

// actions
REGISTER_TYPE( StoryChapterStart );
REGISTER_TYPE( SAActivateChapter );
REGISTER_TYPE( SACondition );
REGISTER_TYPE( SAPause );
REGISTER_TYPE( SASpawnItem );
REGISTER_TYPE( SASpawnActor );

///////////////////////////////////////////////////////////////////////////////

// conditions
REGISTER_TYPE( SCWaitForAMoment );
REGISTER_TYPE( SCActorInTrigger );

///////////////////////////////////////////////////////////////////////////////

// beh tree actions
REGISTER_TYPE( BTAPhysicsToAnimation );
REGISTER_TYPE( BTAAnimationToPhysics );
REGISTER_TYPE( BTAMoveOnAnimation );
REGISTER_TYPE( BTATopDownCameraController );
REGISTER_TYPE( BTAThirdPersonCamera );
REGISTER_TYPE( BTAInputToWorldPos );
REGISTER_TYPE( BTAInputToVelocity );
REGISTER_TYPE( BTAInputCondition );
REGISTER_ABSTRACT_TYPE( PlayerSceneQuery );
REGISTER_TYPE( BTAMoveTo );
REGISTER_TYPE( BTAMoveWithVelocity );
REGISTER_TYPE( BTASlideTo );
REGISTER_TYPE( BTATransformVec );
REGISTER_TYPE( BTAGetActorInfo );
REGISTER_TYPE( BTAClosestActorInDirection );
REGISTER_TYPE( BTAArrive );
REGISTER_TYPE( BTAPursue );

REGISTER_ENUM_TYPE( BTAVecTranformSpace );

///////////////////////////////////////////////////////////////////////////////

REGISTER_TYPE( StoryLevelAnnotation );
REGISTER_TYPE( SAGetRandomAnnotation );

///////////////////////////////////////////////////////////////////////////////
