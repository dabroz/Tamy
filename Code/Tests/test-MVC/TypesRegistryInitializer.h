#pragma once

#include "core\PatchesDB.h"
#include "core-MVC\Entity.h"
#include "core-MVC\Prefab.h"
#include "core-MVC\SceneNode.h"
#include "core-MVC\Model.h"
#include "core-MVC\Component.h"
#include "core\ResourcesManager.h"
#include "core\Filesystem.h"
#include "core\MultithreadedTasksScheduler.h"


///////////////////////////////////////////////////////////////////////////////

#define MVCTESTS_INIT_TYPES_REGISTRY \
   ReflectionTypesRegistry& typesRegistry = TSingleton< ReflectionTypesRegistry >::getInstance(); \
   typesRegistry.clear(); \
   typesRegistry.addSerializableType< ReflectionObject >( "ReflectionObject", NULL ); \
   typesRegistry.addSerializableType< Resource >( "Resource", new TSerializableTypeInstantiator< Resource >() ); \
   typesRegistry.addSerializableType< SceneNode >( "SceneNode", NULL ); \
   typesRegistry.addSerializableType< Entity >( "Entity", new TSerializableTypeInstantiator< Entity >() ); \
   typesRegistry.addSerializableType< Prefab >( "Prefab", new TSerializableTypeInstantiator< Prefab >() ); \
   typesRegistry.addSerializableType< Model >( "Model", new TSerializableTypeInstantiator< Model >() ); \
   typesRegistry.addSerializableType< Component >( "Component", NULL );

///////////////////////////////////////////////////////////////////////////////

#define MVCTESTS_REGISTER_TYPE( TypeName ) \
   typesRegistry.addSerializableType< TypeName >( #TypeName, new TSerializableTypeInstantiator< TypeName >() );

///////////////////////////////////////////////////////////////////////////////

#define MVCTESTS_INIT_PATCHES \
   PatchesDB& patchesDB = TSingleton< PatchesDB >::getInstance(); \
   patchesDB.clear(); \
   typesRegistry.build( patchesDB );

///////////////////////////////////////////////////////////////////////////////

#define MVCTESTS_INIT_RESOURCE_MANAGER( RepositoryPath ) \
   ResourcesManager resMgr; \
   Filesystem filesystem( RepositoryPath ); \
   resMgr.setFilesystem( filesystem ); \

///////////////////////////////////////////////////////////////////////////////