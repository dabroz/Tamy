#include "core-Renderer\Texture.h"
#include "core-Renderer\Renderer.h"

// logging
#include "core\Log.h"

// file IO
#include "core\Filesystem.h"
#include "core\File.h"
#include "core\StreamBuffer.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_RESOURCE( Texture, tex, AM_BINARY );
   PROPERTY( FilePath, m_texFileName );
   PROPERTY_EDIT( "Usage", TextureUsage, m_usage );
   PROPERTY_EDIT( "Width", uint, m_width );
   PROPERTY_EDIT( "Height", uint, m_height );
END_RESOURCE();

///////////////////////////////////////////////////////////////////////////////

Texture::Texture( const FilePath& resourceName )
   : Resource( resourceName )
   , m_usage( TU_COLOR )
   , m_width( 512 )
   , m_height( 512 )
{
   m_renderResource = RenderResource::instantiate< Texture >( this );
}

///////////////////////////////////////////////////////////////////////////////

Texture::~Texture()
{
   m_renderResource->removeReference();
   m_renderResource = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void Texture::replaceContents( Resource& rhs )
{
   Texture& rhsTexture = static_cast< Texture& >( rhs );

   m_texFileName = rhsTexture.m_texFileName;
   m_usage = rhsTexture.m_usage;
   m_width = rhsTexture.m_width;
   m_height = rhsTexture.m_height;

   // reload the resource
   m_renderResource->setDirty();
}

///////////////////////////////////////////////////////////////////////////////

bool Texture::loadImage( byte*& imgBuffer, uint& outBufferSize ) const
{
   Filesystem& fs = TSingleton< Filesystem >::getInstance();

   // load the file
   File* file = fs.open( m_texFileName, std::ios_base::in | std::ios_base::binary );
   if ( file == NULL )
   {
      LOG( "Texture: Texture file '%s' doesn't exist", m_texFileName.c_str() );
      return false;
   }
   StreamBuffer<byte> buf( *file );

   outBufferSize = buf.size();
   if ( outBufferSize == 0 )
   {
      delete file;
      LOG( "Texture: Texture file '%s' is empty", m_texFileName.c_str() );
      return false;
   }

   imgBuffer = new byte[outBufferSize];
   memcpy( imgBuffer, buf.getBuffer(), outBufferSize );
   delete file;

   return true;
}

///////////////////////////////////////////////////////////////////////////////

void Texture::freeImage( byte* imgBuffer ) const
{
   delete [] imgBuffer;
}

///////////////////////////////////////////////////////////////////////////////

void Texture::setImagePath( const FilePath& texPath )
{ 
   m_texFileName = texPath; 
   m_renderResource->setDirty();
}

///////////////////////////////////////////////////////////////////////////////

void Texture::setUsage( TextureUsage usage ) 
{ 
   m_usage = usage; 
   m_renderResource->setDirty();
}

///////////////////////////////////////////////////////////////////////////////

void Texture::setImageSize( uint width, uint height )
{
   m_width = width;
   m_height = height;
   m_renderResource->setDirty();
}

///////////////////////////////////////////////////////////////////////////////
