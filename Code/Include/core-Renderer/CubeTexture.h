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
class CubeTexture : public Resource, public UniqueObject< CubeTexture >
{
   DECLARE_ALLOCATOR( CubeTexture, AM_DEFAULT );
   DECLARE_RESOURCE()

public:
   enum Side
   {
      Cube_Left,
      Cube_Right,
      Cube_Top,
      Cube_Bottom,
      Cube_Front,
      Cube_Rear,
   };

public:
   RenderResource*   m_renderResource;

private:
   FilePath          m_leftTex;
   FilePath          m_rightTex;
   FilePath          m_topTex;
   FilePath          m_bottomTex;
   FilePath          m_frontTex;
   FilePath          m_rearTex;

   TextureUsage      m_usage;
   uint              m_edgeLength;

public:
   /**
    * Constructor.
    */
   CubeTexture( const FilePath& resourceName = FilePath() );
   ~CubeTexture();

   /**
    * Sets the texture path.
    *
    * @param side
    * @param texPath
    */
   void setImagePath( Side side, const FilePath& texPath );

   /**
    * Returns the name of the image file.
    *
    * @param side
    */
   const FilePath& getImagePath( Side side ) const;

   /**
    * Sets new texture usage.
    *
    * @param usage
    */
   void setUsage( TextureUsage usage );

   /**
    * Returns texture usage.
    */
   inline TextureUsage getUsage() const {
      return m_usage;
   }

   /**
    * A texture is just an image. This method returns a pointer
    * to the memory buffer occupied by the image.
    *
    * Calling this method will set the value of the edge length.
    *
    * NOTE: call 'freeImage' when you're done with the image and wish to release it.
    *
    * @param side
    * @param outImgBuffer     output parameter that will be initialized with a pointer to the image memory buffer
    * @param outBufferSize
    */
   bool loadImage( Side side, byte*& outImgBuffer, uint& outBufferSize ) const;

   /**
    * Frees the image buffer that was allocated by the 'loadImage' method.
    *
    * @param imgBuffer
    */
   void freeImage( byte* imgBuffer ) const;   
   /**
    * Sets the the length of the texture edge.
    *
    * @param length
    */
   void setEdgeLength( uint length );

   /**
   * Returns the length of the texture edge.
   */
   inline uint getEdgeLength() const {
      return m_edgeLength;
   }

   // -------------------------------------------------------------------------
   // Resource implementation
   // -------------------------------------------------------------------------
   void replaceContents( Resource& rhs );
};

///////////////////////////////////////////////////////////////////////////////
