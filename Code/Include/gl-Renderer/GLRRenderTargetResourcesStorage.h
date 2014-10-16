/// @file   gl-Renderer/GLRRenderTargetResourcesStorage.h
/// @brief  storage for render target implementations
#pragma once

#include "core-Renderer\RenderResourceStorage.h"
#include "core-Renderer\RenderTarget2D.h"
#include "core-Renderer\RenderTargetCube.h"
#include "gl-Renderer\GLRRenderTarget2D.h"
#include "gl-Renderer\GLRRenderTargetCube.h"


///////////////////////////////////////////////////////////////////////////////

class GLRenderer;

///////////////////////////////////////////////////////////////////////////////

typedef RenderResourceStorage< GLRenderer, RenderTarget2D, GLRRenderTarget2D* > RenderTargets2DStorage;
typedef RenderResourceStorage< GLRenderer, RenderTargetCube, GLRRenderTargetCube* > RenderTargetsCubeStorage;

///////////////////////////////////////////////////////////////////////////////
