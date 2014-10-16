#include "core-Renderer\MNCamera.h"
#include "core-Renderer\Camera.h"
#include "core-Renderer\Renderer.h"
#include "core-Renderer\MaterialSockets.h"
#include "core-Renderer\RenderState.h"
#include "core-Renderer\Shader.h"
#include "core-Renderer\MaterialShaderBuilder.h"
#include "core-Renderer\ShaderDataBuffer.h"
#include "core\ThreadSystem.h"
#include "core\Thread.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( MNCamera );
   PARENT( MaterialNode );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

MNCamera::MNCamera()
   : MaterialNode( "camera" )
   , m_nearZ( NULL )
   , m_farZ( NULL )
   , m_viewportWidth( NULL )
   , m_viewportHeight( NULL )
{
   if ( !IS_BEING_SERIALIZED() )
   {
      m_nearZ = new MSFloatOutput( "nearZ" );
      m_farZ = new MSFloatOutput( "farZ" );
      m_viewportWidth = new MSFloatOutput( "width" );
      m_viewportHeight = new MSFloatOutput( "height" );

      std::vector< GBNodeOutput< MaterialNode >* > outputs;
      outputs.push_back( m_nearZ );
      outputs.push_back( m_farZ );
      outputs.push_back( m_viewportWidth );
      outputs.push_back( m_viewportHeight );

      defineOutputs( outputs );
   }
}

///////////////////////////////////////////////////////////////////////////////

MNCamera::MNCamera( const MNCamera& rhs )
   : MaterialNode( rhs )
   , m_nearZ( NULL )
   , m_farZ( NULL )
   , m_viewportWidth( NULL )
   , m_viewportHeight( NULL )
{
   // find the existing inputs
   m_nearZ = static_cast< MSFloatOutput* >( findOutput( "nearZ" ) );
   m_farZ = static_cast< MSFloatOutput* >( findOutput( "farZ" ) );
   m_viewportWidth = static_cast< MSFloatOutput* >( findOutput( "width" ) );
   m_viewportHeight = static_cast< MSFloatOutput* >( findOutput( "height" ) );
}

///////////////////////////////////////////////////////////////////////////////

MNCamera::~MNCamera()
{
   m_nearZ = NULL;
   m_farZ = NULL;
   m_viewportWidth = NULL;
   m_viewportHeight = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void MNCamera::onObjectLoaded()
{
   __super::onObjectLoaded();

   // find the existing inputs
   m_nearZ = static_cast< MSFloatOutput* >( findOutput( "nearZ" ) );
   m_farZ = static_cast< MSFloatOutput* >( findOutput( "farZ" ) );
   m_viewportWidth = static_cast< MSFloatOutput* >( findOutput( "width" ) );
   m_viewportHeight = static_cast< MSFloatOutput* >( findOutput( "height" ) );
}

///////////////////////////////////////////////////////////////////////////////

void MNCamera::buildMaterialShader( MaterialShaderBuilder& builder )
{
   var_nearZ = builder.addGlobal( m_nearZ );
   var_farZ = builder.addGlobal( m_farZ );
   var_viewportWidth = builder.addGlobal( m_viewportWidth );
   var_viewportHeight = builder.addGlobal( m_viewportHeight );
}

///////////////////////////////////////////////////////////////////////////////

void MNCamera::render( Renderer& renderer, const RenderState& host, RCBindShader* shaderComm, ShaderDataBuffer* constantsBuf ) const
{
   Camera& camera = renderer.getActiveCamera();

   constantsBuf->accessData<float>( var_nearZ ) = camera.getNearClippingPlane();
   constantsBuf->accessData<float>( var_farZ ) = camera.getFarClippingPlane();
   constantsBuf->accessData<float>( var_viewportWidth ) = ( float ) renderer.getViewportWidth();
   constantsBuf->accessData<float>( var_viewportHeight ) = ( float ) renderer.getViewportHeight();
}

///////////////////////////////////////////////////////////////////////////////
