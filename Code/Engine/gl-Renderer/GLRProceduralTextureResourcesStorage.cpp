#include "gl-Renderer\GLRProceduralTextureResourcesStorage.h"
#include "gl-Renderer\GLRenderer.h"
#include "gl-Renderer\GLRDefinitions.h"
#include "gl-Renderer\GLRErrorCheck.h"
#include "SOIL.h"
#include "core\Log.h"


///////////////////////////////////////////////////////////////////////////////

word g_format_1 [] = {
   GL_RGB,
   GL_RGBA32F
};

word g_format_2 [] = {
   GL_RGB,
   GL_RGBA
};

word g_format_3 [] = {
   GL_UNSIGNED_BYTE,
   GL_FLOAT
};

template<>
uint RenderResourceStorage< GLRenderer, ProceduralTexture, uint >::createResource( const ProceduralTexture* obj ) const
{
   uint textureID = 0;

   glGenTextures( 1, &textureID );
   glActiveTexture( GL_TEXTURE0 );
   glBindTexture( GL_TEXTURE_2D, textureID );
   GLR_HALT_ERRORS();

   uint textureWidth = obj->getWidth();
   uint textureHeight = obj->getHeight();
   const byte* pData = obj->getImageBuffer();
   
   const word format1 = g_format_1[obj->getFormat()];
   const word format2 = g_format_2[obj->getFormat()];
   const word format3 = g_format_3[obj->getFormat()];

   glTexImage2D( GL_TEXTURE_2D, 0, format1, textureWidth, textureHeight, 0, format2, format3, pData );
   GLR_HALT_ERRORS();

   // generate mipmaps
   const byte numMipMaps = obj->getMipmapsCount();
   if ( numMipMaps > 1 )
   {
      glTexStorage2D( GL_TEXTURE_2D, numMipMaps, format1, textureWidth, textureHeight );
      glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, textureWidth, textureHeight, format2, format3, pData );
      glGenerateMipmap( GL_TEXTURE_2D );
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
      GLR_HALT_ERRORS();
   }

   return textureID;
}

///////////////////////////////////////////////////////////////////////////////

template<>
void RenderResourceStorage< GLRenderer, ProceduralTexture, uint >::releaseResource( uint textureID ) const
{
   glDeleteTextures( 1, &textureID );
}

///////////////////////////////////////////////////////////////////////////////

template<>
void RenderResourceStorage< GLRenderer, ProceduralTexture, uint >::refreshResource( const ProceduralTexture* obj, uint& textureID ) const
{
   releaseResource( textureID );
   textureID = createResource( obj );
}

///////////////////////////////////////////////////////////////////////////////
