/// @file   core-Renderer\ProceduralTexture.h
/// @brief  a procedural texture
#pragma once

#include "core\MemoryRouter.h"
#include "core-Renderer\ShaderTexture.h"
#include "core\Array.h"
#include "core\Vector.h"


///////////////////////////////////////////////////////////////////////////////

enum ProceduralTextureFormat
{
   PTF_Vector,
   PTF_Float,
};

///////////////////////////////////////////////////////////////////////////////

struct PTVector
{
   byte        r;
   byte        g;
   byte        b;

   /**
    * Sets the value of the pixel from the specified vector, assuming that the vector
    * represents a normal.
    *
    * @param vec
    */
   void fromVectorAsNormal( const Vector& vec );
};

///////////////////////////////////////////////////////////////////////////////

struct PTFloat
{
   float       r;
   float       g;
   float       b;
   float       a;

   /**
   * Sets the value of the pixel from the specified float value.
   *
   * @param val
   */
   void fromFloat( float val );
};

///////////////////////////////////////////////////////////////////////////////

/**
 * A resource for storing procedurally generated textures.
 * All images are stored in the RGB format, so the buffer's size equals ( width * height * 3 ) bytes.
 */
class ProceduralTexture : public ShaderTexture
{
   DECLARE_ALLOCATOR( ProceduralTexture, AM_DEFAULT );

public:
   class RenderResource*         m_renderResource;

private:
   uint                          m_width;
   uint                          m_height;
   byte                          m_mipmapsCount;
   ProceduralTextureFormat       m_format;

   Array< byte >                 m_imageBuf;

public:
   /**
    * Constructor.
    *
    * @param width
    * @param height
    * @param format
    * @param usage
    */
   ProceduralTexture( uint width, uint height, ProceduralTextureFormat format = PTF_Vector, byte mipmapsCount = 2 );
   ~ProceduralTexture();

   /**
    * Returns the number of MipMaps.
    */
   byte getMipmapsCount() const {
      return m_mipmapsCount;
   }

   /**
    * Returns a pointer to the image buffer.
    */
   const byte* getImageBuffer() const {
      return ( const byte*)m_imageBuf;
   }

   /**
    * Returns the intended texture format.
    */
   inline ProceduralTextureFormat getFormat() const {
      return m_format;
   }

   /**
    * Gives write access to the image buffer, marking it as dirty.
    */
   byte* accessImageBuffer();

   // -------------------------------------------------------------------------
   // ShaderTexture implementation
   // -------------------------------------------------------------------------
   uint getWidth() const override {
      return m_width;
   }
   uint getHeight() const override {
      return m_height;
   }
   ShaderParam* createShaderTextureSetter( MemoryAllocator* allocator, const IDString& paramName, const TextureSamplerSettings& samplerSettings, byte renderTextureIdx ) override;
};

///////////////////////////////////////////////////////////////////////////////
