#pragma once

#include "core\PatchesDB.h"
#include "core-MVC\Entity.h"
#include "core-MVC\SceneNode.h"
#include "core-MVC\Model.h"
#include "core-MVC\Component.h"
#include "core\ResourcesManager.h"
#include "core\Filesystem.h"


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

///////////////////////////////////////////////////////////////////////////////

#define GAMELEVEL2DTESTS_REGISTER_TYPE( TypeName ) \
   typesRegistry.addSerializableType< TypeName >( #TypeName, new TSerializableTypeInstantiator< TypeName >( ) );

///////////////////////////////////////////////////////////////////////////////
