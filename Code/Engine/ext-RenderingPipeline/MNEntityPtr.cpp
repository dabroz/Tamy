#include "ext-RenderingPipeline\MNEntityPtr.h"
#include "core-Renderer\RendererUtils.h"
#include "core-Renderer\MaterialSockets.h"
#include "core-Renderer\MaterialShaderBuilder.h"
#include "core-Renderer\Renderer.h"
#include "core-Renderer\RenderState.h"
#include "core-Renderer\Shader.h"
#include "core-Renderer\GeometryComponent.h"
#include "core-Renderer\ShaderDataBuffer.h"
#include "core\ThreadSystem.h"
#include "core\Thread.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( MNEntityPtr );
   PARENT( MaterialNode );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

MNEntityPtr::MNEntityPtr( const char* name )
: MaterialNode( name )
   , m_entityPtr( NULL )
{
   if ( !IS_BEING_SERIALIZED() )
   {
      m_entityPtr = new MSColorOutput( "EntityPtr" );
      defineOutput( m_entityPtr );
   }
}

///////////////////////////////////////////////////////////////////////////////

MNEntityPtr::MNEntityPtr( const MNEntityPtr& rhs )
   : MaterialNode( rhs )
   , m_entityPtr( NULL )
{
   m_entityPtr = static_cast< MSColorOutput* >( findOutput( "EntityPtr" ) );
}

///////////////////////////////////////////////////////////////////////////////

MNEntityPtr::~MNEntityPtr()
{
   m_entityPtr = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void MNEntityPtr::onObjectLoaded()
{
   MaterialNode::onObjectLoaded();

   // find the existing inputs
   m_entityPtr = static_cast< MSColorOutput* >( findOutput( "EntityPtr" ) );

}

///////////////////////////////////////////////////////////////////////////////

void MNEntityPtr::buildMaterialShader( MaterialShaderBuilder& builder )
{
   var_entityPtr = builder.addGlobal( m_entityPtr );
}

///////////////////////////////////////////////////////////////////////////////

void MNEntityPtr::render( Renderer& renderer, const RenderState& host, RCBindShader* shaderComm, ShaderDataBuffer* constantsBuf ) const
{
   const GeometryComponent* geomComp = host.getParent();
   if ( geomComp )
   {
      const Entity* entity = geomComp->getParent();

      Vector ptr;
      RendererUtils::ptrToVec( ( void* ) entity, ptr );
      constantsBuf->accessData< Vector >( var_entityPtr ) = ptr;
   }
   else
   {
      constantsBuf->accessData< Vector >( var_entityPtr ).setZero();
   }
}

///////////////////////////////////////////////////////////////////////////////
