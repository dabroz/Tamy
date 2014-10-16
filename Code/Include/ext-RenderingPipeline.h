/// @file   ext-RenderingPipeline.h
/// @brief  aggregate include file for the 'ext-RenderingPipeline' project
#pragma once

#pragma region Components

// ----------------------------------------------------------------------------
// Components
// ----------------------------------------------------------------------------
#include "ext-RenderingPipeline\LocationRenderSettings.h"

#pragma endregion

#pragma region Utils

#include "ext-RenderingPipeline\RPDataProxies.h"

#pragma endregion

#pragma region ForwardRenderer

// ----------------------------------------------------------------------------
// Runtime
// ----------------------------------------------------------------------------
#include "ext-RenderingPipeline\DeferredRenderingMechanism.h"

#pragma endregion

#pragma region MaterialNodes

// ----------------------------------------------------------------------------
// MaterialNodes
// ----------------------------------------------------------------------------
#include "ext-RenderingPipeline\MNCookTorrance.h"
#include "ext-RenderingPipeline\MNPhongBlinn.h"
#include "ext-RenderingPipeline\MNEntityPtr.h"
#include "ext-RenderingPipeline\MNBoneMatrices.h"

#pragma endregion

