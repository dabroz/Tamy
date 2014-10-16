/// @file   core-Renderer.h
/// @brief  aggregate include file for the 'core-Renderer' project
#pragma once

// ----------------------------------------------------------------------------
// Defines
// ----------------------------------------------------------------------------
#include "core-Renderer\Defines.h"

// ----------------------------------------------------------------------------
// Core
// ----------------------------------------------------------------------------
#include "core-Renderer\RenderSystem.h"
#include "core-Renderer\RenderingMechanism.h"
#include "core-Renderer\Renderer.h"
#include "core-Renderer\RenderCommand.h"
#include "core-Renderer\RenderingParams.h"
// ----------------------------------------------------------------------------
// --> BasicRenderCommands
// ----------------------------------------------------------------------------
#include "core-Renderer\BasicRenderCommands.h"
#include "core-Renderer\InternalRenderCommands.h"
// ----------------------------------------------------------------------------
// --> Camera
// ----------------------------------------------------------------------------
#include "core-Renderer\Camera.h"
#include "core-Renderer\Viewport.h"
// ----------------------------------------------------------------------------
// --> RenderableSurfaces
// ----------------------------------------------------------------------------
#include "core-Renderer\ShaderTexture.h"
#include "core-Renderer\RenderTarget2D.h"
#include "core-Renderer\RenderTargetCube.h"
#include "core-Renderer\Texture.h"
#include "core-Renderer\CubeTexture.h"
#include "core-Renderer\PixelColorQuery.h"
// ----------------------------------------------------------------------------
// --> RenderTree
// ----------------------------------------------------------------------------
#include "core-Renderer\RenderTree.h"
// ----------------------------------------------------------------------------
// --> Resources
// ----------------------------------------------------------------------------
#include "core-Renderer\RenderResource.h"
#include "core-Renderer\RenderResourceStorage.h"
// ----------------------------------------------------------------------------
// --> RendererQueries
// ----------------------------------------------------------------------------
#include "core-Renderer\RQWorldPosition.h"
#include "core-Renderer\RendererQuery.h"
// ----------------------------------------------------------------------------
// --> ShadersCompiler
// ----------------------------------------------------------------------------
#include "core-Renderer\ShaderCompiler.h"
#include "core-Renderer\ShaderConstantDesc.h"
// ----------------------------------------------------------------------------
// --> ShaderRenderCommand
// ----------------------------------------------------------------------------
#include "core-Renderer\ShaderParam.h"
#include "core-Renderer\ShaderRenderCommand.h"

// ----------------------------------------------------------------------------
// Queries
// ----------------------------------------------------------------------------
#include "core-Renderer\RendererSelectionListener.h"
#include "core-Renderer\RQEntitySelection.h"
#include "core-Renderer\RQWorldPosition.h"

// ----------------------------------------------------------------------------
// GeometryComponent
// -->Core
// ----------------------------------------------------------------------------
#include "core-Renderer\Face.h"
#include "core-Renderer\LitVertex.h"
#include "core-Renderer\GeometryComponent.h"
// ----------------------------------------------------------------------------
// -->Implementations
// ----------------------------------------------------------------------------
#include "core-Renderer\StaticGeometry.h"
#include "core-Renderer\FullscreenQuad.h"
// ----------------------------------------------------------------------------
// -->Resources
// ----------------------------------------------------------------------------
#include "core-Renderer\GeometryResource.h"
#include "core-Renderer\TriangleMesh.h"
// ----------------------------------------------------------------------------
// -->ProceduralGeometry
// ----------------------------------------------------------------------------
#include "core-Renderer\ProceduralGeometryRenderer.h"
// ----------------------------------------------------------------------------
// -->Utilities
// ----------------------------------------------------------------------------
#include "core-Renderer\MeshUtils.h"
#include "core-Renderer\StaticGeometryTree.h"

// ----------------------------------------------------------------------------
// Materials
// --> Runtime
// ----------------------------------------------------------------------------
#include "core-Renderer\RenderState.h"
// ----------------------------------------------------------------------------
// --> MaterialRendererGraph
// ----------------------------------------------------------------------------
#include "core-Renderer\Material.h"
#include "core-Renderer\MaterialProfile.h"
#include "core-Renderer\MaterialNode.h"
#include "core-Renderer\MaterialSockets.h"
#include "core-Renderer\MaterialDataProxies.h"
// ----------------------------------------------------------------------------
// ----> Nodes
// ----------------------------------------------------------------------------
#include "core-Renderer\MNColor.h"
#include "core-Renderer\MNCamera.h"
#include "core-Renderer\MNFloat.h"
#include "core-Renderer\MNTexture.h"
#include "core-Renderer\MNCubeTexture.h"
#include "core-Renderer\MNVec4.h"
#include "core-Renderer\MNBool.h"
#include "core-Renderer\MNTimer.h"
#include "core-Renderer\MNTransform.h"
#include "core-Renderer\MNTransformNormal.h"
#include "core-Renderer\MNTransformVector.h"
#include "core-Renderer\MNTransformSkinVector.h"
#include "core-Renderer\MNTransformSkinNormal.h"
#include "core-Renderer\MNNormalTangentToView.h"
#include "core-Renderer\MNColorToNormal.h"
#include "core-Renderer\MNMixRGBs.h"
#include "core-Renderer\MNSplitColor.h"
#include "core-Renderer\MNInputStruct.h"
#include "core-Renderer\MNOutputStruct.h"


// ----------------------------------------------------------------------------
// Mechanisms
// -->OverlayRenderer
// ---->Elements
// ----------------------------------------------------------------------------
#include "core-Renderer\Font.h"
#include "core-Renderer\TextField.h"

// ----------------------------------------------------------------------------
// Lighting
// ----------------------------------------------------------------------------
#include "core-Renderer\Light.h"
// ----------------------------------------------------------------------------
// --> Entities
// ----------------------------------------------------------------------------
#include "core-Renderer\DirectionalLight.h"
#include "core-Renderer\PointLight.h"
#include "core-Renderer\SpotLight.h"

// ----------------------------------------------------------------------------
// --> Shaders
// ----------------------------------------------------------------------------
#include "core-Renderer\Shader.h"
#include "core-Renderer\TextureSamplerSettings.h"
#include "core-Renderer\ShaderDataBuffer.h"

// ----------------------------------------------------------------------------
// Tools
// ----------------------------------------------------------------------------
#include "core-Renderer\ShaderUtils.h"

