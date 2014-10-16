#pragma once

#include "core\PatchesDB.h"
#include "core\PatchesDB.h"
#include "core-MVC\Entity.h"
#include "core-MVC\SceneNode.h"
#include "core-MVC\Model.h"
#include "core-MVC\Component.h"
#include "core-MVC\Prefab.h"


///////////////////////////////////////////////////////////////////////////////

#define STORYTELLERTESTS_INIT_TYPES_REGISTRY \
   ReflectionTypesRegistry& typesRegistry = TSingleton< ReflectionTypesRegistry >::getInstance( ); \

///////////////////////////////////////////////////////////////////////////////

#define STORYTELLERTESTS_REGISTER_TYPE( TypeName ) \
   typesRegistry.addSerializableType< TypeName >( #TypeName, new TSerializableTypeInstantiator< TypeName >( ) );

///////////////////////////////////////////////////////////////////////////////
