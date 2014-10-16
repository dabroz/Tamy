#include "core-Renderer\MNTexture.h"
#include "core-Renderer\MaterialSockets.h"
#include "core-Renderer\Material.h"
#include "core-Renderer\Texture.h"
#include "core-Renderer\RenderableTexture.h"
#include "core-Renderer\RenderState.h"
#include "core-Renderer\MaterialShaderBuilder.h"
#include "core-Renderer\MaterialCodeNode.h"
#include "core-Renderer\Shader.h"
#include "core\ThreadSystem.h"
#include "core\Thread.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( MNTexture );
   PARENT( MaterialNode );
   PROPERTY_EDIT( "Texture", Texture*, m_texture );
   PROPERTY_EDIT( "Sampler settings", TextureSamplerSettings, m_samplerSettings );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

MNTexture::MNTexture( const char* name, Texture* texture )
: MaterialNode( name )
   , m_texture( texture )
   , m_uvInput( NULL )
   , m_output( NULL )
   , m_renderableTexture( new RenderableTexture() )
{
   if ( !IS_BEING_SERIALIZED() )
   {
      m_uvInput = new MSVec2Input( "UV" );
      defineInput( m_uvInput );

      m_output = new MSColorOutput( "Color" );
      defineOutput( m_output );
   }
}

///////////////////////////////////////////////////////////////////////////////

MNTexture::MNTexture( const MNTexture& rhs )
   : MaterialNode( rhs )
   , m_texture( rhs.m_texture )
   , m_samplerSettings( rhs.m_samplerSettings )
   , m_output( NULL )
   , m_renderableTexture( new RenderableTexture() )
{
   m_uvInput = static_cast< MSVec2Input* >( findInput( "UV" ) );
   m_output = static_cast< MSColorOutput* >( findOutput( "Color" ) );

   if ( m_texture )
   {
      m_renderableTexture->setTexture( m_texture );
   }
}

///////////////////////////////////////////////////////////////////////////////

MNTexture::~MNTexture()
{
   if ( m_renderableTexture )
   {
      m_renderableTexture->removeReference();
      m_renderableTexture = NULL;
   }

   m_texture = NULL;
   m_uvInput = NULL;
   m_output = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void MNTexture::onObjectLoaded()
{
   MaterialNode::onObjectLoaded();

   // find the existing inputs
   m_uvInput = static_cast< MSVec2Input* >( findInput( "UV" ) );
   m_output = static_cast< MSColorOutput* >( findOutput( "Color" ) );

   if ( m_texture )
   {
      m_renderableTexture->setTexture( m_texture );
   }
}

///////////////////////////////////////////////////////////////////////////////

void MNTexture::onPropertyChanged( ReflectionProperty& property )
{
   MaterialNode::onPropertyChanged( property );

   if ( property.getName() == "m_texture" )
   {
      m_renderableTexture->setTexture( m_texture );
   }
}

///////////////////////////////////////////////////////////////////////////////

void MNTexture::buildMaterialShader( MaterialShaderBuilder& builder )
{
   var_samplerName = builder.addSampler();

   if ( m_uvInput && m_uvInput->isConnected() )
   {
      const char* uvVarName = builder.getVariable( m_uvInput );

      const char* textureColorVar = builder.addVariable( m_output );
      builder.addCodeLine("<type_Color> %s = texture( %s, %s);", textureColorVar, var_samplerName.c_str(), uvVarName );
   }
}

///////////////////////////////////////////////////////////////////////////////

void MNTexture::render( Renderer& renderer, const RenderState& host, RCBindShader* shaderComm, ShaderDataBuffer* constantsBuf ) const
{
   shaderComm->setTexture( var_samplerName.c_str(), m_renderableTexture, m_samplerSettings );
}

///////////////////////////////////////////////////////////////////////////////
