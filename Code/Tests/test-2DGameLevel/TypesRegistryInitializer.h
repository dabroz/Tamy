#pragma once

#include "core\PatchesDB.h"
#include "core-MVC\Entity.h"
#include "core-MVC\SceneNode.h"
#include "core-MVC\Model.h"
#include "core-MVC\Component.h"
#include "ext-2DGameLevel\GL2DBody.h"
#include "ext-2DGameLevel\GL2DShape.h"
#include "core\ResourcesManager.h"
#include "core\Filesystem.h"
#include "core\MultithreadedTasksScheduler.h"

// shapes
#include "ext-2DGameLevel\GL2DCircleShape.h"
#include "ext-2DGameLevel\GL2DLineSegmentsShape.h"


///////////////////////////////////////////////////////////////////////////////

#define GAMELEVEL2DTESTS_INIT_TYPES_REGISTRY \
   ReflectionTypesRegistry& typesRegistry = TSingleton< ReflectionTypesRegistry >::getInstance( ); \
   typesRegistry.clear( ); \
   typesRegistry.addSerializableType< ReflectionObject >( "ReflectionObject", NULL ); \
   typesRegistry.addSerializableType< Resource >( "Resource", new TSerializableTypeInstantiator< Resource >( ) ); \
   typesRegistry.addSerializableType< SceneNode >( "SceneNode", NULL ); \
   typesRegistry.addSerializableType< Entity >( "Entity", new TSerializableTypeInstantiator< Entity >( ) ); \
   typesRegistry.addSerializableType< Model >( "Model", new TSerializableTypeInstantiator< Model >( ) ); \
   typesRegistry.addSerializableType< Component >( "Component", NULL ); \
   typesRegistry.addSerializableType< GL2DBody >( "GL2DBody", new TSerializableTypeInstantiator< GL2DBody >( ) ); \
   typesRegistry.addSerializableType< GL2DShape >( "GL2DShape", NULL ); \
   typesRegistry.addSerializableType< GL2DCircleShape >( "GL2DCircleShape", new TSerializableTypeInstantiator< GL2DCircleShape >( ) ); \
   typesRegistry.addSerializableType< GL2DLineSegmentsShape >( "GL2DLineSegmentsShape", new TSerializableTypeInstantiator< GL2DLineSegmentsShape >( ) ); 

///////////////////////////////////////////////////////////////////////////////

#define GAMELEVEL2DTESTS_REGISTER_TYPE( TypeName ) \
   typesRegistry.addSerializableType< TypeName >( #TypeName, new TSerializableTypeInstantiator< TypeName >( ) );

///////////////////////////////////////////////////////////////////////////////
