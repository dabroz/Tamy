#pragma once

#include "core\ReflectionTypesRegistry.h"
#include "core\ReflectionObject.h"
#include "core-MVC\Entity.h"
#include "core-MVC\Model.h"
#include "core-MVC\Component.h"
#include "core-AI.h"


///////////////////////////////////////////////////////////////////////////////

#define BLENDTREETESTS_INIT_TYPES_REGISTRY() \
   ReflectionTypesRegistry& typesRegistry = TSingleton< ReflectionTypesRegistry >::getInstance(); \
   typesRegistry.clear(); \
   typesRegistry.addSerializableType< ReflectionObject >( "ReflectionObject", NULL ); \
   typesRegistry.addSerializableType< Entity >( "Entity", new TSerializableTypeInstantiator< Entity >() ); \
   typesRegistry.addSerializableType< Model >( "Model", new TSerializableTypeInstantiator< Model >() ); \
   typesRegistry.addSerializableType< Component >( "Component", NULL ); \
   typesRegistry.addSerializableType< Skeleton >( "Skeleton", new TSerializableTypeInstantiator< Skeleton >() ); \
   typesRegistry.addSerializableType< SkeletonComponent >( "SkeletonComponent", new TSerializableTypeInstantiator< SkeletonComponent >() ); \
   typesRegistry.addSerializableType< BlendTreePlayer >( "BlendTreePlayer", new TSerializableTypeInstantiator< BlendTreePlayer >() ); \
   typesRegistry.addSerializableType< SnapshotAnimation >( "SnapshotAnimation", new TSerializableTypeInstantiator< SnapshotAnimation >() ); \
   typesRegistry.addSerializableType< AnimationPlayer >( "AnimationPlayer", NULL ); \
   typesRegistry.addSerializableType< BlendTree >( "BlendTree", new TSerializableTypeInstantiator< BlendTree >() ); \
   typesRegistry.addSerializableType< BlendTreeEvent >( "BlendTreeEvent", new TSerializableTypeInstantiator< BlendTreeEvent >() ); \
   typesRegistry.addSerializableType< BlendTreeNode >( "BlendTreeNode", NULL ); \
   typesRegistry.addSerializableType< BlendTreeComposite >( "BlendTreeComposite", NULL ); \
   typesRegistry.addSerializableType< BlendTreeAnimation >( "BlendTreeAnimation", new TSerializableTypeInstantiator< BlendTreeAnimation >() ); \
   typesRegistry.addSerializableType< BlendTreeAnimationEvent >( "BlendTreeAnimationEvent", new TSerializableTypeInstantiator< BlendTreeAnimationEvent >() ); \
   typesRegistry.addSerializableType< BlendTreeSyncPoint >( "BlendTreeSyncPoint", new TSerializableTypeInstantiator< BlendTreeSyncPoint >() ); \
   typesRegistry.addSerializableType< BlendTreeStateMachine >( "BlendTreeStateMachine", new TSerializableTypeInstantiator< BlendTreeStateMachine >() ); \
   typesRegistry.addSerializableType< BlendTreeStateTransition >( "BlendTreeStateTransition", new TSerializableTypeInstantiator< BlendTreeStateTransition >() ); \
   typesRegistry.addSerializableType< BlendTreeTransitionTrigger >( "BlendTreeTransitionTrigger", NULL ); \
   typesRegistry.addSerializableType< BlendTreeTransitionEffect >( "BlendTreeTransitionEffect", NULL ); \
   typesRegistry.addSerializableType< BlendTreeSelector >( "BlendTreeSelector", new TSerializableTypeInstantiator< BlendTreeSelector >() ); \
   typesRegistry.addSerializableType< BlendTreeBlender1D >( "BlendTreeBlender1D", new TSerializableTypeInstantiator< BlendTreeBlender1D >() ); \
   typesRegistry.addSerializableType< BlendTreeVariable >( "BlendTreeVariable", NULL ); \
   typesRegistry.addSerializableType< BTVarFloat >( "BTVarFloat", new TSerializableTypeInstantiator< BTVarFloat >() );


///////////////////////////////////////////////////////////////////////////////

#define CLEANUP_TYPES_REGISTRY() \
   typesRegistry.clear();

///////////////////////////////////////////////////////////////////////////////