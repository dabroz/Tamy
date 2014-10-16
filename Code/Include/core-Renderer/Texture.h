/// @file   core-Renderer\Texture.h
/// @brief  image rendered on a surface
#pragma once

#include "core-Renderer\ShaderTexture.h"
#include "core\Resource.h"
#include "core-Renderer\RenderCommand.h"
#include "core-Renderer\RenderResource.h"
#include "core\UniqueObject.h"
#include <string>
#include <windows.h>


///////////////////////////////////////////////////////////////////////////////

class Filesystem;

///////////////////////////////////////////////////////////////////////////////

/**
 * An image that can be drawn on a rendered surface.
 */
class Texture : public Resource, public UniqueObject< Texture >
{
   DECLARE_ALLOCATOR( Texture, AM_DEFAULT );
   DECLARE_RESOURCE()

public:
   RenderResource*   m_renderResource;

private:
   FilePath          m_texFileName;
   TextureUsage      m_usage;
   
   uint              m_width;
   uint              m_height;

public:
   /**
    * Constructor.
    */
   Texture( const FilePath& resourceName = FilePath() );
   ~Texture();

   /**
    * Sets the texture path.
    *
    * @param texPath
    */
   void setImagePath( const FilePath& texPath );

   /**
    * Returns the name of the image file.
    */
   inline const FilePath& getImagePath() const {
      return m_texFileName;
   }

   /**
    * Sets new texture usage.
    *
    * @param usage
    */
   void setUsage( TextureUsage usage );

   /**
    * Returns texture usage.
    */
   inline TextureUsage getUsage() const { return m_usage; }

   /**
    * A texture is just an image. This method returns a pointer
    * to the memory buffer occupied by the image.
    * NOTE: call 'freeImage' when you're done with the image and wish to release it.
    *
    * @param outImgBuffer  output parameter that will be initialized with a pointer to the image memory buffer
    * @param outSize    output parameter that will be initialized with the buffer size (in bytes)
    */
   bool loadImage( byte*& outImgBuffer, uint& bufSize ) const;

   /**
    * Frees the image buffer that was allocated by the 'loadImage' method.
    *
    * @param imgBuffer
    */
   void freeImage( byte* imgBuffer ) const;

   /**
    * Sets the size of the texture.
    *
    *
    * @param width
    * @param height
    */
   void setImageSize( uint width, uint height );

   /**
    * Returns the width of the texture.
    */
   inline unsigned int getWidth() const { return m_width; }

   /**
    * Returns the height of the texture.
    */
   inline unsigned int getHeight() const { return m_height; }

   // -------------------------------------------------------------------------
   // Resource implementation
   // -------------------------------------------------------------------------
   void replaceContents( Resource& rhs );
};

///////////////////////////////////////////////////////////////////////////////
