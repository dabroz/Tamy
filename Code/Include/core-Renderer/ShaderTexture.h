/// @file   core-Renderer\ShaderTexture.h
/// @brief  common interface that allows to use various types of textures
///         as a shader param
#pragma once

#include "core\MemoryRouter.h"
#include "core-Renderer\ShaderParam.h"
#include "core\ReflectionEnum.h"
#include "core\ReflectionObject.h"


///////////////////////////////////////////////////////////////////////////////

class Shader;
class IDString;
struct TextureSamplerSettings;

///////////////////////////////////////////////////////////////////////////////

/**
 * Describes the usage of the texture.
 */
enum TextureUsage
{
   TU_COLOR,
   TU_NORMAL_MAP,
   TU_LUMINANCE,
   TU_HDR,
   TU_FLOAT,
   TU_SHADOW_MAP,

   TU_MAX
};

///////////////////////////////////////////////////////////////////////////////

class ShaderTexture : public ReflectionObject
{
   DECLARE_CLASS()

public:
   virtual ~ShaderTexture() {}

   /**
    * Returns the width of the texture.
    */
   virtual uint getWidth() const = 0;

   /**
    * Returns the height of the texture.
    */
   virtual uint getHeight() const = 0;

   // -------------------------------------------------------------------------
   // Shader params setters creating commands ( Visitor pattern )
   // -------------------------------------------------------------------------
   /**
   * Creates a texture setter for a vertex shader.
   */
   virtual ShaderParam* createShaderTextureSetter( MemoryAllocator* allocator, const IDString& paramName, const TextureSamplerSettings& samplerSettings, byte renderTextureIdx ) = 0;
};

///////////////////////////////////////////////////////////////////////////////
