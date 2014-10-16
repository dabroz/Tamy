#include "core-Renderer\MNTransformNormal.h"
#include "core-Renderer\RenderState.h"
#include "core-Renderer\Material.h"
#include "core-Renderer\MaterialShaderBuilder.h"
#include "core-Renderer\MaterialCodeNode.h"
#include "core-Renderer\Shader.h"
#include "core\ThreadSystem.h"
#include "core\Thread.h"



///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( MNTransformNormal );
   PARENT( MaterialNode );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

MNTransformNormal::MNTransformNormal( const char* name )
   : MaterialNode( name )
{
   if ( !IS_BEING_SERIALIZED() )
   {
      m_transform = new MSMatrixInput( "Transform" );
      m_input = new MSVec3Input( "Input" );

      std::vector< GBNodeInput< MaterialNode >* > inputs;
      inputs.push_back( m_transform );
      inputs.push_back( m_input );
      defineInputs( inputs );

      m_output = new MSVec3Output( "Output" );
      defineOutput( m_output );
   }
}

///////////////////////////////////////////////////////////////////////////////

MNTransformNormal::MNTransformNormal( const MNTransformNormal& rhs )
   : MaterialNode( rhs )
{
   m_transform = static_cast< MSMatrixInput* >( findInput( "Transform" ) );
   m_input = static_cast< MSVec3Input* >( findInput( "Input" ) );
   m_output = static_cast< MSVec3Output* >( findOutput( "Output" ) );
}

///////////////////////////////////////////////////////////////////////////////

MNTransformNormal::~MNTransformNormal()
{
}

///////////////////////////////////////////////////////////////////////////////

void MNTransformNormal::onObjectLoaded()
{
   MaterialNode::onObjectLoaded();

   m_transform = static_cast< MSMatrixInput* >( findInput( "Transform" ) );
   m_input = static_cast< MSVec3Input* >( findInput( "Input" ) );
   m_output = static_cast< MSVec3Output* >( findOutput( "Output" ) );
}

///////////////////////////////////////////////////////////////////////////////

void MNTransformNormal::buildMaterialShader( MaterialShaderBuilder& builder )
{
   if ( m_transform->isConnected() && m_input->isConnected() )
   {
      const char* transformVar = builder.getVariable( m_transform );
      const char* inputVar = builder.getVariable( m_input );
      const char* outputVar = builder.addVariable( m_output );

      builder.addCodeLine( "<type_Vector3> %s = ( <type_Mtx3x3>( %s ) * %s );", outputVar, transformVar, inputVar );
   }
}

///////////////////////////////////////////////////////////////////////////////
