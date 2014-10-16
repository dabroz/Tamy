#include "gl-Renderer\GLRCubeTextureResourcesStorage.h"
#include "gl-Renderer\GLRenderer.h"
#include "gl-Renderer\GLRDefinitions.h"
#include "gl-Renderer\GLRErrorCheck.h"
#include "SOIL.h"
#include "core\Log.h"


///////////////////////////////////////////////////////////////////////////////

template<>
uint RenderResourceStorage< GLRenderer, CubeTexture, uint >::createResource( const CubeTexture* obj ) const
{
   uint textureID = 0;

   glGenTextures( 1, &textureID );
   glActiveTexture( GL_TEXTURE0 );
   glBindTexture( GL_TEXTURE_CUBE_MAP, textureID );
   GLR_HALT_ERRORS();

   // allocate texture 
   Filesystem& fs = TSingleton< Filesystem >::getInstance();

   int textureWidth = 0;
   int textureHeight = 0;
   int channels = 0;
   const int sides[6] = {
      GL_TEXTURE_CUBE_MAP_POSITIVE_X,
      GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
      GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
      GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
      GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
      GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
   };

   for ( byte sideIdx = 0; sideIdx < 6; ++sideIdx )
   {
      std::string absImagePath = obj->getImagePath( ( CubeTexture::Side )sideIdx ).toAbsolutePath( fs );
      GLubyte* pData = SOIL_load_image( absImagePath.c_str(), &textureWidth, &textureHeight, &channels, SOIL_LOAD_AUTO );
      if ( pData == NULL )
      {
         LOG( false, "CubeTextureRenderResourceStorage: Cannot load an image file '%s'", absImagePath.c_str() );
         continue;
      }
     
      const word format = channels == 4 ? GL_RGBA : GL_RGB;
      glTexImage2D( sides[sideIdx], 0, format, textureWidth, textureHeight, 0, format, GL_UNSIGNED_BYTE, pData );
      GLR_HALT_ERRORS();

      // generate mipmaps
      const word numMipMaps = 6;
      glTexStorage2D( GL_TEXTURE_2D, numMipMaps, format, textureWidth, textureHeight );
      glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, textureWidth, textureHeight, format, GL_UNSIGNED_BYTE, pData );
      glGenerateMipmap( GL_TEXTURE_2D );
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
      GLR_HALT_ERRORS();

      // cleanup
      SOIL_free_image_data( pData );
   }

   return textureID;
}

///////////////////////////////////////////////////////////////////////////////

template<>
void RenderResourceStorage< GLRenderer, CubeTexture, uint >::releaseResource( uint textureID ) const
{
   glDeleteTextures( 1, &textureID );
}

///////////////////////////////////////////////////////////////////////////////
