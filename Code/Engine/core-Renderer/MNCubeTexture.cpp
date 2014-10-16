#include "core-Renderer\MNCubeTexture.h"
#include "core-Renderer\MaterialSockets.h"
#include "core-Renderer\Material.h"
#include "core-Renderer\CubeTexture.h"
#include "core-Renderer\RenderableCubeTexture.h"
#include "core-Renderer\RenderState.h"
#include "core-Renderer\MaterialShaderBuilder.h"
#include "core-Renderer\MaterialCodeNode.h"
#include "core-Renderer\Shader.h"
#include "core\ThreadSystem.h"
#include "core\Thread.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( MNCubeTexture );
   PARENT( MaterialNode );
   PROPERTY_EDIT( "Texture", CubeTexture*, m_texture );
   PROPERTY_EDIT( "Sampler settings", TextureSamplerSettings, m_samplerSettings );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

MNCubeTexture::MNCubeTexture( const char* name, CubeTexture* cubeTexture )
   : MaterialNode( name )
   , m_texture( cubeTexture )
   , m_uvwInput( NULL )
   , m_output( NULL )
   , m_renderableTexture( new RenderableCubeTexture() )
{
   if ( !IS_BEING_SERIALIZED() )
   {
      m_uvwInput = new MSVec3Input( "UVW" );
      defineInput( m_uvwInput );

      m_output = new MSColorOutput( "Color" );
      defineOutput( m_output );
   }
}

///////////////////////////////////////////////////////////////////////////////

MNCubeTexture::MNCubeTexture( const MNCubeTexture& rhs )
   : MaterialNode( rhs )
   , m_texture( rhs.m_texture )
   , m_samplerSettings( rhs.m_samplerSettings )
   , m_output( NULL )
   , m_renderableTexture( new RenderableCubeTexture() )
{
   m_uvwInput = static_cast< MSVec3Input* >( findInput( "UVW" ) );
   m_output = static_cast< MSColorOutput* >( findOutput( "Color" ) );

   if ( m_texture )
   {
      m_renderableTexture->setTexture( m_texture );
   }
}

///////////////////////////////////////////////////////////////////////////////

MNCubeTexture::~MNCubeTexture()
{
   if ( m_renderableTexture )
   {
      m_renderableTexture->removeReference();
      m_renderableTexture = NULL;
   }

   m_texture = NULL;
   m_uvwInput = NULL;
   m_output = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void MNCubeTexture::onObjectLoaded()
{
   MaterialNode::onObjectLoaded();

   // find the existing inputs
   m_uvwInput = static_cast< MSVec3Input* >( findInput( "UVW" ) );
   m_output = static_cast< MSColorOutput* >( findOutput( "Color" ) );

   if ( m_texture )
   {
      m_renderableTexture->setTexture( m_texture );
   }
}

///////////////////////////////////////////////////////////////////////////////

void MNCubeTexture::onPropertyChanged( ReflectionProperty& property )
{
   MaterialNode::onPropertyChanged( property );

   if ( property.getName() == "m_texture" )
   {
      m_renderableTexture->setTexture( m_texture );
   }
}

///////////////////////////////////////////////////////////////////////////////

void MNCubeTexture::buildMaterialShader( MaterialShaderBuilder& builder )
{
   var_samplerName = builder.addCubeSampler();

   if ( m_uvwInput && m_uvwInput->isConnected() )
   {
      const char* uvwVarName = builder.getVariable( m_uvwInput );

      const char* textureColorVar = builder.addVariable( m_output );
      builder.addCodeLine("<type_Color> %s = texture( %s, %s);", textureColorVar, var_samplerName.c_str(), uvwVarName );
   }
}

///////////////////////////////////////////////////////////////////////////////

void MNCubeTexture::render( Renderer& renderer, const RenderState& host, RCBindShader* shaderComm, ShaderDataBuffer* constantsBuf ) const
{
   shaderComm->setTexture( var_samplerName.c_str(), m_renderableTexture, m_samplerSettings );
}

///////////////////////////////////////////////////////////////////////////////
