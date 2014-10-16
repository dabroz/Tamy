/// @file   gl-Renderer\GLRTriangleMeshResourcesStorage.h
/// @brief  TriangleMesh resources storage
#pragma once

#include "core-Renderer\RenderResourceStorage.h"
#include "core-Renderer\TriangleMesh.h"
#include "gl-Renderer\GLRTriangleMesh.h"


///////////////////////////////////////////////////////////////////////////////

class GLRenderer;

///////////////////////////////////////////////////////////////////////////////

typedef RenderResourceStorage< GLRenderer, TriangleMesh, GLRTriangleMesh* > TriangleMeshesStorage;

///////////////////////////////////////////////////////////////////////////////
