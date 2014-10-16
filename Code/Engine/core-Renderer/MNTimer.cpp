#include "core-Renderer\MNTimer.h"
#include "core-Renderer\MaterialSockets.h"
#include "core-Renderer\RenderState.h"
#include "core-Renderer\MaterialShaderBuilder.h"
#include "core-Renderer\Shader.h"
#include "core-Renderer\ShaderDataBuffer.h"
#include "core\ThreadSystem.h"
#include "core\Thread.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( MNTimer );
   PARENT( MaterialNode );
   PROPERTY_EDIT( "period", float, m_period );
   PROPERTY_EDIT( "amplitude", float, m_amplitude );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

MNTimer::MNTimer()
   : MaterialNode( "timer" )
   , m_period( 1.0f )
   , m_amplitude( 1.0f )
   , m_time( NULL )
   , m_sinTime( NULL )
{
   if ( !IS_BEING_SERIALIZED() )
   {
      m_time = new MSFloatOutput( "t" );
      m_sinTime = new MSFloatOutput( "sin(t * p) * a" );

      std::vector< GBNodeOutput< MaterialNode >* > outputs;
      outputs.push_back( m_time );
      outputs.push_back( m_sinTime );

      defineOutputs( outputs );
   }
}

///////////////////////////////////////////////////////////////////////////////

MNTimer::MNTimer( const MNTimer& rhs )
   : MaterialNode( rhs )
   , m_period( rhs.m_period )
   , m_amplitude( rhs.m_amplitude )
   , m_time( NULL )
   , m_sinTime( NULL )
{
   m_time = static_cast< MSFloatOutput* >( findOutput( "t" ) );
   m_sinTime = static_cast< MSFloatOutput* >( findOutput( "sin(t * p) * a" ) );
}

///////////////////////////////////////////////////////////////////////////////

MNTimer::~MNTimer()
{
   m_time = NULL;
   m_sinTime = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void MNTimer::onObjectLoaded()
{
   __super::onObjectLoaded();

   // find the existing inputs
   m_time = static_cast< MSFloatOutput* >( findOutput( "t" ) );
   m_sinTime = static_cast< MSFloatOutput* >( findOutput( "sin(t * p) * a" ) );
}

///////////////////////////////////////////////////////////////////////////////

void MNTimer::buildMaterialShader( MaterialShaderBuilder& builder )
{
   var_time = builder.addGlobal( m_time );
   var_sinTime = builder.addGlobal( m_sinTime );
}

///////////////////////////////////////////////////////////////////////////////

void MNTimer::render( Renderer& renderer, const RenderState& host, RCBindShader* shaderComm, ShaderDataBuffer* constantsBuf ) const
{
   float currTime = ( float ) m_timer.getCurrentTime();
   float sinTime = sin( currTime * m_period ) * m_amplitude;

   constantsBuf->accessData< float >( var_time ) = currTime;
   constantsBuf->accessData< float >( var_sinTime ) = sinTime;
}

///////////////////////////////////////////////////////////////////////////////
