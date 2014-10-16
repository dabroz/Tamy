#include "core-Renderer\CubeTexture.h"
#include "core-Renderer\Renderer.h"

// logging
#include "core\Log.h"

// file IO
#include "core\Filesystem.h"
#include "core\File.h"
#include "core\StreamBuffer.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_RESOURCE( CubeTexture, ctex, AM_BINARY );
   PROPERTY( FilePath, m_leftTex );
   PROPERTY( FilePath, m_rightTex );
   PROPERTY( FilePath, m_topTex );
   PROPERTY( FilePath, m_bottomTex );
   PROPERTY( FilePath, m_frontTex );
   PROPERTY( FilePath, m_rearTex );
   PROPERTY_EDIT( "Usage", TextureUsage, m_usage );
   PROPERTY_EDIT( "Edge length", uint, m_edgeLength );
END_RESOURCE();

///////////////////////////////////////////////////////////////////////////////

CubeTexture::CubeTexture( const FilePath& resourceName )
   : Resource( resourceName )
   , m_usage( TU_COLOR )
   , m_edgeLength( 512 )
{
   m_renderResource = RenderResource::instantiate< CubeTexture >( this );
}

///////////////////////////////////////////////////////////////////////////////

CubeTexture::~CubeTexture()
{
   m_renderResource->removeReference();
   m_renderResource = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void CubeTexture::replaceContents( Resource& rhs )
{
   CubeTexture& rhsTexture = static_cast< CubeTexture& >( rhs );

   m_leftTex = rhsTexture.m_leftTex;
   m_rightTex = rhsTexture.m_rightTex;
   m_topTex = rhsTexture.m_topTex;
   m_bottomTex = rhsTexture.m_bottomTex;
   m_frontTex = rhsTexture.m_frontTex;
   m_rearTex = rhsTexture.m_rearTex;

   m_usage = rhsTexture.m_usage;
   m_edgeLength = rhsTexture.m_edgeLength;

   // reload the resource
   m_renderResource->setDirty();
}

///////////////////////////////////////////////////////////////////////////////

bool CubeTexture::loadImage( Side side, byte*& imgBuffer, uint& outBufferSize ) const
{
   const FilePath& texPath = getImagePath( side );
   Filesystem& fs = TSingleton< Filesystem >::getInstance();

   // load the file
   File* file = fs.open( texPath, std::ios_base::in | std::ios_base::binary );
   if ( file == NULL )
   {
      LOG( "Texture: Texture file '%s' doesn't exist", texPath.c_str() );
      return false;
   }
   StreamBuffer<byte> buf( *file );

   outBufferSize = buf.size();
   if ( outBufferSize == 0 )
   {
      delete file;
      LOG( "Texture: Texture file '%s' is empty", texPath.c_str() );
      return false;
   }

   imgBuffer = new byte[outBufferSize];
   memcpy( imgBuffer, buf.getBuffer(), outBufferSize );
   delete file;

   return true;
}

///////////////////////////////////////////////////////////////////////////////

void CubeTexture::freeImage( byte* imgBuffer ) const
{
   delete [] imgBuffer;
}

///////////////////////////////////////////////////////////////////////////////

void CubeTexture::setImagePath( Side side, const FilePath& texPath )
{
   // set the new texture path
   switch ( side )
   {
   case Cube_Left:
   {
      m_leftTex = texPath;
      break;
   }

   case Cube_Right:
   {
      m_rightTex = texPath;
      break;
   }

   case Cube_Top:
   {
      m_topTex = texPath;
      break;
   }

   case Cube_Bottom:
   {
      m_bottomTex = texPath;
      break;
   }

   case Cube_Front:
   {
      m_frontTex = texPath;
      break;
   }

   case Cube_Rear:
   {
      m_rearTex = texPath;
      break;
   }
   }

   // mark the resource as dirty
   m_renderResource->setDirty();
}

///////////////////////////////////////////////////////////////////////////////

const FilePath& CubeTexture::getImagePath( Side side ) const
{
   switch ( side )
   {
   case Cube_Left: return m_leftTex;
   case Cube_Right: return m_rightTex;
   case Cube_Top: return m_topTex;
   case Cube_Bottom: return m_bottomTex;
   case Cube_Front: return m_frontTex;
   case Cube_Rear: return m_rearTex;
   }

   return m_leftTex;
}

///////////////////////////////////////////////////////////////////////////////

void CubeTexture::setUsage( TextureUsage usage )
{
   m_usage = usage;
   m_renderResource->setDirty();
}

///////////////////////////////////////////////////////////////////////////////

void CubeTexture::setEdgeLength( uint length )
{
   m_edgeLength = length;
   m_renderResource->setDirty();
}

///////////////////////////////////////////////////////////////////////////////
