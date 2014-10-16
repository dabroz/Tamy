#include "core-Renderer\MNTransformVector.h"
#include "core-Renderer\RenderState.h"
#include "core-Renderer\Material.h"
#include "core-Renderer\MaterialShaderBuilder.h"
#include "core-Renderer\MaterialCodeNode.h"
#include "core-Renderer\Shader.h"
#include "core\ThreadSystem.h"
#include "core\Thread.h"



///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( MNTransformVector );
   PARENT( MaterialNode );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

MNTransformVector::MNTransformVector( const char* name )
   : MaterialNode( name )
{
   if ( !IS_BEING_SERIALIZED() )
   {
      m_transform = new MSMatrixInput( "Transform" );
      m_input = new MSVec4Input( "Input" );

      std::vector< GBNodeInput< MaterialNode >* > inputs;
      inputs.push_back( m_transform );
      inputs.push_back( m_input );
      defineInputs( inputs );

      m_output = new MSVec4Output( "Output" );
      defineOutput( m_output );
   }
}

///////////////////////////////////////////////////////////////////////////////

MNTransformVector::MNTransformVector( const MNTransformVector& rhs )
   : MaterialNode( rhs )
{
   m_transform = static_cast< MSMatrixInput* >( findInput( "Transform" ) );
   m_input = static_cast< MSVec4Input* >( findInput( "Input" ) );
   m_output = static_cast< MSVec4Output* >( findOutput( "Output" ) );
}

///////////////////////////////////////////////////////////////////////////////

MNTransformVector::~MNTransformVector()
{
}

///////////////////////////////////////////////////////////////////////////////

void MNTransformVector::onObjectLoaded()
{
   MaterialNode::onObjectLoaded();

   m_transform = static_cast< MSMatrixInput* >( findInput( "Transform" ) );
   m_input = static_cast< MSVec4Input* >( findInput( "Input" ) );
   m_output = static_cast< MSVec4Output* >( findOutput( "Output" ) );
}

///////////////////////////////////////////////////////////////////////////////

void MNTransformVector::buildMaterialShader( MaterialShaderBuilder& builder )
{
   if ( m_transform->isConnected() && m_input->isConnected() )
   {
      const char* transformVar = builder.getVariable( m_transform );
      const char* inputVar = builder.getVariable( m_input );
      const char* outputVar = builder.addVariable( m_output );

      builder.addCodeLine("<type_Vector4> %s = (%s * %s);", outputVar, transformVar, inputVar );
   }
}

///////////////////////////////////////////////////////////////////////////////
