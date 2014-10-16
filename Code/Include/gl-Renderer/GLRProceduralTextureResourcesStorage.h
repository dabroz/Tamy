/// @file   gl-Renderer\GLRProceduralTextureResourcesStorage.h
/// @brief  OpenGL implementation of a procedural textures storage
#pragma once

#include "core-Renderer\RenderResourceStorage.h"
#include "core-Renderer\ProceduralTexture.h"


///////////////////////////////////////////////////////////////////////////////

class GLRenderer;

///////////////////////////////////////////////////////////////////////////////

typedef RenderResourceStorage< GLRenderer, ProceduralTexture, uint > ProceduralTexturesStorage;

///////////////////////////////////////////////////////////////////////////////
