#include "core/TypeRegistrationMacros.h"
#include "core-AI.h"


///////////////////////////////////////////////////////////////////////////////

// BehaviorTree resource core
REGISTER_TYPE( BehaviorTree );
REGISTER_ABSTRACT_TYPE( BehTreeNode );
REGISTER_ABSTRACT_TYPE( BehTreeComposite );
REGISTER_ABSTRACT_TYPE( BehTreeDecorator );
REGISTER_ABSTRACT_TYPE( BehTreeAction );

// sequences
REGISTER_TYPE( BehTreeSequence );
REGISTER_TYPE( BehTreeParallel );

// decorators
REGISTER_TYPE( BehTreeRepeater );
REGISTER_TYPE( BehTreeSelector );

// actions
REGISTER_TYPE( BehTreeSetBool );

// conditions
REGISTER_ABSTRACT_TYPE( BehTreeCondition );
REGISTER_TYPE( BehTreeBoolCondition );

// variables
REGISTER_ABSTRACT_TYPE( BehaviorTreeVariable );
REGISTER_TYPE( BehTreeVarFloat );
REGISTER_TYPE( BehTreeVarBool );
REGISTER_TYPE( BehTreeVarInt );
REGISTER_TYPE( BehTreeVarVector );
REGISTER_TYPE( BehTreeVarReflectionObject );

///////////////////////////////////////////////////////////////////////////////

REGISTER_ABSTRACT_TYPE( AnimationPlayer );
REGISTER_TYPE( SkeletonComponent );
REGISTER_TYPE( SkeletonAnimationPlayer );
REGISTER_TYPE( SnapshotAnimation );
REGISTER_TYPE( Skeleton );
REGISTER_TYPE( EntityAnimationPlayer );
REGISTER_ENUM_TYPE( BoneLookupMethod );

///////////////////////////////////////////////////////////////////////////////

REGISTER_TYPE( BlendTree );
REGISTER_ABSTRACT_TYPE( BlendTreeNode );
REGISTER_ABSTRACT_TYPE( BlendTreeComposite );
REGISTER_TYPE( BlendTreeEvent );
REGISTER_TYPE( BlendTreePlayer );
REGISTER_TYPE( BlendTreeStateMachine );
REGISTER_TYPE( BlendTreeStateTransition );
REGISTER_TYPE( BlendTreeSelector );

// blenders
REGISTER_TYPE( BlendTreeBlender1D );

// pose generators
REGISTER_TYPE( BlendTreeAnimation );
REGISTER_TYPE( BlendTreeAnimationEvent );
REGISTER_TYPE( BlendTreeSyncPoint );

// variables
REGISTER_ABSTRACT_TYPE( BlendTreeVariable );
REGISTER_TYPE( BTVarFloat );
REGISTER_TYPE( BTVarBool );
REGISTER_TYPE( BTVarInt );
REGISTER_TYPE( BTVarVector );

// transition triggers
REGISTER_ABSTRACT_TYPE( BlendTreeTransitionTrigger );
REGISTER_TYPE( BTTTEvent );
REGISTER_TYPE( BTTTVariable );
REGISTER_ENUM_TYPE( ComparisonOp );

// transition effects
REGISTER_ABSTRACT_TYPE( BlendTreeTransitionEffect );
REGISTER_TYPE( BTTEBlend );

// selector switches
REGISTER_ABSTRACT_TYPE( BlendTreeSelectorSwitch );
REGISTER_TYPE( BTSSVariable );

///////////////////////////////////////////////////////////////////////////////
