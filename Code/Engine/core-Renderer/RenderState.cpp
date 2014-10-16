#include "core-Renderer\RenderState.h"
#include "core-Renderer\GeometryComponent.h"
#include "core-Renderer\Material.h"
#include "core-Renderer\MaterialProfile.h"
#include "core-Renderer\MaterialNode.h"
#include "core-Renderer\Renderer.h"
#include "core-Renderer\RenderStateChangeTracker.h"
#include "core-Renderer\RenderStateCommand.h"
#include "core-Renderer\Shader.h"
#include "core-Renderer\RendererUtils.h"
#include "core-Renderer\RenderingContext.h"
#include "core-Renderer\ShaderUtils.h"
#include "core-Renderer\ShaderDataBuffer.h"

// files
#include "core\FilesystemUtils.h"
#include "core\FilePath.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( RenderState );
   VERSION_NO( 1 );
   PARENT( ReflectionObject );
   PROPERTY_EDIT( "Name", std::string, m_name );
   PROPERTY_EDIT( "Material", Material*, m_material );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

RenderState::RenderState( const char* name )
   : m_name( name )
   , m_material( NULL )
   , m_dataBuf( NULL )
   , m_parent( NULL )
   , m_routineConstantBuffers( LAST_SHADER_ROUTINE )
   , m_singleColorShader( NULL )
   , m_selectionQueryShader( NULL )
   , m_shadowMapShader( NULL )
   , m_activeInRuntime( false )
{
   m_selectionPassDataBuffer = new ShaderDataBuffer( sizeof( Color ) );
   m_selectionQueryPassDataBuffer = new ShaderDataBuffer( sizeof( Vector ) );
   m_routineConstantBuffers.resize( LAST_SHADER_ROUTINE, NULL );
}

///////////////////////////////////////////////////////////////////////////////

RenderState::RenderState( const RenderState& rhs )
   : ReflectionObject( rhs )
   , m_name( rhs.m_name )
   , m_material( rhs.m_material )
   , m_dataBuf( NULL )
   , m_parent( NULL )
   , m_routineConstantBuffers( LAST_SHADER_ROUTINE )
   , m_singleColorShader( NULL )
   , m_selectionQueryShader( NULL )
   , m_shadowMapShader( NULL )
   , m_activeInRuntime( false )
{
   m_selectionPassDataBuffer = new ShaderDataBuffer( sizeof( Color ) );
   m_selectionQueryPassDataBuffer = new ShaderDataBuffer( sizeof( Vector ) );
   m_routineConstantBuffers.resize( LAST_SHADER_ROUTINE, NULL );
}

///////////////////////////////////////////////////////////////////////////////

RenderState::~RenderState()
{
   m_selectionPassDataBuffer->removeReference();
   m_selectionPassDataBuffer = NULL;

   m_selectionQueryPassDataBuffer->removeReference();
   m_selectionQueryPassDataBuffer = NULL;

   if ( m_singleColorShader )
   {
      m_singleColorShader->removeReference();
      m_singleColorShader = NULL;
   }

   if ( m_selectionQueryShader )
   {
      m_selectionQueryShader->removeReference();
      m_selectionQueryShader = NULL;
   }

   if ( m_shadowMapShader )
   {
      m_shadowMapShader->removeReference();
      m_shadowMapShader = NULL;
   }

   m_material = NULL;
   m_parent = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void RenderState::setMaterial( Material* material )
{
   NOTIFY_PROPERTY_CHANGE( m_material );

   m_material = material;
}

///////////////////////////////////////////////////////////////////////////////

void RenderState::onPrePropertyChanged( ReflectionProperty& property )
{
   ReflectionObject::onPrePropertyChanged( property );

   if ( property.getName() == "m_material" )
   {
      if ( m_activeInRuntime )
      {
         deinitializeMaterial();
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void RenderState::onPropertyChanged( ReflectionProperty& property )
{
   ReflectionObject::onPropertyChanged( property );

   if ( property.getName() == "m_material" )
   {
      if ( m_activeInRuntime )
      {
         initializeMaterial();
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void RenderState::onInitialize( const GeometryComponent* parent )
{   
   // set the parent of the state - we need that reference during for rendering purposes
   ASSERT( m_parent == NULL );
   m_parent = parent;

   initializeMaterial();

   m_activeInRuntime = true;
}

///////////////////////////////////////////////////////////////////////////////

void RenderState::onDeinitialize()
{
   m_activeInRuntime = false;

   // reset the parent
   m_parent = NULL;

   deinitializeMaterial();
}

///////////////////////////////////////////////////////////////////////////////

void RenderState::initializeMaterial()
{
   if ( !m_material )
   {
      return;
   }

   // create new runtime data buffer
   ASSERT( !m_dataBuf );
   m_dataBuf = new RuntimeDataBuffer();

   m_material->addReference();
   MaterialProfile* profile = m_material->accessProfile();

   // initialize the nodes
   for ( uint routineIdx = 0; routineIdx < LAST_SHADER_ROUTINE; ++routineIdx )
   {
      profile->attachGraphListener( ( ShaderRoutine ) routineIdx, this );

      Array< MaterialNode* >& nodesQueue = profile->m_nodesQueue[routineIdx];
      uint count = nodesQueue.size();
      for ( uint i = 0; i < count; ++i )
      {
         MaterialNode* node = nodesQueue[i];
         node->createLayout( *this );
      }
   }

   // initialize routine data buffers
   for ( byte i = 0; i < LAST_SHADER_ROUTINE; ++i )
   {
      ASSERT( m_routineConstantBuffers[i] == NULL );
      m_routineConstantBuffers[i] = new ShaderDataBuffer( profile->m_routineConstantBufferSize[i] );
   }

   rebuildCustomShaders();
}

///////////////////////////////////////////////////////////////////////////////

void RenderState::deinitializeMaterial()
{
   if ( !m_material )
   {
      ASSERT_MSG( m_dataBuf == NULL, "RenderState: If there was no material, then the runtime data buffer shouldn't be initialized" );
      return;
   }

   // deinitialize the nodes
   MaterialProfile* profile = m_material->accessProfile();
   for ( uint routineIdx = 0; routineIdx < LAST_SHADER_ROUTINE; ++routineIdx )
   {
      profile->detachGraphListener( ( ShaderRoutine ) routineIdx, this );

      Array< MaterialNode* >& nodesQueue = profile->m_nodesQueue[routineIdx];
      uint count = nodesQueue.size();
      for ( uint i = 0; i < count; ++i )
      {
         MaterialNode* node = nodesQueue[i];
         node->destroyLayout( *this );
      }
   }

   // deinitialize routine data buffers
   for ( byte i = 0; i < LAST_SHADER_ROUTINE; ++i )
   {
      if ( m_routineConstantBuffers[i] )
      {
         m_routineConstantBuffers[i]->removeReference();
         m_routineConstantBuffers[i] = NULL;
      }
   }

   // release the material
   m_material->removeReference();

   delete m_dataBuf;
   m_dataBuf = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void RenderState::onNodeCreated( MaterialNode* node )
{
   node->createLayout( *this );

   rebuildCustomShaders();
}

///////////////////////////////////////////////////////////////////////////////

void RenderState::onNodeRemoved( MaterialNode* node )
{
   node->destroyLayout( *this );

   rebuildCustomShaders();
}

///////////////////////////////////////////////////////////////////////////////

RCBindShader* RenderState::renderDefault( Renderer& renderer ) const
{
   if ( !m_material )
   {
      return NULL;
   }

   const MaterialProfile* profile = m_material->getProfile();

   const Shader* shader = profile->getShader();
   if ( !shader )
   {
      return NULL;
   }

   // TODO: these render states - we're gonna have separate passes for rendering transparency etc.
   // set the render state
   SAVE_RENDER_STATE( renderer );
   if ( profile->isTransparent() )
   {
      CHANGE_RENDER_STATE( renderer, RSSetAlphaTest( true ) );
   }

   if ( profile->isDoubleSided() )
   {
      CHANGE_RENDER_STATE( renderer, RSSetFaceCulling( CULL_NONE ) );
   }

   // set the render command
   RCBindShader* shaderComm = new ( renderer.rtComm() ) RCBindShader( shader, renderer );

   for ( uint routineIdx = 0; routineIdx < LAST_SHADER_ROUTINE; ++routineIdx )
   {
      updateMaterialNodes( routineIdx, renderer, shaderComm );
   }

   return shaderComm;
}

///////////////////////////////////////////////////////////////////////////////

RCBindShader* RenderState::renderSelectionQuery( Renderer& renderer ) const
{
   if ( !m_selectionQueryShader )
   {
      return NULL;
   }

   SAVE_RENDER_STATE( renderer );

   // set the render command
   RCBindShader* shaderComm = new ( renderer.rtComm() ) RCBindShader( m_selectionQueryShader, renderer );
   if ( m_parent )
   {
      const Entity* entity = m_parent->getParent();

      Vector ptr;
      RendererUtils::ptrToVec( ( void* ) entity, ptr );
      m_selectionQueryPassDataBuffer->accessData< Vector >() = ptr;
   }
   else
   {
      m_selectionQueryPassDataBuffer->accessData< Vector >() = Vector_ZERO;
   }
   shaderComm->setDataBuf( "FragmentShaderConstants", m_selectionQueryPassDataBuffer );

   for ( uint routineIdx = 0; routineIdx < SHADER_FRAGMENT; ++routineIdx )
   {
      updateMaterialNodes( routineIdx, renderer, shaderComm );
   }

   return shaderComm;
}

///////////////////////////////////////////////////////////////////////////////

RCBindShader* RenderState::renderSelection( Renderer& renderer ) const
{
   if ( !m_singleColorShader )
   {
      return NULL;
   }

   SAVE_RENDER_STATE( renderer );

   // set the render command
   RCBindShader* shaderComm = new ( renderer.rtComm() ) RCBindShader( m_singleColorShader, renderer );

   const RenderingContext& context = renderer.getContext();
   m_selectionPassDataBuffer->accessData< Color >() = context.m_color;
   shaderComm->setDataBuf( "FragmentShaderConstants", m_selectionPassDataBuffer );
   
   for ( uint routineIdx = 0; routineIdx < SHADER_FRAGMENT; ++routineIdx )
   {
      updateMaterialNodes( routineIdx, renderer, shaderComm );
   }

   return shaderComm;
}

///////////////////////////////////////////////////////////////////////////////

RCBindShader* RenderState::rendererShadowMap( Renderer& renderer ) const
{
   if ( !m_shadowMapShader )
   {
      return NULL;
   }

   SAVE_RENDER_STATE( renderer );

   // set the render command
   RCBindShader* shaderComm = new ( renderer.rtComm() ) RCBindShader( m_shadowMapShader, renderer );
   const RenderingContext& context = renderer.getContext();
   for ( uint routineIdx = 0; routineIdx < SHADER_FRAGMENT; ++routineIdx )
   {
      updateMaterialNodes( routineIdx, renderer, shaderComm );
   }

   return shaderComm;
}

///////////////////////////////////////////////////////////////////////////////

void RenderState::updateMaterialNodes( uint routineIdx, Renderer& renderer, RCBindShader* shaderComm ) const
{
   if ( !m_material )
   {
      return;
   }

   const MaterialProfile* profile = m_material->getProfile();
   const Array< MaterialNode* >& nodesQueue = profile->m_nodesQueue[routineIdx];
   ShaderDataBuffer* constantsBuf = m_routineConstantBuffers[routineIdx];

   uint count = nodesQueue.size();
   for ( uint i = 0; i < count; ++i )
   {
      MaterialNode* node = nodesQueue[i];
      node->render( renderer, *this, shaderComm, constantsBuf );
   }

   // bind the constants buffer
   ASSERT( m_routineConstantBuffers[routineIdx] );
   if ( constantsBuf->getSize() > 0 )
   {
      shaderComm->setDataBuf( profile->m_routineConstantBufferName[routineIdx], constantsBuf );
   }
}

///////////////////////////////////////////////////////////////////////////////

void RenderState::postRender( Renderer& renderer ) const
{
   new ( renderer.rtComm() ) RCUnbindShader();
   RESTORE_RENDER_STATE( renderer );
}

///////////////////////////////////////////////////////////////////////////////

void RenderState::rebuildCustomShaders()
{
   // create custom shaders
   const MaterialProfile* defaultProfile = m_material->getProfile();
   const Shader* defaultProfileShader = defaultProfile->getShader();

   createShader( defaultProfileShader, m_singleColorShader, "_singleColor" );
   createShader( defaultProfileShader, m_selectionQueryShader, "_selectionQuery" );
   createShader( defaultProfileShader, m_shadowMapShader, "_shadowMap" );

   FilePath singleColorShaderTemplate( "/Renderer/Shaders/RenderingPipeline/singleColorShader.glsl" );
   FilePath shadowmapShaderTemplate( "/Renderer/Shaders/RenderingPipeline/shadowMapShader.glsl" );

   ShaderUtils::swapRoutineCode( SHADER_FRAGMENT, m_singleColorShader, singleColorShaderTemplate );
   ShaderUtils::swapRoutineCode( SHADER_FRAGMENT, m_selectionQueryShader, singleColorShaderTemplate );
   ShaderUtils::swapRoutineCode( SHADER_FRAGMENT, m_shadowMapShader, shadowmapShaderTemplate );

   m_singleColorShader->build();
   m_selectionQueryShader->build();
   m_shadowMapShader->build();
}

///////////////////////////////////////////////////////////////////////////////

void RenderState::createShader( const Shader* defaultProfileShader, Shader*& shader, const char* postfix ) const
{
   Filesystem& fs = TSingleton< Filesystem >::getInstance();

   FilePath scriptPath, shaderPath;
   defaultProfileShader->getScriptPath().appendPostfix( postfix, scriptPath );
   defaultProfileShader->getFilePath().appendPostfix( postfix, shaderPath );

   std::string absScriptPath = defaultProfileShader->getScriptPath().toAbsolutePath( fs );
   std::string absCustomScriptPath = scriptPath.toAbsolutePath( fs );
   FilesystemUtils::copyFile( absScriptPath.c_str(), absCustomScriptPath.c_str(), false );

   if ( !shader )
   {
      shader = new Shader( shaderPath );
   }

   shader->setPath( scriptPath );
}

///////////////////////////////////////////////////////////////////////////////
