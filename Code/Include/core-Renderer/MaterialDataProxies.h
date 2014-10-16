/// @file   core-Renderer\MaterialDataProxies.h
/// @brief  proxies of more complex data types circulating within a material
#pragma once

#include "core\ReflectionObject.h"

///////////////////////////////////////////////////////////////////////////////

class MatrixArray : public ReflectionObject
{
   DECLARE_ALLOCATOR( MatrixArray, AM_DEFAULT );
   DECLARE_STRUCT();
};

///////////////////////////////////////////////////////////////////////////////
