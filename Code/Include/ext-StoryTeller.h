/// @file   ext-StoryTeller.h
/// @brief  aggregate include file for the 'ext-StoryTeller' project
#pragma once


// ----------------------------------------------------------------------------
// Actors
// ----------------------------------------------------------------------------
#include "ext-StoryTeller\StoryActor.h"
// ----------------------------------------------------------------------------
// -->Actions
// ----------------------------------------------------------------------------
#include "ext-StoryTeller\SASpawnActor.h"

// ----------------------------------------------------------------------------
// Chapters
// ----------------------------------------------------------------------------
#include "ext-StoryTeller\StoryAction.h"
#include "ext-StoryTeller\StoryCondition.h"
#include "ext-StoryTeller\StoryChapter.h"
#include "ext-StoryTeller\StoryActionSockets.h"
// ----------------------------------------------------------------------------
// -->Actions
// ----------------------------------------------------------------------------
#include "ext-StoryTeller\StoryChapterStart.h"
#include "ext-StoryTeller\SAActivateChapter.h"
#include "ext-StoryTeller\SACondition.h"
#include "ext-StoryTeller\SAPause.h"
// ----------------------------------------------------------------------------
// -->Conditions
// ----------------------------------------------------------------------------
#include "ext-StoryTeller\SCWaitForAMoment.h"
#include "ext-StoryTeller\SCActorInTrigger.h"

// ----------------------------------------------------------------------------
// Core
// ----------------------------------------------------------------------------
#include "ext-StoryTeller\Story.h"
#include "ext-StoryTeller\StoryNode.h"
#include "ext-StoryTeller\StoryListener.h"

// ----------------------------------------------------------------------------
// Items
// ----------------------------------------------------------------------------
#include "ext-StoryTeller\StoryItem.h"
#include "ext-StoryTeller\SITrigger.h"
// ----------------------------------------------------------------------------
// -->Actions
// ----------------------------------------------------------------------------
#include "ext-StoryTeller\SASpawnItem.h"

// ----------------------------------------------------------------------------
// Runtime
// ----------------------------------------------------------------------------
#include "ext-StoryTeller\StoryPlayer.h"
#include "ext-StoryTeller\StoryBehTreeContext.h"
#include "ext-StoryTeller\StoryNodeInstance.h"

// ----------------------------------------------------------------------------
// BehTreeNodes
// ----------------------------------------------------------------------------
// --> Animation
// ----------------------------------------------------------------------------
#include "ext-StoryTeller\BTAPhysicsToAnimation.h"
#include "ext-StoryTeller\BTAAnimationToPhysics.h"
#include "ext-StoryTeller\BTAMoveOnAnimation.h"
// ----------------------------------------------------------------------------
// --> Camera
// ----------------------------------------------------------------------------
#include "ext-StoryTeller\BTATopDownCameraController.h"
#include "ext-StoryTeller\BTAThirdPersonCamera.h"
// ----------------------------------------------------------------------------
// --> Inputs
// ----------------------------------------------------------------------------
#include "ext-StoryTeller\BTAInputToWorldPos.h"
#include "ext-StoryTeller\BTAInputToVelocity.h"
#include "ext-StoryTeller\BTAInputCondition.h"
// ----------------------------------------------------------------------------
// --> ActorActions
// ----------------------------------------------------------------------------
#include "ext-StoryTeller\BTAMoveTo.h"
#include "ext-StoryTeller\BTAMoveWithVelocity.h"
#include "ext-StoryTeller\BTASlideTo.h"
// ----------------------------------------------------------------------------
// --> Math
// ----------------------------------------------------------------------------
#include "ext-StoryTeller\BTATransformVec.h"
// ----------------------------------------------------------------------------
// --> SteeringBehaviors
// ----------------------------------------------------------------------------
#include "ext-StoryTeller\BTAArrive.h"
#include "ext-StoryTeller\BTAPursue.h"
// ----------------------------------------------------------------------------
// --> World
// ----------------------------------------------------------------------------
#include "ext-StoryTeller\BTAGetActorInfo.h"
#include "ext-StoryTeller\BTAClosestActorInDirection.h"

// ----------------------------------------------------------------------------
// Locations
// ----------------------------------------------------------------------------
#include "ext-StoryTeller\StoryLevelAnnotation.h"
#include "ext-StoryTeller\SAGetRandomAnnotation.h"

