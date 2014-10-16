/// @file   core-AI.h
/// @brief  aggregate include file for the 'core-AI' project
#pragma once


// ----------------------------------------------------------------------------
// AISystem
// -->Animation
// ----------------------------------------------------------------------------
#include "core-AI\SnapshotAnimation.h"
#include "core-AI\BoneSRTAnimation.h"
// ----------------------------------------------------------------------------
// -->Core
// ----------------------------------------------------------------------------
#include "core-AI\AnimationPlayer.h"
#include "core-AI\AnimationWorld.h"
// ----------------------------------------------------------------------------
// -->SkeletalAnimation
// ----------------------------------------------------------------------------
#include "core-AI\SkeletonComponent.h"
#include "core-AI\SkeletonAnimationPlayer.h"
#include "core-AI\Skeleton.h"
// ----------------------------------------------------------------------------
// -->SpatialEntityAnimation
// ----------------------------------------------------------------------------
#include "core-AI\EntityAnimationPlayer.h"
// ----------------------------------------------------------------------------
// -->Utils
// ----------------------------------------------------------------------------
#include "core-AI\PoseBlendingUtils.h"


// ----------------------------------------------------------------------------
// BlendTree
// ----------------------------------------------------------------------------
// -->Resource
// ----------------------------------------------------------------------------
#include "core-AI\BlendTree.h"
#include "core-AI\BlendTreeListener.h"
#include "core-AI\BlendTreeNode.h"
#include "core-AI\BlendTreeComposite.h"
#include "core-AI\BlendTreeEvent.h"
#include "core-AI\BlendTreeVariable.h"
// ----------------------------------------------------------------------------
// -->Blenders
// ----------------------------------------------------------------------------
#include "core-AI\BlendTreeBlender1D.h"
// ----------------------------------------------------------------------------
// -->PoseGenerators
// ----------------------------------------------------------------------------
#include "core-AI\BlendTreeAnimation.h"
#include "core-AI\BlendTreeAnimationListener.h"
#include "core-AI\BlendTreeAnimationEvent.h"
#include "core-AI\BlendTreeSyncPoint.h"
// ----------------------------------------------------------------------------
// -->StateMachine
// ----------------------------------------------------------------------------
#include "core-AI\BlendTreeStateMachine.h"
#include "core-AI\BlendTreeStateTransition.h"
#include "core-AI\BlendTreeTransitionTrigger.h"
#include "core-AI\BTTTEvent.h"
#include "core-AI\BTTTVariable.h"
// ----------------------------------------------------------------------------
// -->Selector
// ----------------------------------------------------------------------------
#include "core-AI\BlendTreeSelector.h"
#include "core-AI\BlendTreeSelectorSwitch.h"
#include "core-AI\BTSSVariable.h"
// ----------------------------------------------------------------------------
// -->TransitionEffects
// ----------------------------------------------------------------------------
#include "core-AI\BlendTreeTransitionEffect.h"
#include "core-AI\BTTEBlend.h"
// ----------------------------------------------------------------------------
// -->Runtime
// ----------------------------------------------------------------------------
#include "core-AI\BlendTreePlayer.h"
#include "core-AI\BlendTreePlayerListener.h"
// ----------------------------------------------------------------------------
// -->Synchronization
// ----------------------------------------------------------------------------
#include "core-AI\BlendTreeSyncProfile.h"
#include "core-AI\BlendTreeAnimationTrack.h"

// ----------------------------------------------------------------------------
// Core
// ----------------------------------------------------------------------------
#include "core-AI\AISystem.h"

// ----------------------------------------------------------------------------
// ControlStructures
// ----------------------------------------------------------------------------
// ---> BehaviorTrees
// ----->Resource
// ----------------------------------------------------------------------------
#include "core-AI\BehaviorTree.h"
#include "core-AI\BehTreeNode.h"
#include "core-AI\BehTreeComposite.h"
#include "core-AI\BehTreeDecorator.h"
#include "core-AI\BehTreeAction.h"
#include "core-AI\BehTreeVariable.h"
// ----------------------------------------------------------------------------
// -----> Composites
// ----------------------------------------------------------------------------
#include "core-AI\BehTreeSequence.h"
#include "core-AI\BehTreeParallel.h"
// ----------------------------------------------------------------------------
// -----> Decorators
// ----------------------------------------------------------------------------
#include "core-AI\BehTreeRepeater.h"
#include "core-AI\BehTreeSelector.h"
// ----------------------------------------------------------------------------
// -----> Actions
// ----------------------------------------------------------------------------
#include "core-AI\BehTreeSetBool.h"
// ----------------------------------------------------------------------------
// -----> Conditions
// ----------------------------------------------------------------------------
#include "core-AI\BehTreeCondition.h"
#include "core-AI\BehTreeBoolCondition.h"
// ----------------------------------------------------------------------------
// -----> Runtime
// ----------------------------------------------------------------------------
#include "core-AI\BehaviorTreeRunner.h"
// ----------------------------------------------------------------------------
// ---> FSM
// ----------------------------------------------------------------------------
#include "core-AI\FSMState.h"
#include "core-AI\FSMController.h"
