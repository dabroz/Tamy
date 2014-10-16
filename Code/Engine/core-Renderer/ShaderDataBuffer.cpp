#include "core-Renderer\ShaderDataBuffer.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( ShaderDataBuffer );
   PARENT( ReflectionObject );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

ShaderDataBuffer::ShaderDataBuffer( const uint dataSize )
   : m_dataBuf( dataSize )
{
   m_dataBuf.resize( dataSize, 0 );
   m_renderResource = RenderResource::instantiate< ShaderDataBuffer >( this );
}

///////////////////////////////////////////////////////////////////////////////

ShaderDataBuffer::~ShaderDataBuffer()
{
   m_renderResource->removeReference();
   m_renderResource = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void ShaderDataBuffer::resize( const uint dataSize )
{
   m_dataBuf.resize( dataSize, 0 );
   m_renderResource->setDirty();
}

///////////////////////////////////////////////////////////////////////////////

void ShaderDataBuffer::copy( byte* outData ) const
{
   memcpy( outData, ( const byte* ) m_dataBuf, m_dataBuf.size() );
}

///////////////////////////////////////////////////////////////////////////////
