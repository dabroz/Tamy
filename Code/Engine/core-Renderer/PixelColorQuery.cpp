#include "core-Renderer\PixelColorQuery.h"
#include "core\DefaultAllocator.h"


///////////////////////////////////////////////////////////////////////////////

PixelColorQuery::PixelColorQuery()
   : m_isCompleted( false )
{
}

///////////////////////////////////////////////////////////////////////////////

PixelColorQuery::~PixelColorQuery()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

SurfaceQuery::SurfaceQuery()
   : m_snapshotIdx( -1 )
   , m_buffer ( NULL )
   , m_size( 0 )
   , m_width( 0 )
   , m_height( 0 )
   , m_pixelStride( 0 )
{
}

///////////////////////////////////////////////////////////////////////////////

SurfaceQuery::~SurfaceQuery()
{
   if ( m_buffer )
   {
      MemoryRouter& memRouter = TSingleton< MemoryRouter >::getInstance();
      memRouter.dealloc( m_buffer, AM_DEFAULT );

      m_buffer = NULL;
   }
}

///////////////////////////////////////////////////////////////////////////////

void SurfaceQuery::allocateBuffer( uint width, uint height, byte pixelStride )
{
   const uint newSize = width * height * pixelStride;
   if ( newSize > m_size )
   {
      // we need to reallocate
      MemoryRouter& memRouter = TSingleton< MemoryRouter >::getInstance();
      memRouter.dealloc( m_buffer, AM_DEFAULT );
      m_buffer = (byte*)memRouter.alloc( newSize, AM_DEFAULT, &memRouter.m_defaultAllocator );
   }

   // memorize the new buffer size
   m_size = newSize;
   m_width = width;
   m_height = height;
   m_pixelStride = pixelStride;
}

///////////////////////////////////////////////////////////////////////////////

void SurfaceQuery::resetContents()
{
   if ( m_size > 0 )
   {
      memset( m_buffer, 0, m_size );
   }
}

///////////////////////////////////////////////////////////////////////////////
